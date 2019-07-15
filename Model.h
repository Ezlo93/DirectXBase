#pragma once

#include "util.h"

namespace Vertex
{

    struct BasicColor
    {
        XMFLOAT3 Pos;
        XMFLOAT4 Color;
    };

    struct Basic32
    {
        XMFLOAT3 Pos;
        XMFLOAT3 Normal;
        XMFLOAT2 Tex;
    };

    struct PosTexNormalTan
    {
        PosTexNormalTan(XMFLOAT3 p, XMFLOAT2 t, XMFLOAT3 n, XMFLOAT3 tu) : Pos(p), Tex(t), Normal(n), TangentU(tu) {}
        PosTexNormalTan() : Pos(), Tex(), Normal(), TangentU() { RtlSecureZeroMemory(this, sizeof(this)); }
        PosTexNormalTan(float px, float py, float pz,
                        float u, float v,
                        float nx, float ny, float nz,
                        float tx, float ty, float tz
                        )
            : Pos(px, py, pz), Tex(u,v), Normal(nx,ny,nz), TangentU(tx,ty,tz){ }
        XMFLOAT3 Pos;
        XMFLOAT2 Tex;
        XMFLOAT3 Normal;
        XMFLOAT3 TangentU;
    };

    struct PosTexNormalTanSkinned
    {
        XMFLOAT3 Pos;
        XMFLOAT3 Normal;
        XMFLOAT2 Tex;
        XMFLOAT4 TangentU;
        XMFLOAT3 Weights;
        BYTE BoneIndices[4];
    };
}

namespace Material {
    struct Standard
    {
        Standard() : Ambient(), Diffuse(), Specular() { RtlSecureZeroMemory(this, sizeof(this)); }

        XMFLOAT4 Ambient;
        XMFLOAT4 Diffuse;
        XMFLOAT4 Specular; // w = SpecPower
        //XMFLOAT4 Reflect;
    };
}


class Mesh
{
public:
    std::vector<Vertex::PosTexNormalTan> vertices;
    std::vector<UINT> indices;
    Material::Standard material;

    bool hasTextureCoordinates = false;
    bool hasTangentu = false;

    ID3D11Buffer* vertex = 0, * index = 0;
    std::string textureID;

    ~Mesh()
    {
        DXRelease(vertex);
        DXRelease(index);   
    }


    /*draw mesh*/
    void Draw(ID3D11DeviceContext* context)
    {

        UINT stride = sizeof(Vertex::PosTexNormalTan);
        UINT offset = 0;

        context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
        context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);

        context->DrawIndexed((UINT)(indices.size()), 0, 0);

    }


    /*create vertex and index buffer on gpu*/
    void createBuffers(ID3D11Device* device)
    {

        D3D11_BUFFER_DESC vbd;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        vbd.ByteWidth = (UINT)(sizeof(Vertex::PosTexNormalTan) * vertices.size());
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = 0;
        vbd.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = &vertices[0];

        device->CreateBuffer(&vbd, &initData, &vertex);

        D3D11_BUFFER_DESC ibd;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = (UINT)(sizeof(UINT) * indices.size());
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.CPUAccessFlags = 0;
        ibd.MiscFlags = 0;

        initData.pSysMem = &indices[0];

        device->CreateBuffer(&ibd, &initData, &index);

    }
};


class Model
{

public:

    std::vector<Mesh*> meshes;

    Model(ID3D11Device* dev);
    ~Model();

    void Draw(ID3D11DeviceContext* context);
    void CreateBuffers();


private:
    ID3D11Device* device;

};