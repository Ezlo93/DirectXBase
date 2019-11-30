#include "Ball.h"
#include "Shader.h"

Ball::Ball(std::string id, ResourceManager* r, std::vector<PlayableChar*> p)
{
    ballState = BallState::SPAWN;
    res = r;
    modelID = id;
    XMStoreFloat4x4(&World, XMMatrixIdentity());

    Scale = XMFLOAT3(2.f, 2.f, 2.f);
    ballHeight = Scale.x / 2.f;
    Translation = XMFLOAT3(0.f, ballHeight,0.f);
    
    Rotation = XMFLOAT3(0.f, 0.f, 0.f);

    bounceFactor = 0.75f;
    bounceTime = 1.f;

    players = p;

    hitBox.Radius = 1.f;

    maxPlayArea.Center = XMFLOAT3(0, 0, 0);
    maxPlayArea.Extents = XMFLOAT3(BALL_BORDER * 1.2f, BALL_BORDER * 1.2f, BALL_BORDER);

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
            res->getSound()->add("ball_hit");
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
        XMFLOAT3 pPos = Translation;

        inplayTime += deltaTime;

        Translation.x += Velocity.x * Direction.x * deltaTime;
        Translation.z += Velocity.x * Direction.z * deltaTime;

        hitBox.Center.x = Translation.x;
        hitBox.Center.y = Translation.y;
        hitBox.Center.z = Translation.z;

        /*inc velocity*/

        if (inplayTime < 25)
        {
            Velocity.x = START_VELOCITY + (inplayTime / 25.f) * (150-START_VELOCITY);
        }
        else if (inplayTime < 45)
        {
            Velocity.x = 150.f + ((inplayTime - 25.f) / 20.f) * 50.f;
        }
        else if (inplayTime < 120)
        {
            Velocity.x = 200.f + ((inplayTime - 45) / 120.f) * 150.f;
        }
        else
        {
            Velocity.x = 350.f;
        }
        Velocity.x = min(Velocity.x, MAX_VELOCITY);

        //Velocity.x = 35.f;// 1350 - 1318 / (1 + pow(double((inplayTime / 655)), 0.64));
        // 1350.4 + (32.19252 - 1350.4)/(1 + (x/655.3164)^0.6426537)

        /*check collision*/

        for (int index = 0; index < players.size(); ++index)
        {
            if (!collisionOn)
                break;

            if (hitBox.Intersects(players[index]->hitBox))
            {
                DBOUT("Player " << index << " touched the ball\n");
                bool skip = false;
                res->getSound()->add("ball_hit");

                switch (index)
                {
                    case 0:
                        if (hitBox.Center.z < players[index]->hitBox.Center.z + players[index]->hitBox.Extents.z)
                        {
                            skip = true; collisionOn = false;
                            Translation = pPos;
                            Direction.x *= -1;

                            if (players[index]->currState == PCState::DASH)
                            {
                               
                            }

                        }
                        break;
                    case 1:
                        if (hitBox.Center.z > players[index]->hitBox.Center.z - players[index]->hitBox.Extents.z)
                        {
                            skip = true; collisionOn = false;
                            Translation = pPos;
                            Direction.x *= -1;
                        }
                        break;
                    case 2:
                        if (hitBox.Center.x < players[index]->hitBox.Center.x + players[index]->hitBox.Extents.z)
                        {
                            skip = true; collisionOn = false;
                            Translation = pPos;
                            Direction.z *= -1;
                        }
                        break;
                    case 3:
                        if (hitBox.Center.x > players[index]->hitBox.Center.x - players[index]->hitBox.Extents.z)
                        {
                            skip = true; collisionOn = false;
                            Translation = pPos;
                            Direction.z *= -1;
                        }
                        break;
                }
                
                if(!skip)
                {

                    Color = players[index]->Color;
                    lastTouch = index;

                    if (players[index]->Orientation)
                    {
                        Translation = pPos;
                        Direction.x *= -1;
                    }
                    else
                    {

                        Translation = pPos;
                        Direction.z *= -1;
                    }

                }
            }
        }


        /*check if ball not defended*/
        if (Translation.z <= -BALL_BORDER && !players[0]->npc)
        {
            resetB = true;
            DBOUT("Player 1 hit by Player " << lastTouch << "\n");
            if (players[0]->controllingPlayer)
            {
                players[0]->controllingPlayer->hp--;
            }
        }
        else if (Translation.z >= BALL_BORDER && !players[1]->npc)
        {
            resetB = true;
            DBOUT("Player 2 hit by Player " << lastTouch << "\n");
            if (players[1]->controllingPlayer)
            {
                players[1]->controllingPlayer->hp--;
            }
        }

        if (Translation.x <= -BALL_BORDER && !players[2]->npc)
        {
            resetB = true;
            DBOUT("Player 3 hit by Player " << lastTouch << "\n");
            if (players[2]->controllingPlayer)
            {
                players[2]->controllingPlayer->hp--;
            }
        }
        else if (Translation.x >= BALL_BORDER && !players[3]->npc)
        {
            resetB = true;
            DBOUT("Player 4 hit by Player " << lastTouch << "\n");
            if (players[3]->controllingPlayer)
            {
                players[3]->controllingPlayer->hp--;
            }
        }

        if (!maxPlayArea.Contains(hitBox))
        {
            DBOUT("OUT OF MAX PLAY AREA\n");
            resetB = true;
        }

        if (resetB)
        {
            res->getSound()->add("boom");
            ballState = BallState::RESET;
            distanceV = Translation;

            DBOUT("HP LEVEL:\n");
            for (auto& p : players)
            {
                if (p->controllingPlayer != nullptr)
                {
                    DBOUT("Player " << p->controllingPlayer->pID << ": " << p->controllingPlayer->hp << "\n");
                }
                    
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

void Ball::resetBallFull()
{
    resetBall();
    ballState = BallState::SPAWN;
    Velocity.y = 10.f;
    Color = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
}


void Ball::resetBall()
{
    ballState = BallState::FREEZE;
    resetTime = 0.f;
    inplayTime = 0.f;
    Velocity.y = 0.f;

    Translation = XMFLOAT3(0.f, ballHeight, 0.f);

    /*random direction*/
    double rDir = rand() / (RAND_MAX + 1.) * 2 * XM_PI;

    Direction.x = (float)cos(rDir);
    Direction.z = (float)sin(rDir);

    if (rand() % 2 == 0)
    {
        Direction.x *= -1;
    }
    if (rand() % 2 == 0)
    {
        Direction.y *= -1;
    }

//#ifdef _DEBUG
//    Direction.x = 0.f;
//    Direction.z = -1.f;
//#endif
    DBOUT("Direction: " << Direction.x << " | " << Direction.z << "\n");

    Velocity.z = START_VELOCITY;
    Velocity.x = START_VELOCITY;
 
    Color = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);

    collisionOn = true;
    resetB = false;
}