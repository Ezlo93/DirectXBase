#pragma once

#include "DirectXBase.h"

class RenderStates
{
public:
    static void Init(ID3D11Device* device);
    static void Destroy();

    static ID3D11RasterizerState* wireFrame;
    static ID3D11RasterizerState* noCullRS;
    static ID3D11DepthStencilState* equalsDSS;
    static ID3D11BlendState* alphaToCoverageBS;
    static ID3D11BlendState* transparentBS;

};