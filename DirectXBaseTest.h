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
#include "ModelInstanceStatic.h"
#include "Level.h"
#include "ShadowMap.h"

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
    Level* testLevel;
    DirectionalLight gDirLights;

    float clearColor[4];

    Skybox *skybox;

    ID3DX11Effect* effect;

    bool renderWireFrame = false;
};