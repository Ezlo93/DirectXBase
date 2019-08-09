#include "TextureToView.h"

void TextureToView::Init(ID3D11Device *device)
{
    /*position and tex coords*/
    std::vector<Vertex::Standard> vertices(4);

    vertices[0] = Vertex::Standard(-1.0f, -1.0f, 0.0f,
                                   0.0f, 1.0f,
                                   0.0f, 0.0f, -1.0f,
                                   1.0f, 0.0f, 0.0f);

    vertices[1] = Vertex::Standard(-1.0f, 1.0f, 0.0f,
                                   0.0f, 0.0f,
                                   0.0f, 0.0f, -1.0f,
                                   1.0f, 0.0f, 0.0f);

    vertices[2] = Vertex::Standard(1.0f, 1.0f, 0.0f,
                                   1.0f, 0.0f,
                                   0.0f, 0.0f, -1.0f,
                                   1.0f, 0.0f, 0.0f);

    vertices[3] = Vertex::Standard(1.0f, -1.0f, 0.0f,
                                   1.0, 1.0f,
                                   0.0f, 0.0f, -1.0f,
                                   1.0f, 0.0f, 0.0f);

    /*indices for quad*/
    std::vector<UINT> indices(6);
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    /*send to graphics card*/
    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = (UINT)(sizeof(Vertex::Standard) * vertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    
    device->CreateBuffer(&vbd, &vinitData, &ttvVB);


    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = (UINT)(sizeof(UINT) * indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    device->CreateBuffer(&ibd, &iinitData, &ttvIB);

}
