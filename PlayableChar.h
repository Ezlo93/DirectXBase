#pragma once

#include "util.h"
#include "Camera.h"
#include "Model.h"
#include "ResourceManager.h"

class PlayableChar
{

public:
    PlayableChar(std::string id, ResourceManager* r);
    ~PlayableChar();

    XMFLOAT3 Translation, Rotation, Scale, Velocity;
    float Speed;

    void Update(float deltaTime);

    void Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX shadowT);
    void ShadowDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX lightView, XMMATRIX lightProj);

    string texID;
    Camera* getCamera();

private:
    string modelID;
    ResourceManager* res = 0;
    XMFLOAT4X4 World;
    Camera* cam;
};