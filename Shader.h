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

    void SetWorldViewProj(DirectX::CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetTexture(ID3D11ShaderResourceView* texture) { DiffuseMap->SetResource(texture); }
    void SetTexTransform(DirectX::CXMMATRIX M) { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }


    ID3DX11EffectTechnique* BasicTextureTechnique;

    ID3DX11EffectMatrixVariable* WorldViewProj;
    ID3DX11EffectMatrixVariable* TexTransform;
    ID3DX11EffectShaderResourceVariable* DiffuseMap;
};



class Shaders
{
public:
    static void Init(ID3D11Device* device);
    static void Destroy();

    static SkyboxShader* skyShader;
    static BasicTextureShader* basicTextureShader;
};
