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

    struct Standard
    {
        Standard(XMFLOAT3 p, XMFLOAT2 t, XMFLOAT3 n, XMFLOAT3 tu) : Pos(p), Tex(t), Normal(n), TangentU(tu) {}
        Standard() : Pos(), Tex(), Normal(), TangentU() { RtlSecureZeroMemory(this, sizeof(this)); }
        Standard(float px, float py, float pz,
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

    struct StandardSkinned
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
        Standard() : Ambient(), Diffuse(), Specular(), Reflect() { RtlSecureZeroMemory(this, sizeof(this)); }

        XMFLOAT4 Ambient;
        XMFLOAT4 Diffuse;
        XMFLOAT4 Specular; // w = SpecPower
        XMFLOAT4 Reflect;
    };
}


struct DirectionalLight
{
    DirectionalLight() { ZeroMemory(this, sizeof(this)); }

    XMFLOAT4 Ambient;
    XMFLOAT4 Diffuse;
    XMFLOAT4 Specular;
    XMFLOAT3 Direction;
    float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct PointLight
{
    PointLight() { ZeroMemory(this, sizeof(this)); }

    XMFLOAT4 Ambient;
    XMFLOAT4 Diffuse;
    XMFLOAT4 Specular;

    // Packed into 4D vector: (Position, Range)
    XMFLOAT3 Position;
    float Range;

    // Packed into 4D vector: (A0, A1, A2, Pad)
    XMFLOAT3 Att;
    float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct SpotLight
{
    SpotLight() { ZeroMemory(this, sizeof(this)); }

    XMFLOAT4 Ambient;
    XMFLOAT4 Diffuse;
    XMFLOAT4 Specular;

    // Packed into 4D vector: (Position, Range)
    XMFLOAT3 Position;
    float Range;

    // Packed into 4D vector: (Direction, Spot)
    XMFLOAT3 Direction;
    float Spot;

    // Packed into 4D vector: (Att, Pad)
    XMFLOAT3 Att;
    float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

class Mesh
{
public:
    std::vector<Vertex::Standard> vertices;
    std::vector<UINT> indices;
    Material::Standard material;

    bool hasTextureCoordinates = false;
    bool hasTangentu = false;

    ID3D11Buffer* vertex = 0, * index = 0;
    std::string diffuseMapID;
    std::string normalMapID;
    std::string bumpMapID;

    ~Mesh()
    {
        DXRelease(vertex);
        DXRelease(index);   
    }


    /*create vertex and index buffer on gpu*/
    void createBuffers(ID3D11Device* device)
    {

        D3D11_BUFFER_DESC vbd;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        vbd.ByteWidth = (UINT)(sizeof(Vertex::Standard) * vertices.size());
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

    void CreateBuffers();

    XMMATRIX axisRot;
private:
    ID3D11Device* device;
    

};