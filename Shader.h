#pragma once

#include "util.h"
#include "d3dx11effect.h"
#include "Model.h"

namespace UShader {
    enum UsedShader
    {
        Basic,
        Normal
    };
}

namespace UTech {
    enum UsedTechnique
    {
        Basic,
        BasicNoTexture,
        BasicNoLighting,

        NormalTech
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

/*basic texture & lighting*/
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
    ID3DX11EffectMatrixVariable* ShadowTransform;
    ID3DX11EffectShaderResourceVariable* DiffuseMap;
    ID3DX11EffectShaderResourceVariable* ShadowMap;
    ID3DX11EffectVariable* Mat;
    ID3DX11EffectVariable* DirLights;

    void SetWorldViewProj(DirectX::CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetWorld(CXMMATRIX M) { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetShadowTransform(CXMMATRIX M) { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetWorldInvTranspose(CXMMATRIX M) { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetTexture(ID3D11ShaderResourceView* texture) { DiffuseMap->SetResource(texture); }
    void SetShadowMap(ID3D11ShaderResourceView* tex) { ShadowMap->SetResource(tex); }
    void SetTexTransform(DirectX::CXMMATRIX M) { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
    void SetMaterial(const Material::Standard& mat) { Mat->SetRawValue(&mat, 0, sizeof(Material::Standard)); }
    void SetDirLights(const DirectionalLight lights) { DirLights->SetRawValue(&lights, 0, sizeof(DirectionalLight)); }

};

/*include normalmap in lighting calculation*/
class NormalMapShader : public Shader
{
public:
    NormalMapShader(ID3D11Device* device, const std::wstring& filename);
    ~NormalMapShader();

    ID3DX11EffectTechnique* NormalMapTech;

    ID3DX11EffectMatrixVariable* WorldViewProj;
    ID3DX11EffectMatrixVariable* World;
    ID3DX11EffectMatrixVariable* WorldInvTranspose;
    ID3DX11EffectVectorVariable* EyePosW;
    ID3DX11EffectMatrixVariable* TexTransform;
    ID3DX11EffectMatrixVariable* ShadowTransform;
    ID3DX11EffectShaderResourceVariable* DiffuseMap;
    ID3DX11EffectShaderResourceVariable* NormalMap;
    ID3DX11EffectShaderResourceVariable* ShadowMap;
    ID3DX11EffectVariable* Mat;
    ID3DX11EffectVariable* DirLights;

    void SetWorldViewProj(DirectX::CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetWorld(CXMMATRIX M) { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetWorldInvTranspose(CXMMATRIX M) { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetShadowTransform(CXMMATRIX M) { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetTexture(ID3D11ShaderResourceView* texture) { DiffuseMap->SetResource(texture); }
    void SetNormalMap(ID3D11ShaderResourceView* texture) { NormalMap->SetResource(texture); }
    void SetShadowMap(ID3D11ShaderResourceView* tex) { ShadowMap->SetResource(tex); }
    void SetTexTransform(DirectX::CXMMATRIX M) { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
    void SetMaterial(const Material::Standard& mat) { Mat->SetRawValue(&mat, 0, sizeof(Material::Standard)); }
    void SetDirLights(const DirectionalLight lights) { DirLights->SetRawValue(&lights, 0, sizeof(DirectionalLight)); }

};

/*build the shadow map in the depth srv*/
class ShadowMapShader : public Shader
{
public:
    ShadowMapShader(ID3D11Device* device, const std::wstring& filename);
    ~ShadowMapShader();

    void SetViewProj(CXMMATRIX M) { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetWorld(CXMMATRIX M) { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetWorldInvTranspose(CXMMATRIX M) { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetDiffuseMap(ID3D11ShaderResourceView* tex) { DiffuseMap->SetResource(tex); }

    ID3DX11EffectTechnique* ShadowMapTech;

    ID3DX11EffectMatrixVariable* ViewProj;
    ID3DX11EffectMatrixVariable* WorldViewProj;
    ID3DX11EffectMatrixVariable* World;
    ID3DX11EffectMatrixVariable* WorldInvTranspose;
    ID3DX11EffectShaderResourceVariable* DiffuseMap;
};


class BlurShader : public Shader
{
public:
    BlurShader(ID3D11Device* device, const std::wstring& filename);
    ~BlurShader();

    void SetWeights(const float _weights[9]) { weights->SetFloatArray(_weights, 0, 9); }
    void SetInput(ID3D11ShaderResourceView* tex) { input->SetResource(tex); }
    void SetOutput(ID3D11UnorderedAccessView* tex) { output->SetUnorderedAccessView(tex); }

    ID3DX11EffectTechnique* HorizontalBlur;
    ID3DX11EffectTechnique* VerticalBlur;

    ID3DX11EffectScalarVariable* weights;
    ID3DX11EffectShaderResourceVariable* input;
    ID3DX11EffectUnorderedAccessViewVariable* output;
};


/*debug texture*/

class DebugTexEffect : public Shader
{
public:
    DebugTexEffect(ID3D11Device* device, const std::wstring& filename);
    ~DebugTexEffect();

    void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetTexture(ID3D11ShaderResourceView* tex) { Texture->SetResource(tex); }

    ID3DX11EffectTechnique* ViewArgbTech;
    ID3DX11EffectTechnique* ViewRedTech;
    ID3DX11EffectTechnique* ViewGreenTech;
    ID3DX11EffectTechnique* ViewBlueTech;
    ID3DX11EffectTechnique* ViewAlphaTech;

    ID3DX11EffectMatrixVariable* WorldViewProj;
    ID3DX11EffectShaderResourceVariable* Texture;
};


class Shaders
{
public:
    static void Init(ID3D11Device* device);
    static void Destroy();

    static SkyboxShader* skyShader;
    static BasicTextureShader* basicTextureShader;
    static NormalMapShader* normalMapShader;
    static ShadowMapShader* shadowMapShader;
    static BlurShader* blurShader;
    static DebugTexEffect* DebugTexFX;
};
