#pragma once

#include "DirectXBase.h"
#pragma comment(lib, "Effects11d.lib")
#include "d3dx11effect.h"

struct Vertex
{
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
};

class DXTest : public DirectXBase
{
public:
    DXTest(HINSTANCE hProgramID);
    ~DXTest();

    bool Initialisation();
    void OnWindowResize();
    void Update(float deltaTime);
    void Draw();
    bool goFullscreen(bool s);

private:
    float clearColor[4];


    void buildCube();
    void buildShader();
    void buildLayout();

    ID3D11Buffer* boxVB;
    ID3D11Buffer* boxIB;

    ID3DX11Effect* effect;
    ID3DX11EffectTechnique* technique;
    ID3DX11EffectMatrixVariable* worldViewProj;

    ID3D11InputLayout* inputLayout;

    XMFLOAT4X4 mWorld;
    XMFLOAT4X4 mView;
    XMFLOAT4X4 mProj;
};