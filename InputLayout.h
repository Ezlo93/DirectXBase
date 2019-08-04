#pragma once

#include "util.h"


class InputLayoutDesc
{
public:
    static const D3D11_INPUT_ELEMENT_DESC Pos[1];
    static const D3D11_INPUT_ELEMENT_DESC Standard[4];
    static const D3D11_INPUT_ELEMENT_DESC StandardSkinned[6];
};

class InputLayouts
{
public:
    static void Init(ID3D11Device* device);
    static void Destroy();

    static ID3D11InputLayout* Pos;
    static ID3D11InputLayout* Standard;
    static ID3D11InputLayout* StandardSkinned;
};