#include "Shader.h"
#include <fstream>

SkyboxShader* Shaders::skyShader = 0;
BasicTextureShader* Shaders::basicTextureShader = 0;

void Shaders::Init(ID3D11Device* device)
{
    skyShader = new SkyboxShader(device, L"data/shader/skybox.fxo");
    basicTextureShader = new BasicTextureShader(device, L"data/shader/basictexture.fxo");
}

void Shaders::Destroy()
{
    delete skyShader; skyShader = 0;
    delete basicTextureShader; basicTextureShader = 0;
}


Shader::Shader(ID3D11Device* device, const std::wstring& filename)
    : effect(0)
{
    std::ifstream fin(filename.c_str(), std::ios::binary);

    fin.seekg(0, std::ios_base::end);
    int size = (int)fin.tellg();
    fin.seekg(0, std::ios_base::beg);
    std::vector<char> compiledShader(size);

    fin.read(&compiledShader[0], size);
    fin.close();

    HRESULT hr = D3DX11CreateEffectFromMemory(&compiledShader[0], size,
       0, device, &effect);

    if (FAILED(hr))
    {
        throw std::exception("failed to load shader");
    }

}

Shader::~Shader()
{
    DXRelease(effect);
}

/*shader*/


/*skybox shader*/
SkyboxShader::SkyboxShader(ID3D11Device* device, const std::wstring& filename)
    : Shader(device, filename)
{
    SkyTech = effect->GetTechniqueByName("SkyTech");
    WorldViewProj = effect->GetVariableByName("gWorldViewProj")->AsMatrix();
    CubeMap = effect->GetVariableByName("gCubeMap")->AsShaderResource();
}

SkyboxShader::~SkyboxShader()
{
    DXRelease(SkyTech);
    DXRelease(WorldViewProj);
    DXRelease(CubeMap);
}

/*basic texture shader*/

BasicTextureShader::BasicTextureShader(ID3D11Device* device, const std::wstring& filename) : Shader(device, filename)
{
    BasicTextureTechnique = effect->GetTechniqueByName("BasicTextureTech");
    WorldViewProj = effect->GetVariableByName("gWorldViewProj")->AsMatrix();
    DiffuseMap = effect->GetVariableByName("diffuseMap")->AsShaderResource();
    TexTransform = effect->GetVariableByName("gTexTransform")->AsMatrix();
    World = effect->GetVariableByName("gWorld")->AsMatrix();
    WorldInvTranspose = effect->GetVariableByName("gWorldInvTranspose")->AsMatrix();
    EyePosW = effect->GetVariableByName("gEyePosW")->AsVector(); // not found??
    Mat = effect->GetVariableByName("gMaterial");
}

BasicTextureShader::~BasicTextureShader()
{
    DXRelease(BasicTextureTechnique);
    DXRelease(WorldViewProj);
    DXRelease(DiffuseMap);
    DXRelease(TexTransform);
    DXRelease(World);
    DXRelease(WorldInvTranspose);
    DXRelease(EyePosW);
    DXRelease(Mat);
}
