#pragma once

#include "util.h"
#include "Model.h"
#include "ResourceManager.h"
#include "Camera.h"
#include "PlayableChar.h"

enum class BallState
{
    SPAWN, FREEZE, INPLAY, RESET
};

class Ball
{

public:
    Ball(std::string id, ResourceManager* r, std::vector<PlayableChar*> p);
    ~Ball();

    XMFLOAT3 Translation, Rotation, Scale, Velocity, Direction;
    bool Collision = true;
    BoundingSphere hitBox;
    XMFLOAT4 Color;
    int lastTouch = -1;

    void Update(float deltaTime);

    void Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX shadowT);
    void ShadowDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX lightView, XMMATRIX lightProj);

    void resetBallFull();
private:

    std::string modelID;
    ResourceManager* res = 0;
    XMFLOAT4X4 World;

    std::vector<PlayableChar*> players;

    float bounceFactor;
    float bounceTime = 0.f;
    float resetTime = 0.f;
    float spawnTime = 0.f;
    float ballHeight;
    bool collisionOn = true;
    bool resetB = false;
    XMFLOAT3 distanceV;

    BallState ballState;
    
    void resetBall();
};