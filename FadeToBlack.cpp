#include "FadeToBlack.h"

FadeToBlack::FadeToBlack() : mFadedOutputTexSRV(0), mFadedOutputTexUAV(0), width(0), height(0), format(DXGI_FORMAT_R8G8B8A8_UNORM)
{
}

FadeToBlack::~FadeToBlack()
{
    DXRelease(mFadedOutputTexSRV);
    DXRelease(mFadedOutputTexUAV);
}

ID3D11ShaderResourceView* FadeToBlack::getOutput()
{
    return mFadedOutputTexSRV;
}

void FadeToBlack::Init(ID3D11Device* device, UINT _width, UINT _height, DXGI_FORMAT _format)
{

    // Start fresh.
    DXRelease(mFadedOutputTexSRV);
    DXRelease(mFadedOutputTexUAV);

    width = _width;
    height = _height;
    format = _format;

    D3D11_TEXTURE2D_DESC fadedTexDesc;
    fadedTexDesc.Width = width;
    fadedTexDesc.Height = height;
    fadedTexDesc.MipLevels = 1;
    fadedTexDesc.ArraySize = 1;
    fadedTexDesc.Format = format;
    fadedTexDesc.SampleDesc.Count = 1;
    fadedTexDesc.SampleDesc.Quality = 0;
    fadedTexDesc.Usage = D3D11_USAGE_DEFAULT;
    fadedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    fadedTexDesc.CPUAccessFlags = 0;
    fadedTexDesc.MiscFlags = 0;

    ID3D11Texture2D* fadedTex = 0;
    device->CreateTexture2D(&fadedTexDesc, 0, &fadedTex);

    if (fadedTex != nullptr)
    {

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        device->CreateShaderResourceView(fadedTex, &srvDesc, &mFadedOutputTexSRV);

        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        uavDesc.Format = format;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Texture2D.MipSlice = 0;
        device->CreateUnorderedAccessView(fadedTex, &uavDesc, &mFadedOutputTexUAV);
    }
    // Views save a reference to the texture so we can release our reference.
    DXRelease(fadedTex);
}

void FadeToBlack::Fade(ID3D11DeviceContext* context, ID3D11ShaderResourceView* inputSRV, ID3D11UnorderedAccessView* inputUAV, float fadeValue)
{

    /*horizontal*/
    D3DX11_TECHNIQUE_DESC tech;
    Shaders::fadeShader->FadeTech->GetDesc(&tech);

    for (UINT p = 0; p < tech.Passes; p++)
    {
        Shaders::fadeShader->SetInput(inputSRV);
        Shaders::fadeShader->SetOutput(mFadedOutputTexUAV);
        Shaders::fadeShader->SetFadeValue(fadeValue);

        Shaders::fadeShader->FadeTech->GetPassByIndex(p)->Apply(0, context);

        UINT numGroups = (UINT)ceilf(width / NUM_THREADS);
        context->Dispatch(numGroups, height, 1);

    }

    ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
    ID3D11ShaderResourceView* nullSRV[1] = { 0 };
    context->CSSetShaderResources(0, 1, nullSRV);
    context->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);


    context->CSSetShader(0, 0, 0);
}