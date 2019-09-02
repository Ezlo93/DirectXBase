#include "Ball.h"
#include "Shader.h"

Ball::Ball(std::string id, ResourceManager* r)
{

    res = r;
    modelID = id;
    XMStoreFloat4x4(&World, XMMatrixIdentity());

    Position = XMFLOAT3(0.f,.51f,0.f);
    Scale = XMFLOAT3(2.f, 2.f, 2.f);
    Rotation = XMFLOAT3(0.f, 0.f, 0.f);

    Velocity.y = 10.f;
    bounceFactor = 0.75f;
    bounceTime = 1.f;
}

Ball::~Ball()
{

}

void Ball::Update(float deltaTime)
{

    if (Velocity.y <= 0.1f)
    {
        
        bounceTime = 0.f;
        Velocity.y = 0.f;
        return;
    }

    bounceTime += deltaTime;

    if (Position.y > 0.5f)
    {
        Position.y = Velocity.y * bounceTime - (GRAVITY / 2.f * bounceTime * bounceTime) + 0.51f;
    }
    else
    {
        bounceTime = 0.f;
        Position.y = 0.51f;
        Velocity.y = Velocity.y * 0.75f;
    }

}

void Ball::Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX shadowT)
{

    Model* model = res->getModel(modelID);

    XMStoreFloat4x4(&World, XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z) * XMMatrixTranslation(Position.x, Position.y, Position.z) * XMMatrixScaling(Scale.x, Scale.y, Scale.z));

    XMMATRIX view = c->getView();
    XMMATRIX proj = c->getProj();
    XMMATRIX viewProj = c->getViewProj();

    XMMATRIX world = XMLoadFloat4x4(&World);
    XMMATRIX wvp = world * view * proj;

    XMMATRIX wit = DXMath::InverseTranspose(world);

    UINT stride = sizeof(Vertex::Standard);
    UINT offset = 0;

    D3DX11_TECHNIQUE_DESC techDesc;
    Shaders::basicTextureShader->BasicNoTextureTechnique->GetDesc(&techDesc);

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

            Shaders::basicTextureShader->BasicNoTextureTechnique->GetPassByIndex(p)->Apply(0, deviceContext);
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
