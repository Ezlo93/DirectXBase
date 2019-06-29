#pragma once
#include "DirectXBase.h"
#include <DirectXMath.h>
#include <string>
#include "d3dx11effect.h"

class Shader
{
public:
    Shader(ID3D11Device* device, const std::wstring& file);
    virtual ~Shader();

private:
    Shader(const Shader& s);
    Shader& operator=(const Shader& s);

protected:
    ID3DX11Effect* effect;

};


class SkyboxShader : public Shader
{
public:
    SkyboxShader(ID3D11Device* device, const std::wstring& filename);
    ~SkyboxShader();

    void SetWorldViewProj(DirectX::CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetCubeMap(ID3D11ShaderResourceView* cubemap) { CubeMap->SetResource(cubemap); }

    ID3DX11EffectTechnique* SkyTech;

    ID3DX11EffectMatrixVariable* WorldViewProj;

    ID3DX11EffectShaderResourceVariable* CubeMap;
};


class Shaders
{
public:
    static void Init(ID3D11Device* device);
    static void Destroy();

    static SkyboxShader* skyShader;
};
