#pragma once

#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>
#include <time.h>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include "constants.h"
#include "DDSTextureLoader.h"

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std;

/*often used macros and functions*/

/*release Dx11 COMs*/
#define DXRelease(x) { if(x && x != nullptr){x->Release(); x = 0; }}


/*print to vs output, only in debug mode*/
#ifdef _DEBUG
#define DBOUT( s )           \
{                            \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugString( os_.str().c_str() );  \
}
#else
#define DBOUT(s)
#endif

/*assertion, only debug mode*/

#ifdef _DEBUG

#define ASSERT(expr) \
    if(expr) { }\
    else\
    {\
        std::wostringstream os_;\
        os_ << "Assertion failed: " << #expr << " in " << __FILEW__ << " at line: " << __LINE__ <<endl;\
        OutputDebugString( os_.str().c_str() );\
        DebugBreak();\
    }
#else 
#define ASSERT(expr) //nothing
#endif

#define GRAVITY 9.81f

class DXMath
{
public:
    static XMMATRIX InverseTranspose(CXMMATRIX M)
    {
        // Inverse-transpose is just applied to normals.  So zero out 
        // translation row so that it doesn't get into our inverse-transpose
        // calculation--we don't want the inverse-transpose of the translation.
        XMMATRIX A = M;
        A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

        XMVECTOR det = XMMatrixDeterminant(A);
        return XMMatrixTranspose(XMMatrixInverse(&det, A));
    }

    template<typename T>
    static T Min(const T& a, const T& b)
    {
        return a < b ? a : b;
    }

    template<typename T>
    static T Max(const T& a, const T& b)
    {
        return a > b ? a : b;
    }

    template<typename T>
    static T Lerp(const T& a, const T& b, float t)
    {
        return a + (b - a) * t;
    }

    template<typename T>
    static T Clamp(const T& x, const T& low, const T& high)
    {
        return x < low ? low : (x > high ? high : x);
    }

    // Returns random float in [0, 1).
    static float RandF()
    {
        return (float)(rand()) / (float)RAND_MAX;
    }

    // Returns random float in [a, b).
    static float RandF(float a, float b)
    {
        return a + RandF() * (b - a);
    }

};

static ID3D11ShaderResourceView* CreateRandomTexture1DSRV(ID3D11Device* device)
{
    // 
    // Create the random data.
    //
    XMFLOAT4 randomValues[1024];

    for (int i = 0; i < 1024; ++i)
    {
        randomValues[i].x = DXMath::RandF(-1.0f, 1.0f);
        randomValues[i].y = DXMath::RandF(-1.0f, 1.0f);
        randomValues[i].z = DXMath::RandF(-1.0f, 1.0f);
        randomValues[i].w = DXMath::RandF(-1.0f, 1.0f);
    }

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = randomValues;
    initData.SysMemPitch = 1024 * sizeof(XMFLOAT4);
    initData.SysMemSlicePitch = 0;

    //
    // Create the texture.
    //
    D3D11_TEXTURE1D_DESC texDesc;
    texDesc.Width = 1024;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    texDesc.Usage = D3D11_USAGE_IMMUTABLE;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.ArraySize = 1;

    ID3D11Texture1D* randomTex = 0;
    device->CreateTexture1D(&texDesc, &initData, &randomTex);

    //
    // Create the resource view.
    //
    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
    viewDesc.Format = texDesc.Format;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
    viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
    viewDesc.Texture1D.MostDetailedMip = 0;

    ID3D11ShaderResourceView* randomTexSRV = 0;
    device->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV);

    DXRelease(randomTex);

    return randomTexSRV;
}

static ID3D11ShaderResourceView* CreateTexture2DArraySRV(ID3D11Device* device, ID3D11DeviceContext* context, std::vector<std::wstring>& filenames)
{


    UINT size = (UINT)filenames.size();
    std::vector<ID3D11Texture2D*> srcTex(size);

    ID3D11ShaderResourceView* texArraySRV = 0;

    /*load all texture in vector*/

    for (UINT i = 0; i < size; i++)
    {


        CreateDDSTextureFromFileEx(device,
           filenames[i].c_str(),
           0,
           D3D11_USAGE_STAGING,
           0,
           D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ,
           0,
           false,
           (ID3D11Resource**)&srcTex[i],
           nullptr,
           nullptr)        ;
    }

    /*Create texture array*/
    D3D11_TEXTURE2D_DESC texElemDesc;
    srcTex[0]->GetDesc(&texElemDesc);

    D3D11_TEXTURE2D_DESC texArrDesc;
    texArrDesc.Width = texElemDesc.Width;
    texArrDesc.Height = texElemDesc.Height;
    texArrDesc.MipLevels = texElemDesc.MipLevels;
    texArrDesc.ArraySize = size;
    texArrDesc.Format = texElemDesc.Format;
    texArrDesc.SampleDesc.Count = 1;
    texArrDesc.SampleDesc.Quality = 0;
    texArrDesc.Usage = D3D11_USAGE_DEFAULT;
    texArrDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texArrDesc.CPUAccessFlags = 0;
    texArrDesc.MiscFlags = 0;

    ID3D11Texture2D* texArray = 0;
    device->CreateTexture2D(&texArrDesc, 0, &texArray);

    /* copy textures to array */

    for (UINT i = 0; i < size; i++)
    {

        for (UINT m = 0; m < texElemDesc.MipLevels; m++)
        {
            D3D11_MAPPED_SUBRESOURCE mappedTex;

            context->Map(srcTex[i], m, D3D11_MAP_READ, 0, &mappedTex);
            context->UpdateSubresource(texArray, D3D11CalcSubresource(m, i, texElemDesc.MipLevels), 0,
                                       mappedTex.pData, mappedTex.RowPitch, mappedTex.DepthPitch);
            context->Unmap(srcTex[i], m);
        }

    }

    /* create srv for tex array*/

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

    srvDesc.Format = texArrDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = texArrDesc.MipLevels;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = size;

    device->CreateShaderResourceView(texArray, &srvDesc, &texArraySRV);

    DXRelease(texArray);

    for (UINT i = 0; i < size; i++)
    {
        DXRelease(srcTex[i]);
    }

    return texArraySRV;
}