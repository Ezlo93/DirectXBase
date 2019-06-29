#pragma once

#include "DirectXBase.h"


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

    struct PosNormalTexTan
    {
        XMFLOAT3 Pos;
        XMFLOAT3 Normal;
        XMFLOAT2 Tex;
        XMFLOAT4 TangentU;
    };

    struct PosNormalTexTanSkinned
    {
        XMFLOAT3 Pos;
        XMFLOAT3 Normal;
        XMFLOAT2 Tex;
        XMFLOAT4 TangentU;
        XMFLOAT3 Weights;
        BYTE BoneIndices[4];
    };
}

class InputLayoutDesc
{
public:
    // Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
    static const D3D11_INPUT_ELEMENT_DESC Pos[1];
    static const D3D11_INPUT_ELEMENT_DESC Basic32[3];
    static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTan[4];
    static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTanSkinned[6];
};

class InputLayouts
{
public:
    static void Init(ID3D11Device* device);
    static void Destroy();

    static ID3D11InputLayout* Pos;
    static ID3D11InputLayout* Basic32;
    static ID3D11InputLayout* PosNormalTexTan;
    static ID3D11InputLayout* PosNormalTexTanSkinned;
};