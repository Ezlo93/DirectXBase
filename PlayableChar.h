#pragma once

#include "util.h"
#include "Camera.h"
#include "Model.h"
#include "ResourceManager.h"
#include "Player.h"

struct PCHitbox
{
    XMFLOAT3 x,y;
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

    void Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX shadowT);
    void ShadowDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX lightView, XMMATRIX lightProj);

    BoundingOrientedBox hitBox;
    Camera* getCamera();
    bool Orientation = false;
    bool npc = true;
    XMFLOAT4 Color;
    Player* controllingPlayer = 0;
private:
    string modelID;
    ResourceManager* res = 0;
    XMFLOAT4X4 World;
    Camera* cam;
    float jumpTime = 0.f;
};