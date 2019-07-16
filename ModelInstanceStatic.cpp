#include "ModelInstanceStatic.h"


ModelInstanceStatic::ModelInstanceStatic(ID3D11Device* d, ID3D11DeviceContext* c, ResourceManager* r, std::string id)
{
    device = d;
    deviceContext = c;
    XMMATRIX I = XMMatrixIdentity();
    XMStoreFloat4x4(&World, I);
    XMStoreFloat4x4(&TextureTransform, I);
    Rotation = I;
    Scale = I;
    Translation = I;
    modelID = id;
    resources = r;
}

ModelInstanceStatic::~ModelInstanceStatic()
{
}

void ModelInstanceStatic::Draw(Camera* c, BasicTextureShader* s)
{
    XMStoreFloat4x4(&World, Rotation * Scale * Translation);

    XMMATRIX view = c->getView();
    XMMATRIX proj = c->getProj();
    XMMATRIX viewProj = c->getViewProj();

    XMMATRIX world = XMLoadFloat4x4(&World);
    XMMATRIX wvp = world * view * proj;

    UINT stride = sizeof(Vertex::PosTexNormalTan);
    UINT offset = 0;


    ID3DX11EffectTechnique* tech = s->BasicTextureTechnique;

    D3DX11_TECHNIQUE_DESC techDesc;
    tech->GetDesc(&techDesc);
    
    Model* model = resources->getModel(modelID);

    for (auto& m : model->meshes)
    {
        for (UINT p = 0; p < techDesc.Passes; p++)
        {
            deviceContext->IASetVertexBuffers(0, 1, &m->vertex, &stride, &offset);
            deviceContext->IASetIndexBuffer(m->index, DXGI_FORMAT_R32_UINT, 0);

            /*set per object constants*/
            s->SetWorld(world);
            s->SetWorldViewProj(wvp);
            s->SetWorldInvTranspose(DXMath::InverseTranspose(world));
            s->SetMaterial(m->material);
            s->SetTexture(resources->getTexture(m->textureID));
            s->SetTexTransform(XMLoadFloat4x4(&TextureTransform));

            /*apply and draw*/
            tech->GetPassByIndex(p)->Apply(0, deviceContext);
            deviceContext->DrawIndexed((UINT)(m->indices.size()), 0, 0);
        }



    }


}
