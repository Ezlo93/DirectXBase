#pragma once

#include "util.h"


class InputLayoutDesc
{
public:
    // Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
    static const D3D11_INPUT_ELEMENT_DESC Pos[1];
    static const D3D11_INPUT_ELEMENT_DESC Basic32[3];
    static const D3D11_INPUT_ELEMENT_DESC PosTexNormalTan[4];
    static const D3D11_INPUT_ELEMENT_DESC PosTexNormalTanSkinned[6];
};

class InputLayouts
{
public:
    static void Init(ID3D11Device* device);
    static void Destroy();

    static ID3D11InputLayout* Pos;
    static ID3D11InputLayout* Basic32;
    static ID3D11InputLayout* PosTexNormalTan;
    static ID3D11InputLayout* PosTexNormalTanSkinned;
};