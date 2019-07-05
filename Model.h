#pragma once

#include "DirectXBase.h"
#include <vector>

namespace Vertex
{
    // Basic 32-byte vertex structure.

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
        PosTexNormalTan(XMFLOAT3 p, XMFLOAT2 t, XMFLOAT3 n, XMFLOAT4 tu) : Pos(p), Tex(t), Normal(n), TangentU(tu) {}
        PosTexNormalTan() : Pos(), Tex(), Normal(), TangentU() { RtlSecureZeroMemory(this, sizeof(this)); }
        XMFLOAT3 Pos;
        XMFLOAT2 Tex;
        XMFLOAT3 Normal;
        XMFLOAT4 TangentU;
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


struct Mesh
{
    std::vector<Vertex::PosTexNormalTan> vertices;
    std::vector<UINT> indices;
    Material::Standard material;

    bool hasTextureCoordinates = false;
    bool hasTangentu = false;

};


class Model
{

public:

    std::vector<Mesh> meshes;

    Model()
    {

    }

    ~Model()
    {

    }
};