#include "Shader.h"
#include <fstream>
#include <vector>

SkyboxShader* Shaders::skyShader = 0;

void Shaders::Init(ID3D11Device* device)
{
    skyShader = new SkyboxShader(device, L"data/shader/skybox.fxo");
}

void Shaders::Destroy()
{
    delete skyShader; skyShader = 0;
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

    D3DX11CreateEffectFromMemory(&compiledShader[0], size,
       0, device, &effect);
}

Shader::~Shader()
{
    DXRelease(effect);
}

/*shader*/

SkyboxShader::SkyboxShader(ID3D11Device* device, const std::wstring& filename)
    : Shader(device, filename)
{
    SkyTech = effect->GetTechniqueByName("SkyTech");
    WorldViewProj = effect->GetVariableByName("gWorldViewProj")->AsMatrix();
    CubeMap = effect->GetVariableByName("gCubeMap")->AsShaderResource();
}

SkyboxShader::~SkyboxShader()
{
}