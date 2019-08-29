#include "Ball.h"

Ball::Ball(std::string id, ResourceManager* r)
{

    res = r;
    modelID = id;
    XMStoreFloat4x4(&World, XMMatrixIdentity());

}

Ball::~Ball()
{

}

void Ball::Update()
{

}

void Ball::ConfirmUpdate()
{

}

void Ball::Draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX shadowT)
{


}

void Ball::ShadowDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Camera* c, XMMATRIX lightView, XMMATRIX lightProj)
{

}
