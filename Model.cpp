#include "Model.h"


Model::Model(ID3D11Device* dev)
{
    device = dev;
}

Model::~Model()
{
    for (auto& i : meshes)
    {
        DXRelease(i.vertex);
        DXRelease(i.index);
    }
    
}

void Model::Draw(ID3D11DeviceContext* context)
{


}

/*create vertex and indexbuffers for all meshes of the model*/
void Model::CreateBuffers()
{

    for (auto& m : meshes)
    {

        D3D11_BUFFER_DESC vbd;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        vbd.ByteWidth = (UINT)(sizeof(Vertex::PosTexNormalTan) * m.vertices.size());
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = 0;
        vbd.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = &m.vertices[0];

        device->CreateBuffer(&vbd, &initData, &m.vertex);

        D3D11_BUFFER_DESC ibd;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = (UINT)(sizeof(UINT) * m.indices.size());
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.CPUAccessFlags = 0;
        ibd.MiscFlags = 0;

        initData.pSysMem = &m.indices[0];

        device->CreateBuffer(&ibd, &initData, &m.index);

    }

}
