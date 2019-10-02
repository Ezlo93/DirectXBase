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
        BasicStaticColor,
        BasicOnlyShadow,
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
    ID3DX11EffectTechnique* BasicStaticColor;
    ID3DX11EffectTechnique* BasicOnlyShadow;

    ID3DX11EffectMatrixVariable* WorldViewProj;
    ID3DX11EffectMatrixVariable* World;
    ID3DX11EffectMatrixVariable* WorldInvTranspose;
    ID3DX11EffectVectorVariable* EyePosW;
    ID3DX11EffectVectorVariable* StaticColor;
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
    void SetStaticColor(const XMFLOAT4& v) { StaticColor->SetRawValue(&v, 0, sizeof(XMFLOAT4)); }
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

/*blurs the complete screen*/
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

/*used for particle systems*/
class ParticleEffect : public Shader
{
public:
    ParticleEffect(ID3D11Device* device, const std::wstring& filename);
    ~ParticleEffect();

    void SetViewProj(CXMMATRIX M) { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }

    void SetGameTime(float f) { GameTime->SetFloat(f); }
    void SetTimeStep(float f) { TimeStep->SetFloat(f); }

    void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
    void SetEmitPosW(const XMFLOAT3& v) { EmitPosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
    void SetEmitDirW(const XMFLOAT3& v) { EmitDirW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }

    void SetTexArray(ID3D11ShaderResourceView* tex) { TexArray->SetResource(tex); }
    void SetRandomTex(ID3D11ShaderResourceView* tex) { RandomTex->SetResource(tex); }

    ID3DX11EffectTechnique* StreamOutTech;
    ID3DX11EffectTechnique* DrawTech;

    ID3DX11EffectMatrixVariable* ViewProj;
    ID3DX11EffectScalarVariable* GameTime;
    ID3DX11EffectScalarVariable* TimeStep;
    ID3DX11EffectVectorVariable* EyePosW;
    ID3DX11EffectVectorVariable* EmitPosW;
    ID3DX11EffectVectorVariable* EmitDirW;
    ID3DX11EffectShaderResourceVariable* TexArray;
    ID3DX11EffectShaderResourceVariable* RandomTex;
};

/*fullscreen quad texture*/

class FullscreenShader : public Shader
{
public:
    FullscreenShader(ID3D11Device* device, const std::wstring& filename);
    ~FullscreenShader();

    void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetTexture(ID3D11ShaderResourceView* tex) { Texture->SetResource(tex); }
    void SetFadeValue(const float v) { FadeValue->SetFloat(v); }

    ID3DX11EffectTechnique* ViewStandard;

    ID3DX11EffectMatrixVariable* WorldViewProj;
    ID3DX11EffectShaderResourceVariable* Texture;
    ID3DX11EffectScalarVariable* FadeValue;
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
    static FullscreenShader* fullscreenShader;
    static ParticleEffect* fireShader;
};
