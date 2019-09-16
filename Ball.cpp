#include "Ball.h"
#include "Shader.h"

Ball::Ball(std::string id, ResourceManager* r, std::vector<PlayableChar*> p)
{
    ballState = SPAWN;
    res = r;
    modelID = id;
    XMStoreFloat4x4(&World, XMMatrixIdentity());

    Scale = XMFLOAT3(2.f, 2.f, 2.f);
    ballHeight = Scale.x / 2.f;
    Translation = XMFLOAT3(0.f, ballHeight,0.f);
    
    Rotation = XMFLOAT3(0.f, 0.f, 0.f);

    Velocity.y = 10.f;
    bounceFactor = 0.75f;
    bounceTime = 1.f;

    players = p;

    hitBox.Radius = 1.f;

    Color = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
}

Ball::~Ball()
{

}

void Ball::Update(float deltaTime)
{

    if (ballState == BallState::SPAWN)
    {
        bounceTime += deltaTime;

        if (Translation.y >= ballHeight)
        {
            Translation.y = Velocity.y * bounceTime - (GRAVITY / 2.f * bounceTime * bounceTime) + ballHeight;
        }
        else
        {
            bounceTime = 0.f;
            Translation.y = ballHeight;
            Velocity.y = Velocity.y * 0.65f;

            if (Velocity.y < 0.1f)
            {
                resetBall();
            }
            
        }
    }
    else if (ballState == BallState::FREEZE)
    {

        spawnTime += deltaTime;

        if (spawnTime >= SPAWN_FREEZE)
        {
            spawnTime = 0.f;
            ballState = BallState::INPLAY;
        }

    }
    else if (ballState == BallState::INPLAY)
    {
        Translation.x += Velocity.x * deltaTime;
        Translation.z += Velocity.z * deltaTime;

        hitBox.Center.x = Translation.x;
        hitBox.Center.y = Translation.y;
        hitBox.Center.z = Translation.z;

        /*inc velocity*/

        Velocity.x += Velocity.x * 0.1f * deltaTime;
        Velocity.z += Velocity.z * 0.1f * deltaTime;

        Velocity.x = min(Velocity.x, MAX_VELOCITY);
        Velocity.z = min(Velocity.z, MAX_VELOCITY);

        /*check collision*/

        for (int index = 0; index < players.size(); ++index)
        {
            if (hitBox.Intersects(players[index]->hitBox))
            {
                DBOUT("Player " << index << " touched the ball\n");

                Color = players[index]->Color;
                lastTouch = index;

                if (players[index]->Orientation)
                {
                    Translation.x -= Velocity.x * deltaTime;
                    Translation.z -= Velocity.z * deltaTime;
                    Velocity.x *= -1;
                }
                else
                {

                    Translation.x -= Velocity.x * deltaTime;
                    Translation.z -= Velocity.z * deltaTime;
                    Velocity.z *= -1;
                }
            }
        }


        /*check if ball not defended*/
        if (Translation.z <= -BALL_BORDER)
        {
            resetB = true;
            DBOUT("Player 1 hit by Player " << lastTouch << "\n");
            players[0]->controllingPlayer->hp--;
        }
        else if (Translation.z >= BALL_BORDER)
        {
            resetB = true;
            DBOUT("Player 2 hit by Player " << lastTouch << "\n");
            players[1]->controllingPlayer->hp--;
        }

        if (Translation.x <= -BALL_BORDER)
        {
            resetB = true;
            DBOUT("Player 3 hit by Player " << lastTouch << "\n");
            players[2]->controllingPlayer->hp--;
        }
        else if (Translation.x >= BALL_BORDER)
        {
            resetB = true;
            DBOUT("Player 4 hit by Player " << lastTouch << "\n");
            players[3]->controllingPlayer->hp--;
        }

        if (resetB)
        {
            
            ballState = BallState::RESET;
            distanceV = Translation;

            DBOUT("HP LEVEL:\n");
            for (auto& p : players)
            {
                if (p->controllingPlayer != nullptr)
                    DBOUT("Player " << p->controllingPlayer->pID << ": " << p->controllingPlayer->hp << "\n");
            }
        }

    }
    else if (ballState == BallState::RESET)
    {

        /*check if all but one player dead*/



        /*move ball back to middle and do random velocity*/

        resetTime += deltaTime;

        if (resetTime > 1.f)
        {
            resetBall();
        }
        else
        {
            /*interpolate to mid position*/

            Translation.x = distanceV.x * (1.f - resetTime);
            Translation.z = distanceV.z * (1.f - resetTime);
            Translation.y = (-48.f * resetTime * resetTime) + 48.f*resetTime + ballHeight;

        }

    }



}

void Ball::Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX shadowT)
{

    Model* model = res->getModel(modelID);

    XMStoreFloat4x4(&World, XMMatrixScaling(Scale.x, Scale.y, Scale.z) * XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z) * XMMatrixTranslation(Translation.x, Translation.y, Translation.z));

    XMMATRIX view = c->getView();
    XMMATRIX proj = c->getProj();
    XMMATRIX viewProj = c->getViewProj();

    XMMATRIX world = XMLoadFloat4x4(&World);
    XMMATRIX wvp = world * view * proj;

    XMMATRIX wit = DXMath::InverseTranspose(world);

    UINT stride = sizeof(Vertex::Standard);
    UINT offset = 0;

    D3DX11_TECHNIQUE_DESC techDesc;
    Shaders::basicTextureShader->BasicStaticColor->GetDesc(&techDesc);

    for (auto& m : model->meshes)
    {

        for (UINT p = 0; p < techDesc.Passes; p++)
        {
            deviceContext->IASetVertexBuffers(0, 1, &m->vertex, &stride, &offset);
            deviceContext->IASetIndexBuffer(m->index, DXGI_FORMAT_R32_UINT, 0);

            Shaders::basicTextureShader->SetWorld(world);
            Shaders::basicTextureShader->SetWorldViewProj(wvp);
            Shaders::basicTextureShader->SetWorldInvTranspose(wit);
            Shaders::basicTextureShader->SetMaterial(m->material);
            Shaders::basicTextureShader->SetStaticColor(Color);
            Shaders::basicTextureShader->SetShadowTransform(world * shadowT);

            Shaders::basicTextureShader->BasicStaticColor->GetPassByIndex(p)->Apply(0, deviceContext);
            deviceContext->DrawIndexed((UINT)(m->indices.size()), 0, 0);
        }


    }

}

void Ball::ShadowDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX lightView, XMMATRIX lightProj)
{

    Model* model = res->getModel(modelID);

    XMMATRIX _r = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
    XMMATRIX _t = XMMatrixTranslation(Translation.x, Translation.y, Translation.z);
    XMMATRIX _s = XMMatrixScaling(Scale.x, Scale.y, Scale.z);

    XMStoreFloat4x4(&World, _s * model->axisRot * _r * _t);

    XMMATRIX view = lightView;
    XMMATRIX proj = lightProj;
    XMMATRIX viewProj = XMMatrixMultiply(view, proj);

    XMMATRIX world = XMLoadFloat4x4(&World);
    XMMATRIX wvp = world * view * proj;

    XMMATRIX wit = DXMath::InverseTranspose(world);

    UINT stride = sizeof(Vertex::Standard);
    UINT offset = 0;

    /*select tech*/
    ID3DX11EffectTechnique* tech = 0;

    tech = Shaders::shadowMapShader->ShadowMapTech;

    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc(&techDesc);

    for (auto& m : model->meshes)
    {
        for (UINT p = 0; p < techDesc.Passes; p++)
        {
            deviceContext->IASetVertexBuffers(0, 1, &m->vertex, &stride, &offset);
            deviceContext->IASetIndexBuffer(m->index, DXGI_FORMAT_R32_UINT, 0);

            /*set per object constants based on used shader and technique*/

            Shaders::shadowMapShader->SetWorld(world);
            Shaders::shadowMapShader->SetWorldInvTranspose(wit);
            Shaders::shadowMapShader->SetWorldViewProj(wvp);
            Shaders::shadowMapShader->SetViewProj(viewProj);
            Shaders::shadowMapShader->SetDiffuseMap(res->getTexture(m->diffuseMapID));

            /*apply and draw*/
            tech->GetPassByIndex(p)->Apply(0, deviceContext);
            deviceContext->DrawIndexed((UINT)(m->indices.size()), 0, 0);
        }

    }



}


void Ball::resetBall()
{
    ballState = BallState::FREEZE;
    resetTime = 0.f;
    Velocity.y = 0.f;

    Translation = XMFLOAT3(0.f, ballHeight, 0.f);

    /*random direction*/
    Velocity.z = 25.f;
    Velocity.x = 17.f;
 
    resetB = false;
}