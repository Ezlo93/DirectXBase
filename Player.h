#pragma once

#include "util.h"
#include "Camera.h"
#include "Model.h"
#include "ResourceManager.h"

class Player
{

public:
    Player();
    ~Player();

    XMFLOAT3 Translation, Rotation, Scale, Velocity;

    void Update(float deltaTime);

private:
    string modelID;
    ResourceManager* res = 0;
    XMFLOAT4X4 World;

};