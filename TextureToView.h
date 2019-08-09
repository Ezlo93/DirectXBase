#pragma once

#include "util.h"
#include "Model.h"

class TextureToView
{
    ~TextureToView() { DXRelease(ttvVB); DXRelease(ttvIB); }

public:
    void Init(ID3D11Device* device);

    ID3D11Buffer* getVertexBuffer() { return ttvIB; }
    ID3D11Buffer* getIndexBuffer() { return ttvIB; }

private:
    ID3D11Buffer* ttvVB = 0;
    ID3D11Buffer* ttvIB = 0;
};