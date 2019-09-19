#pragma once

#include "util.h"
#include "Shader.h"

#define NUM_THREADS 256.f

class FadeToBlack
{
public:
    FadeToBlack();
    ~FadeToBlack();

    ID3D11ShaderResourceView* getOutput();

    void Init(ID3D11Device* device, UINT _width, UINT _height, DXGI_FORMAT _format);

    void Fade(ID3D11DeviceContext* context, ID3D11ShaderResourceView* inputSRV,
                    ID3D11UnorderedAccessView* inputUAV, float fadeValue);

private:
    UINT width, height;
    DXGI_FORMAT format;

    ID3D11ShaderResourceView* mFadedOutputTexSRV;
    ID3D11UnorderedAccessView* mFadedOutputTexUAV;
};