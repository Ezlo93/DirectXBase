#include "PlayableChar.h"
#include "Shader.h"

PlayableChar::PlayableChar(std::string id, ResourceManager* r)
{
    res = r;
    modelID = id;
    XMStoreFloat4x4(&World, XMMatrixIdentity());


    Translation = XMFLOAT3(0.f, PLAYER_HEIGHT, 0.f);
    Scale = XMFLOAT3(1.f, 1.f, 1.f);
    Rotation = XMFLOAT3(0.f, 0.f, 0.f);
    Velocity = XMFLOAT3(0.f, 0.f, 0.f);

    Color = XMFLOAT4(.5f, 0.5f, 0.5f, 1.0f);
    Speed = PLAYER_SPEED;
    cam = new Camera();

    hitBox.Center = res->getModel(modelID)->collisionBox.Center;
    hitBox.Extents = res->getModel(modelID)->collisionBox.Extents;
    hitBox.Orientation = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
    
    hitBox.Center.x += Translation.x;
    hitBox.Center.y += Translation.y;
    hitBox.Center.z += Translation.z;

    currState = PCState::REST;
    prevState = currState;
}

PlayableChar::~PlayableChar()
{
    delete cam;
    DXRelease(splitScreenSRV);
    DXRelease(splitScreenUAV);
    DXRelease(splitScreenView);
}

void PlayableChar::Update(float deltaTime)
{
    prevState = currState;

    if (Velocity.y > 0)
    {
        currState = PCState::JUMP;

        if (currState == PCState::JUMP && prevState == PCState::REST)
        {
            res->getSound()->add("boing");
        }

        jumpTime += deltaTime;

        if (Translation.y >= BaseHeight)
        {
            Translation.y = Velocity.y * jumpTime - (GRAVITY / 1.5f * jumpTime * jumpTime) + BaseHeight;
        }
        else
        {
            jumpTime = 0.f;
            Velocity.y = 0.f;
            Translation.y = BaseHeight;
            currState = PCState::REST;
        }
    }

    dashCooldown -= deltaTime;
    if (dashCooldown < 0)
    {
        dashCooldown = 0.f;
    }

    if (currState == PCState::DASH)
    {
        if (dashTimer > DASH_DURATION)
        {
            dashTimer = 0.f;
            currState = PCState::MOVE;
            Speed = PLAYER_SPEED;
        }
        else
        {
            dashTimer += deltaTime;
        }
    }



    hitBox.Center.x = res->getModel(modelID)->collisionBox.Center.x + Translation.x;
    hitBox.Center.y = res->getModel(modelID)->collisionBox.Center.y + Translation.y;
    hitBox.Center.z = res->getModel(modelID)->collisionBox.Center.z + Translation.z;

    switch (metaPosition)
    {
        case 0: cam->lookAt(XMFLOAT3(Translation.x, Translation.y + CAMERA_DIST_UP, Translation.z - CAMERA_DIST_BACK), XMFLOAT3(Translation.x, 0, 0), XMFLOAT3(0, 1, 0)); break;
        case 1: cam->lookAt(XMFLOAT3(Translation.x, Translation.y + CAMERA_DIST_UP, Translation.z + CAMERA_DIST_BACK), XMFLOAT3(Translation.x, 0, 0), XMFLOAT3(0, 1, 0)); break;
        case 2: cam->lookAt(XMFLOAT3(Translation.x - CAMERA_DIST_BACK, Translation.y + CAMERA_DIST_UP, Translation.z), XMFLOAT3(0, 0, Translation.z), XMFLOAT3(0, 1, 0)); break;
        case 3: cam->lookAt(XMFLOAT3(Translation.x + CAMERA_DIST_BACK, Translation.y + CAMERA_DIST_UP, Translation.z), XMFLOAT3(0, 0, Translation.z), XMFLOAT3(0, 1, 0)); break;
    }

    cam->UpdateViewMatrix();
}

void PlayableChar::initDash()
{
    if (dashCooldown == 0.f)
    {
        currState = PCState::DASH;
        dashCooldown = DASH_CD;
        dashTimer = 0.f;
        res->getSound()->add("woosh");
    }
    else
    {
        res->getSound()->add("no");
    }
}

void PlayableChar::Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX shadowT)
{

    Model* model = res->getModel(modelID);

    XMStoreFloat4x4(&World, XMMatrixScaling(Scale.x, Scale.y, Scale.z) * XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z) * model->axisRot * XMMatrixTranslation(Translation.x, Translation.y, Translation.z) );

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
            Shaders::basicTextureShader->SetShadowTransform(world * shadowT);
            Shaders::basicTextureShader->SetStaticColor(Color);

            Shaders::basicTextureShader->BasicStaticColor->GetPassByIndex(p)->Apply(0, deviceContext);
            deviceContext->DrawIndexed((UINT)(m->indices.size()), 0, 0);
        }


    }

}

Camera* PlayableChar::getCamera()
{
    return cam;
}

void PlayableChar::ShadowDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX lightView, XMMATRIX lightProj)
{

    Model* model = res->getModel(modelID);

    XMStoreFloat4x4(&World, XMMatrixScaling(Scale.x, Scale.y, Scale.z) * XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z) * model->axisRot * XMMatrixTranslation(Translation.x, Translation.y, Translation.z));

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
