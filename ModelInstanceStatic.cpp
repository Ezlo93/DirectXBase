#include "ModelInstanceStatic.h"


ModelInstanceStatic::ModelInstanceStatic(ID3D11Device* d, ID3D11DeviceContext* c, ResourceManager* r, std::string id)
{
    device = d;
    deviceContext = c;
    XMMATRIX I = XMMatrixIdentity();
    XMStoreFloat4x4(&World, I);
    XMStoreFloat4x4(&TextureTransform, I);
    Rotation = XMFLOAT3(0.f,0.f,0.f);
    Scale = XMFLOAT3(1.f, 1.f, 1.f);
    Translation = XMFLOAT3(0.f, 0.f, 0.f);
    modelID = id;
    resources = r;
    usedShader = UShader::Basic;
    usedTechnique = UTech::Basic;
}

ModelInstanceStatic::~ModelInstanceStatic()
{
}

void ModelInstanceStatic::Draw(Camera* c)
{
    XMMATRIX _r = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
    XMMATRIX _t = XMMatrixTranslation(Translation.x, Translation.y, Translation.z);
    XMMATRIX _s = XMMatrixScaling(Scale.x, Scale.y, Scale.z);

    XMStoreFloat4x4(&World, _r * _s * _t);

    XMMATRIX view = c->getView();
    XMMATRIX proj = c->getProj();
    XMMATRIX viewProj = c->getViewProj();

    XMMATRIX world = XMLoadFloat4x4(&World);
    XMMATRIX wvp = world * view * proj;

    UINT stride = sizeof(Vertex::PosTexNormalTan);
    UINT offset = 0;

    /*select tech*/
    ID3DX11EffectTechnique* tech = 0;
    
    switch (usedShader)
    {
        case UShader::Basic:
            switch (usedTechnique)
            {
                case UTech::Basic: tech = Shaders::basicTextureShader->BasicTextureTechnique; break;
                case UTech::BasicNormalMap: tech = Shaders::basicTextureShader->BasicTextureNormalTechnique; break;
            }
            break;
    }

    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc(&techDesc);
    
    Model* model = resources->getModel(modelID);

    for (auto& m : model->meshes)
    {
        for (UINT p = 0; p < techDesc.Passes; p++)
        {
            deviceContext->IASetVertexBuffers(0, 1, &m->vertex, &stride, &offset);
            deviceContext->IASetIndexBuffer(m->index, DXGI_FORMAT_R32_UINT, 0);

            /*set per object constants based on used shader and technique*/

            switch (usedShader)
            {
                case UShader::Basic:
                    switch (usedTechnique)
                    {
                        case UTech::Basic:
                            Shaders::basicTextureShader->SetWorld(world);
                            Shaders::basicTextureShader->SetWorldViewProj(wvp);
                            Shaders::basicTextureShader->SetWorldInvTranspose(DXMath::InverseTranspose(world));
                            Shaders::basicTextureShader->SetMaterial(m->material);
                            Shaders::basicTextureShader->SetTexture(resources->getTexture(m->diffuseMapID));
                            Shaders::basicTextureShader->SetTexTransform(XMLoadFloat4x4(&TextureTransform));
                            break;

                        case UTech::BasicNormalMap:
                            Shaders::basicTextureShader->SetWorld(world);
                            Shaders::basicTextureShader->SetWorldViewProj(wvp);
                            Shaders::basicTextureShader->SetWorldInvTranspose(DXMath::InverseTranspose(world));
                            Shaders::basicTextureShader->SetMaterial(m->material);
                            Shaders::basicTextureShader->SetTexture(resources->getTexture(m->diffuseMapID));
                            Shaders::basicTextureShader->SetNormalMap(resources->getTexture(m->normalMapID));
                            Shaders::basicTextureShader->SetTexTransform(XMLoadFloat4x4(&TextureTransform));
                            break;
                    }
            }


            /*apply and draw*/
            tech->GetPassByIndex(p)->Apply(0, deviceContext);
            deviceContext->DrawIndexed((UINT)(m->indices.size()), 0, 0);
        }



    }


}
