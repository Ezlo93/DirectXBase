#pragma once

#include "util.h"
#include "Model.h"
#include "ResourceManager.h"
#include "Camera.h"

#define BALL_BORDER 43

enum BallState
{
    SPAWN, INPLAY, RESET
};

class Ball
{

public:
    Ball(std::string id, ResourceManager* r);
    ~Ball();

    XMFLOAT3 Translation, Rotation, Scale, Velocity;
    bool Collision = true;

    void Update(float deltaTime);

    void Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX shadowT);
    void ShadowDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX lightView, XMMATRIX lightProj);
private:

    std::string modelID;
    ResourceManager* res = 0;
    XMFLOAT4X4 World;

    float bounceFactor;
    float bounceTime;
    float ballHeight;

    BallState ballState;
};