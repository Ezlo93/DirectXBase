#include "InputLayout.h"
#include "Shader.h"


const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Pos[1] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Basic32[3] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosTexNormalTan[4] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TANGENT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosTexNormalTanSkinned[6] =
{
    {"POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TANGENT",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"WEIGHTS",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"BONEINDICES",  0, DXGI_FORMAT_R8G8B8A8_UINT,   0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0}
};




ID3D11InputLayout* InputLayouts::Pos = 0;
ID3D11InputLayout* InputLayouts::Basic32 = 0;
ID3D11InputLayout* InputLayouts::PosTexNormalTan = 0;
ID3D11InputLayout* InputLayouts::PosTexNormalTanSkinned = 0;




void InputLayouts::Init(ID3D11Device* device)
{
    D3DX11_PASS_DESC passDesc;

    //
    // Pos
    //

    Shaders::skyShader->SkyTech->GetPassByIndex(0)->GetDesc(&passDesc);
    device->CreateInputLayout(InputLayoutDesc::Pos, 1, passDesc.pIAInputSignature,
       passDesc.IAInputSignatureSize, &Pos);

    
    //
    // Basic32
    //

    Shaders::basicTextureShader->BasicTextureTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
    device->CreateInputLayout(InputLayoutDesc::PosTexNormalTan, 4, passDesc.pIAInputSignature,
       passDesc.IAInputSignatureSize, &PosTexNormalTan);
    /*
    //
    // NormalMap
    //

    Effects::NormalMapFX->Light1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
    HR(device->CreateInputLayout(InputLayoutDesc::PosTexNormalTan, 4, passDesc.pIAInputSignature,
       passDesc.IAInputSignatureSize, &PosTexNormalTan));

    //
    // NormalMapSkinned
    //

    Effects::NormalMapFX->Light1SkinnedTech->GetPassByIndex(0)->GetDesc(&passDesc);
    HR(device->CreateInputLayout(InputLayoutDesc::PosTexNormalTanSkinned, 6, passDesc.pIAInputSignature,
       passDesc.IAInputSignatureSize, &PosTexNormalTanSkinned));*/
}

void InputLayouts::Destroy()
{
    DXRelease(Pos);
    DXRelease(Basic32);
    DXRelease(PosTexNormalTan);
    DXRelease(PosTexNormalTanSkinned);
}
