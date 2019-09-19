#include "Blur.h"

Blur::Blur() : mBlurredOutputTexSRV(0), mBlurredOutputTexUAV(0), width(0), height(0), format(DXGI_FORMAT_R8G8B8A8_UNORM)
{
}

Blur::~Blur()
{
    DXRelease(mBlurredOutputTexSRV);
    DXRelease(mBlurredOutputTexUAV);
}

ID3D11ShaderResourceView* Blur::getOutput()
{
    return mBlurredOutputTexSRV;
}

void Blur::Init(ID3D11Device* device, UINT _width, UINT _height, DXGI_FORMAT _format)
{
    
    // Start fresh.
    DXRelease(mBlurredOutputTexSRV);
    DXRelease(mBlurredOutputTexUAV);

    width = _width;
    height = _height;
    format = _format;

    D3D11_TEXTURE2D_DESC blurredTexDesc;
    blurredTexDesc.Width = width;
    blurredTexDesc.Height = height;
    blurredTexDesc.MipLevels = 1;
    blurredTexDesc.ArraySize = 1;
    blurredTexDesc.Format = format;
    blurredTexDesc.SampleDesc.Count = 1;
    blurredTexDesc.SampleDesc.Quality = 0;
    blurredTexDesc.Usage = D3D11_USAGE_DEFAULT;
    blurredTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    blurredTexDesc.CPUAccessFlags = 0;
    blurredTexDesc.MiscFlags = 0;

    ID3D11Texture2D* blurredTex = 0;
    device->CreateTexture2D(&blurredTexDesc, 0, &blurredTex);

    if (blurredTex != nullptr)
    {

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        device->CreateShaderResourceView(blurredTex, &srvDesc, &mBlurredOutputTexSRV);

        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        uavDesc.Format = format;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Texture2D.MipSlice = 0;
        device->CreateUnorderedAccessView(blurredTex, &uavDesc, &mBlurredOutputTexUAV);
    }
    // Views save a reference to the texture so we can release our reference.
    DXRelease(blurredTex);
}

void Blur::BlurSRV(ID3D11DeviceContext* context, ID3D11ShaderResourceView* inputSRV, ID3D11UnorderedAccessView* inputUAV, int count)
{

    for (int i = 0; i < count; i++)
    {

        /*horizontal*/
        D3DX11_TECHNIQUE_DESC tech;
        Shaders::blurShader->HorizontalBlur->GetDesc(&tech);

        for (UINT p = 0; p < tech.Passes; p++)
        {
            Shaders::blurShader->SetInput(inputSRV);
            Shaders::blurShader->SetOutput(mBlurredOutputTexUAV);

            Shaders::blurShader->HorizontalBlur->GetPassByIndex(p)->Apply(0, context);

            UINT numGroups = (UINT)ceilf(width / NUM_THREADS);
            context->Dispatch(numGroups, height, 1);

        }

        /*unbind*/
        ID3D11ShaderResourceView* nullSRV[1] = { 0 };
        context->CSSetShaderResources(0, 1, nullSRV);

        ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
        context->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

        /*vertical*/
        Shaders::blurShader->VerticalBlur->GetDesc(&tech);

        for (UINT p = 0; p < tech.Passes; p++)
        {
            Shaders::blurShader->SetInput(mBlurredOutputTexSRV);
            Shaders::blurShader->SetOutput(inputUAV);
            Shaders::blurShader->VerticalBlur->GetPassByIndex(p)->Apply(0, context);

            UINT numGroups = (UINT)ceilf(height / NUM_THREADS);
            context->Dispatch(width, numGroups, 1);
        }

        context->CSSetShaderResources(0, 1, nullSRV);
        context->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

    }

    context->CSSetShader(0, 0, 0);
}

void Blur::SetGauss(float s)
{

    float d = 2.0f * s * s;

    float weights[9];
    float sum = 0.0f;
    for (int i = 0; i < 8; ++i)
    {
        float x = (float)i;
        weights[i] = expf(-x * x / d);

        sum += weights[i];
    }

    // Divide by the sum so all the weights add up to 1.0.
    for (int i = 0; i < 8; ++i)
    {
        weights[i] /= sum;
    }

    Shaders::blurShader->SetWeights(weights);

}
