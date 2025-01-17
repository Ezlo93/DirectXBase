#pragma once

#include "util.h"
#include "Camera.h"

class ShadowMap
{
public:
    ShadowMap(ID3D11Device* device, UINT size);
    ~ShadowMap();

    ID3D11ShaderResourceView* DepthMapSRV();

    void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc);

private:
    UINT mWidth;
    UINT mHeight;

    ID3D11ShaderResourceView* mDepthMapSRV;
    ID3D11DepthStencilView* mDepthMapDSV;

    D3D11_VIEWPORT mViewport;
};