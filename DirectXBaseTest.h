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
#include "Ball.h"
#include "PlayableChar.h"
#include "Player.h"
#include "ParticleSystem.h"

enum class MainGameState
{
    PLAYER_REGISTRATION, INGAME, END_SCREEN
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

    /*+++*/
    InputManager* input;

    ResourceManager* res;
    Level* activeLevel, *gameLevel, *endLevel;
    float clearColor[4], clearColorSec[4];
    Skybox* skybox;
    Camera introCamera;
    Camera endScreenCamera;
    float introCameraTime = 0;
    float endCameraTime = 0;

    /*gameplay related*/
    Ball* playball;
    std::vector<PlayableChar*> playCharacters;
    std::vector<Player*> players;
    std::vector<Player*> winOrder;
    MainGameState gameState = MainGameState::PLAYER_REGISTRATION;
    MainGameState prevGameState = MainGameState::PLAYER_REGISTRATION;

    int playerCount = 0;
    bool allDead = false;
    bool transToIngame = false;
    bool transToRegistration = false;
    bool transToEndScreen = false;
    XMFLOAT4 playerColors[4];

    float transitionTimer = 0.f;
    float endTimer = 0.f;
    void clearData();
    void setupEndScreen();

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

    /*particle system*/
    ParticleSystem mFire;
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    Blur blurEffect;
    int blurStrength = 0;

    int transitionInProgress = 0;
    bool UpdateTransition(float deltaTime);
    float fadeValue = 0.f;

    Camera* activeCamera = 0;
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