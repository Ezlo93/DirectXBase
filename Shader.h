#pragma once

#include "util.h"
#include "d3dx11effect.h"
#include "Model.h"

namespace UShader {
    enum UsedShader
    {
        Basic
    };
}

namespace UTech {
    enum UsedTechnique
    {
        Basic,
        BasicNoTexture,
        BasicNoLighting
    };
}

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


/*used for the skybox*/
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

/*used for texture testing - no lighting normal mapping etc*/
class BasicTextureShader : public Shader
{
public:
    BasicTextureShader(ID3D11Device* device, const std::wstring& filename);
    ~BasicTextureShader();

    ID3DX11EffectTechnique* BasicTextureTechnique;
    ID3DX11EffectTechnique* BasicNoTextureTechnique;
    ID3DX11EffectTechnique* BasicTextureNoLighting;

    ID3DX11EffectMatrixVariable* WorldViewProj;
    ID3DX11EffectMatrixVariable* World;
    ID3DX11EffectMatrixVariable* WorldInvTranspose;
    ID3DX11EffectVectorVariable* EyePosW;
    ID3DX11EffectMatrixVariable* TexTransform;
    ID3DX11EffectShaderResourceVariable* DiffuseMap;
    ID3DX11EffectVariable* Mat;
    ID3DX11EffectVariable* DirLights;

    void SetWorldViewProj(DirectX::CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetWorld(CXMMATRIX M) { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetWorldInvTranspose(CXMMATRIX M) { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetTexture(ID3D11ShaderResourceView* texture) { DiffuseMap->SetResource(texture); }
    void SetTexTransform(DirectX::CXMMATRIX M) { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
    void SetMaterial(const Material::Standard& mat) { Mat->SetRawValue(&mat, 0, sizeof(Material::Standard)); }
    void SetDirLights(const DirectionalLight lights) { DirLights->SetRawValue(&lights, 0, sizeof(DirectionalLight)); }

};



class Shaders
{
public:
    static void Init(ID3D11Device* device);
    static void Destroy();

    static SkyboxShader* skyShader;
    static BasicTextureShader* basicTextureShader;
};
