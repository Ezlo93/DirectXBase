#include "ModelInstanceStatic.h"


ModelInstanceStatic::ModelInstanceStatic(ResourceManager* r, std::string id)
{
    XMMATRIX I = XMMatrixIdentity();
    XMStoreFloat4x4(&World, I);
    XMStoreFloat4x4(&TextureTransform, I);
    Rotation = XMFLOAT3(0.f,0.f,0.f);
    Scale = XMFLOAT3(1.f, 1.f, 1.f);
    Translation = XMFLOAT3(0.f, 0.f, 0.f);
    modelID = id;
    resources = r;
    usedShader = UShader::Basic;
    usedTechnique = UTech::BasicNoTexture;
    ovrwrTex = "";
    ovrwrNrm = "";
    useOverwriteDiffuse = false;
    useOverwriteNormalMap = false;
}

ModelInstanceStatic::~ModelInstanceStatic()
{
}

void ModelInstanceStatic::Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX shadowT)
{
    Model* model = resources->getModel(modelID);

    XMMATRIX _r = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
    XMMATRIX _t = XMMatrixTranslation(Translation.x, Translation.y, Translation.z);
    XMMATRIX _s = XMMatrixScaling(Scale.x, Scale.y, Scale.z);

    XMStoreFloat4x4(&World, _s * model->axisRot * _r  * _t);

    XMMATRIX view = c->getView();
    XMMATRIX proj = c->getProj();
    XMMATRIX viewProj = c->getViewProj();

    XMMATRIX world = XMLoadFloat4x4(&World);
    XMMATRIX wvp = world * view * proj;

    XMMATRIX wit = DXMath::InverseTranspose(world);

    UINT stride = sizeof(Vertex::Standard);
    UINT offset = 0;

    /*select tech*/
    ID3DX11EffectTechnique* tech = 0;
    
    switch (usedShader)
    {
        case UShader::Basic:
            switch (usedTechnique)
            {
                case UTech::Basic: tech = Shaders::basicTextureShader->BasicTextureTechnique; break;
                case UTech::BasicNoTexture: tech = Shaders::basicTextureShader->BasicNoTextureTechnique; break;
                case UTech::BasicNoLighting: tech = Shaders::basicTextureShader->BasicTextureNoLighting; break;
                default: tech = Shaders::basicTextureShader->BasicTextureTechnique; break;
            }
            break;
        case UShader::Normal:
            switch (usedTechnique)
            {
                case UTech::NormalTech: tech = Shaders::normalMapShader->NormalMapTech; break;
                default: tech = Shaders::normalMapShader->NormalMapTech; break;
            }
    }

    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc(&techDesc);

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
                            Shaders::basicTextureShader->SetWorldInvTranspose(wit);
                            Shaders::basicTextureShader->SetMaterial(m->material);

                            if (useOverwriteDiffuse)
                            {
                                Shaders::basicTextureShader->SetTexture(resources->getTexture(ovrwrTex));
                            }
                            else
                            {
                                Shaders::basicTextureShader->SetTexture(resources->getTexture(m->diffuseMapID));
                            }

                            Shaders::basicTextureShader->SetTexTransform(XMLoadFloat4x4(&TextureTransform));
                            Shaders::basicTextureShader->SetShadowTransform(world * shadowT);
                            break;

                        case UTech::BasicNoLighting:

                            Shaders::basicTextureShader->SetWorld(world);
                            Shaders::basicTextureShader->SetWorldViewProj(wvp);
                            Shaders::basicTextureShader->SetWorldInvTranspose(wit);
                            Shaders::basicTextureShader->SetMaterial(m->material);

                            if (useOverwriteDiffuse)
                            {
                                Shaders::basicTextureShader->SetTexture(resources->getTexture(ovrwrTex));
                            }
                            else
                            {
                                Shaders::basicTextureShader->SetTexture(resources->getTexture(m->diffuseMapID));
                            }

                            Shaders::basicTextureShader->SetTexTransform(XMLoadFloat4x4(&TextureTransform));
                            break;

                        case UTech::BasicNoTexture:
                            Shaders::basicTextureShader->SetWorld(world);
                            Shaders::basicTextureShader->SetWorldInvTranspose(wit);
                            Shaders::basicTextureShader->SetWorldViewProj(wvp);
                            Shaders::basicTextureShader->SetMaterial(m->material);
                            break;

                    }
                    break;
                case UShader::Normal:
                    switch (usedTechnique)
                    {
                        case UTech::NormalTech:
                            Shaders::normalMapShader->SetWorld(world);
                            Shaders::normalMapShader->SetWorldViewProj(wvp);
                            Shaders::normalMapShader->SetWorldInvTranspose(wit);
                            Shaders::normalMapShader->SetMaterial(m->material);

                            if (useOverwriteDiffuse)
                            {
                                Shaders::normalMapShader->SetTexture(resources->getTexture(ovrwrTex));
                            }
                            else
                            {
                                Shaders::normalMapShader->SetTexture(resources->getTexture(m->diffuseMapID));
                            }

                            if (useOverwriteNormalMap)
                            {
                                Shaders::normalMapShader->SetNormalMap(resources->getTexture(ovrwrNrm));
                            }
                            else
                            {
                                Shaders::normalMapShader->SetNormalMap(resources->getTexture(m->normalMapID));
                            }

                            Shaders::normalMapShader->SetTexTransform(XMLoadFloat4x4(&TextureTransform));
                            break;
                    }
                    break;
            }


            /*apply and draw*/
            tech->GetPassByIndex(p)->Apply(0, deviceContext);
            deviceContext->DrawIndexed((UINT)(m->indices.size()), 0, 0);
        }



    }


}



void ModelInstanceStatic::ShadowDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c)
{
    Model* model = resources->getModel(modelID);

    XMMATRIX _r = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
    XMMATRIX _t = XMMatrixTranslation(Translation.x, Translation.y, Translation.z);
    XMMATRIX _s = XMMatrixScaling(Scale.x, Scale.y, Scale.z);

    XMStoreFloat4x4(&World, _s * model->axisRot * _r * _t);

    XMMATRIX view = c->getView();
    XMMATRIX proj = c->getProj();
    XMMATRIX viewProj = c->getViewProj();

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

            /*apply and draw*/
            tech->GetPassByIndex(p)->Apply(0, deviceContext);
            deviceContext->DrawIndexed((UINT)(m->indices.size()), 0, 0);
        }



    }


}




void ModelInstanceStatic::OverwriteDiffuseMap(std::string id)
{
    useOverwriteDiffuse = true;
    ovrwrTex = id;
}

void ModelInstanceStatic::OverwriteNormalMap(std::string id)
{
    useOverwriteNormalMap = true;
    ovrwrNrm = id;
}
