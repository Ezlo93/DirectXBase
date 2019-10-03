#include "InputLayout.h"
#include "Shader.h"


const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Pos[1] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Standard[4] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TANGENT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::StandardSkinned[6] =
{
    {"POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TANGENT",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"WEIGHTS",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"BONEINDICES",  0, DXGI_FORMAT_R8G8B8A8_UINT,   0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Particle[5] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
};


ID3D11InputLayout* InputLayouts::Pos = 0;
ID3D11InputLayout* InputLayouts::Standard = 0;
ID3D11InputLayout* InputLayouts::StandardSkinned = 0;
ID3D11InputLayout* InputLayouts::Particle = 0;


void InputLayouts::Init(ID3D11Device* device)
{
    D3DX11_PASS_DESC passDesc;


    Shaders::skyShader->SkyTech->GetPassByIndex(0)->GetDesc(&passDesc);
    device->CreateInputLayout(InputLayoutDesc::Pos, 1, passDesc.pIAInputSignature,
       passDesc.IAInputSignatureSize, &Pos);


    Shaders::basicTextureShader->BasicTextureTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
    device->CreateInputLayout(InputLayoutDesc::Standard, 4, passDesc.pIAInputSignature,
       passDesc.IAInputSignatureSize, &Standard);

    Shaders::fireShader->StreamOutTech->GetPassByIndex(0)->GetDesc(&passDesc);
    device->CreateInputLayout(InputLayoutDesc::Particle, 5, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &Particle);
}

void InputLayouts::Destroy()
{
    DXRelease(Pos);
    DXRelease(Standard);
    DXRelease(Particle);
    DXRelease(StandardSkinned);
}
