#include "Camera.h"


Camera::Camera() : position(0.f,0.f,0.f), right(1.f,0.f,0.f),up(0.f,1.f,0.f),look(0.f,0.f,1.f)
{
    // 0.25f * pi = 90° FOV
    setLens(0.2f * XM_PI, 1.f, .01f, 1000.f);
    yAxis = XMVectorSet(0.f, 1.0f, 0.f, 0.f);
}

Camera::~Camera()
{

}


//camera vectors
XMVECTOR Camera::getRightXM()
{
    return XMLoadFloat3(&right);
}

XMFLOAT3 Camera::getRight()
{
    return right;
}

XMVECTOR Camera::getLookXM()
{
    return XMLoadFloat3(&look);
}

XMFLOAT3 Camera::getLook()
{
    return look;
}

XMVECTOR Camera::getUpXM()
{
    return XMLoadFloat3(&up);
}

XMFLOAT3 Camera::getUp()
{
    return up;
}


//frustum
float Camera::getNearZ()
{
    return nearZ;
}

float Camera::getFarZ()
{
    return farZ;
}

float Camera::getAspectRatio()
{
    return aspectRatio;
}

float Camera::getFovX()
{
    float t = 0.5f * getNearWWidth();
    return (float)(2.f*atan(t/nearZ));
}

float Camera::getFovY()
{
    return fovY;
}

//camera position
XMVECTOR Camera::getPositionXM()
{
    return XMLoadFloat3(&position);
}

XMFLOAT3 Camera::getPosition()
{
    return position;
}

void Camera::setPosition(float x, float y, float z)
{
    position = XMFLOAT3(x, y, z);
}

void Camera::setPosition(const XMFLOAT3& p)
{
    position = p;
}

//planes
float Camera::getNearWWidth()
{
    return aspectRatio * nearWHeight;
}

float Camera::getNearWHeight()
{
    return nearWHeight;
}

float Camera::getFarWWidth()
{
    return aspectRatio * farWHeight;
}

float Camera::getFarWHeight()
{
    return farWHeight;
}

//set frustum
void Camera::setLens(float _fovy, float _aspect, float _zn, float _zf)
{
    fovY = _fovy;
    aspectRatio = _aspect;
    nearZ = _zn;
    farZ = _zf;

    nearWHeight = 2.f * nearZ * tanf(0.5f * fovY);
    farWHeight = 2.f * farZ * tanf(0.5f * fovY);

    XMMATRIX p = XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
    XMStoreFloat4x4(&projMatrix, p);

}

//look at
void Camera::lookAt(FXMVECTOR _pos, FXMVECTOR _target, FXMVECTOR _up)
{
    XMVECTOR L = XMVector3Normalize(XMVectorSubtract(_target, _pos));
    XMVECTOR R = XMVector3Normalize(XMVector3Cross(_up, L));
    XMVECTOR U = XMVector3Cross(L, R);

    XMStoreFloat3(&position, _pos);
    XMStoreFloat3(&look, L);
    XMStoreFloat3(&right, R);
    XMStoreFloat3(&up, U);

}

void Camera::lookAt(const XMFLOAT3& _pos, const XMFLOAT3& _target, const XMFLOAT3& _up)
{
    XMVECTOR P = XMLoadFloat3(&_pos);
    XMVECTOR T = XMLoadFloat3(&_target);
    XMVECTOR U = XMLoadFloat3(&_up);

    lookAt(P, T, U);

}


//get matrices
XMMATRIX Camera::getView()
{
    return XMLoadFloat4x4(&viewMatrix);
}

XMMATRIX Camera::getProj()
{
    return XMLoadFloat4x4(&projMatrix);
}

XMMATRIX Camera::getViewProj()
{
    return XMMatrixMultiply(getView(), getProj());
}

//rotate camera
void Camera::pitch(float angle)
{
    XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&right), angle);

    /*dont overrotate camera*/

    /*compute angle between look vector and y axis*/
    XMVECTOR t = XMVector3TransformNormal(XMLoadFloat3(&look), R);
    XMVECTOR a = XMVector3AngleBetweenNormals(yAxis, t);

    float convertedAngle = XMConvertToDegrees(XMVectorGetX(a));

    if (convertedAngle < CAMERA_RESTRICTION_ANGLE || convertedAngle > (180.f-CAMERA_RESTRICTION_ANGLE))
    {
        return;
    }

    /*set values if test passed*/
    XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
    XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), R));
}

void Camera::yaw(float angle)
{
    XMMATRIX R = XMMatrixRotationY(angle);

    XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), R));
    XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
    XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), R));
}

void Camera::roll(float angle)
{
    XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&look), angle);

    XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), R));
    XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), R));
}

void Camera::UpdateViewMatrix()
{
    XMVECTOR R = XMLoadFloat3(&right);
    XMVECTOR U = XMLoadFloat3(&up);
    XMVECTOR L = XMLoadFloat3(&look);
    XMVECTOR P = XMLoadFloat3(&position);

    // Keep camera's axes orthogonal to each other and of unit length.
    L = XMVector3Normalize(L);
    U = XMVector3Normalize(XMVector3Cross(L, R));

    // U, L already ortho-normal, so no need to normalize cross product.
    R = XMVector3Cross(U, L);

    // Fill in the view matrix entries.
    float x = -XMVectorGetX(XMVector3Dot(P, R));
    float y = -XMVectorGetX(XMVector3Dot(P, U));
    float z = -XMVectorGetX(XMVector3Dot(P, L));

    XMStoreFloat3(&right, R);
    XMStoreFloat3(&up, U);
    XMStoreFloat3(&look, L);

    viewMatrix(0, 0) = right.x;
    viewMatrix(1, 0) = right.y;
    viewMatrix(2, 0) = right.z;
    viewMatrix(3, 0) = x;

    viewMatrix(0, 1) = up.x;
    viewMatrix(1, 1) = up.y;
    viewMatrix(2, 1) = up.z;
    viewMatrix(3, 1) = y;

    viewMatrix(0, 2) = look.x;
    viewMatrix(1, 2) = look.y;
    viewMatrix(2, 2) = look.z;
    viewMatrix(3, 2) = z;

    viewMatrix(0, 3) = 0.0f;
    viewMatrix(1, 3) = 0.0f;
    viewMatrix(2, 3) = 0.0f;
    viewMatrix(3, 3) = 1.0f;
}


// test movement

void Camera::strafe(float d)
{
    // mPosition += d*mRight
    XMVECTOR s = XMVectorReplicate(d);
    XMVECTOR r = XMLoadFloat3(&right);
    XMVECTOR p = XMLoadFloat3(&position);
    XMStoreFloat3(&position, XMVectorMultiplyAdd(s, r, p));
}

void Camera::walk(float d)
{
    // mPosition += d*mLook
    XMVECTOR s = XMVectorReplicate(d);
    XMVECTOR l = XMLoadFloat3(&look);
    XMVECTOR p = XMLoadFloat3(&position);
    XMStoreFloat3(&position, XMVectorMultiplyAdd(s, l, p));
}