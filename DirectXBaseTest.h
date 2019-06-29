#pragma once

#include "DirectXBase.h"
#ifdef _DEBUG
#pragma comment(lib, "Effects11d.lib")
#else
#pragma comment(lib, "Effects11.lib")
#endif
#include "d3dx11effect.h"
#include "ControllerInput.h"
#include "Skybox.h"

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
    ControllerInput* controllerInput;

    float clearColor[4];

    void buildCube();
    void buildShader();
    void buildLayout();

    Skybox *skybox;

    ID3D11Buffer* boxVB;
    ID3D11Buffer* boxIB;

    ID3DX11Effect* effect;
    ID3DX11EffectTechnique* technique;
    ID3DX11EffectMatrixVariable* worldViewProj;

    ID3D11InputLayout* inputLayout;

    XMFLOAT4X4 boxWorld;
};