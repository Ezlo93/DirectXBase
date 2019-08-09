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
#include "Blur.h"

#define MODEL_PATH "data/models"
#define TEXTURE_PATH "data/textures"

#define POST_PROCESS

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
    float clearColor[4], clearColorSec[4];
    Skybox* skybox;

    /*lighting*/
    DirectionalLight gDirLights;
    float lightRotationAngle = 0.f;
    XMFLOAT3 originalLightDir;


    /*shadow related*/
    ShadowMap* shadowMap;

    XMFLOAT4X4 lightView, lightProj, shadowTransform;
    XMFLOAT3 originalLightDirection;

    BoundingSphere sceneBounds;
    void buildShadowTransform();

    /*render related*/
    Blur blurEffect;

    ID3D11Buffer* mScreenQuadVB;
    ID3D11Buffer* mScreenQuadIB;

    void BuildScreenQuadGeometryBuffers();
    void BuildOffscreenViews();
    void DrawScreenQuad(ID3D11ShaderResourceView* srv);

    ID3D11ShaderResourceView* mOffscreenSRV;
    ID3D11UnorderedAccessView* mOffscreenUAV;
    ID3D11RenderTargetView* mOffscreenRTV;

    bool renderWireFrame = false;
};