#pragma once

#include "util.h"
#include "Camera.h"
#include "Model.h"
#include "ResourceManager.h"
#include "Player.h"

#define PLAYER_SPEED 50.f

#define DASH_CD 1.5f
#define DASH_DURATION .35f

#define CAMERA_DIST_UP 5.5f
#define CAMERA_DIST_BACK 33.0f

enum class PCState
{
    REST,
    MOVE,
    JUMP,
    DASH
};


class PlayableChar
{

public:
    PlayableChar(std::string id, ResourceManager* r);
    ~PlayableChar();

    XMFLOAT3 Translation, Rotation, Scale, Velocity;
    float Speed;
    float BaseHeight;

    void Update(float deltaTime);

    void initDash();

    void Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX shadowT);
    void ShadowDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX lightView, XMMATRIX lightProj);

    BoundingOrientedBox hitBox;
    Camera* getCamera();
    bool Orientation = false;
    bool npc = true;
    int metaPosition = -1;
    XMFLOAT4 Color;
    Player* controllingPlayer = 0;
    ID3D11RenderTargetView* splitScreenView;
    ID3D11ShaderResourceView* splitScreenSRV;
    ID3D11UnorderedAccessView* splitScreenUAV;

    PCState currState, prevState;
    float dashTimer = 0.f;
    int dashDirection = 1;

private:
    string modelID;
    ResourceManager* res = 0;
    XMFLOAT4X4 World, CamWorld;
    Camera* cam;
    float jumpTime = 0.f;
    float dashCooldown = 0.f;

};