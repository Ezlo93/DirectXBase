#pragma once

#include <DirectXMath.h>

using namespace DirectX;


class Camera
{

public:
    Camera();
    ~Camera();


    //camera vectors
    XMVECTOR getRightXM();
    XMFLOAT3 getRight();
    XMVECTOR getLookXM();
    XMFLOAT3 getLook();
    XMVECTOR getUpXM();
    XMFLOAT3 getUp();

    //frustum
    float getNearZ();
    float getFarZ();
    float getAspectRatio();
    float getFovY();
    float getFovX();

    //camera position
    XMVECTOR getPositionXM();
    XMFLOAT3 getPosition();
    void setPosition(float x, float y, float z);
    void setPosition(const XMFLOAT3& p);

    //planes
    float getNearWWidth();
    float getNearWHeight();
    float getFarWWidth();
    float getFarWHeight();

    //set frustum
    void setLens(float _fovy, float _aspect, float _zn, float _zf);

    // look at
    void lookAt(FXMVECTOR _pos, FXMVECTOR _target, FXMVECTOR _up);
    void lookAt(const XMFLOAT3& _pos, const XMFLOAT3& _target, const XMFLOAT3& _up);

    // get matrices
    XMMATRIX getView();
    XMMATRIX getProj();
    XMMATRIX getViewProj();

    //rotate camera
    void pitch(float angle);
    void yaw(float angle);
    void roll(float angle);

    //test
    void walk(float d);
    void strafe(float d);


    void UpdateViewMatrix();

private:
    XMFLOAT3 position;
    XMFLOAT3 right;
    XMFLOAT3 up;
    XMFLOAT3 look;


    XMFLOAT4X4 viewMatrix;
    XMFLOAT4X4 projMatrix;

    //frustum
    float nearZ, farZ, aspectRatio, fovY, nearWHeight, farWHeight;

};