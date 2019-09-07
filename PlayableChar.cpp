#include "PlayableChar.h"
#include "Shader.h"

PlayableChar::PlayableChar(std::string id, ResourceManager* r)
{
    res = r;
    modelID = id;
    XMStoreFloat4x4(&World, XMMatrixIdentity());


    Translation = XMFLOAT3(0.f, 1.01f, 0.f);
    Scale = XMFLOAT3(1.f, 1.f, 1.f);
    Rotation = XMFLOAT3(0.f, 0.f, 0.f);
    Velocity = XMFLOAT3(0.f, 0.f, 0.f);

    texID = "bar";
    Speed = 40.f;
    cam = new Camera();

    hitBox.Center = res->getModel(modelID)->collisionBox.Center;
    hitBox.Extents = res->getModel(modelID)->collisionBox.Extents;
    hitBox.Orientation = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
    
    hitBox.Center.x += Translation.x;
    hitBox.Center.y += Translation.y;
    hitBox.Center.z += Translation.z;
}

PlayableChar::~PlayableChar()
{

    delete cam;
}

void PlayableChar::Update(float deltaTime)
{

    hitBox.Center.x = res->getModel(modelID)->collisionBox.Center.x + Translation.x;
    hitBox.Center.y = res->getModel(modelID)->collisionBox.Center.y + Translation.y;
    hitBox.Center.z = res->getModel(modelID)->collisionBox.Center.z + Translation.z;

   // DBOUT("hitbox: " << hitBox.Center.x << " " << hitBox.Center.y << " "<< hitBox.Center.z << std::endl);

    cam->setPosition(Translation.x, Translation.y + 4.0f, Translation.z - 25);
    cam->UpdateViewMatrix();
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
    Shaders::basicTextureShader->BasicTextureTechnique->GetDesc(&techDesc);

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
            Shaders::basicTextureShader->SetTexture(res->getTexture(texID));

            Shaders::basicTextureShader->BasicTextureTechnique->GetPassByIndex(p)->Apply(0, deviceContext);
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
