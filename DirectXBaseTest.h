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
#include "ResourceManager.h"
#include <chrono>



#define MODEL_PATH "data/models"
#define TEXTURE_PATH "data/textures"

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

    InputManager* input;
    int controllingInput = -1;

    ResourceManager* res;
    DirectionalLight gDirLights[3];

    float clearColor[4];

    Skybox *skybox;

    ID3DX11Effect* effect;
    ID3DX11EffectMatrixVariable* worldViewProj;

    XMFLOAT4X4 boxWorld;
};