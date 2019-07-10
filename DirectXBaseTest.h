#pragma once

#include "DirectXBase.h"
#ifdef _DEBUG
#pragma comment(lib, "Effects11d.lib")
#else
#pragma comment(lib, "Effects11.lib")
#endif
#include "d3dx11effect.h"
#include "InputManager.h"
#include "Skybox.h"
#include "ModelCollection.h"
#include "TextureCollection.h"

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

    ModelCollection* modelCollection;
    TextureCollection* textureCollection;

    InputManager* input;
    int controllingInput = -1;

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