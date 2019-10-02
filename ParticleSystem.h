#pragma once
#include "util.h"
#include "Camera.h"
#include "Shader.h"

class ParticleSystem
{
public:
    ParticleSystem();
    ~ParticleSystem();

    float getAge();
    void setEyePos(const XMFLOAT3& eyePosW);
    void setEmitPosition(const XMFLOAT3& emitPosW);
    void setEmitDirection(const XMFLOAT3& emitDirW);

    void init(ID3D11Device* device, ParticleEffect* fx, ID3D11ShaderResourceView* texArraySRV,
              ID3D11ShaderResourceView* randomSRV, UINT maxP);

    void reset();
    void update(float deltaTime, float gameTime);
    void draw(ID3D11DeviceContext* context, Camera& cam);

private:

    void buildVertexBuffer(ID3D11Device* device);

    UINT maxParticles = 0;
    bool firstRun = true;
    float gameTime = 0.f;
    float timeStep = 0.f;
    float age = 0.f;

    XMFLOAT3 mEyePosW;
    XMFLOAT3 mEmitPositionW;
    XMFLOAT3 mEmitDirectionW;

    ParticleEffect* effect = 0;

    ID3D11Buffer* mInitVB = 0, * mDrawVB = 0, *mStreamVB = 0;

    ID3D11ShaderResourceView* mTexArraySRV, * mRandomSRV;

};