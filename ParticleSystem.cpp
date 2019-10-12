#include "ParticleSystem.h"
#include "InputLayout.h"

ParticleSystem::ParticleSystem()
{
    firstRun = true;
    mStreamVB = 0;
    mTexArraySRV = 0;
    mRandomSRV = 0;
    mEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
    mEmitPositionW = XMFLOAT3(0.0f, 0.0f, 0.0f);
    mEmitDirectionW = XMFLOAT3(0.0f, 1.0f, 0.0f);
    mAcceleration = XMFLOAT3(0.f, -9.8f, 0.f);
    mSizeParticle = XMFLOAT2(1.0f, 1.0f);
}

ParticleSystem::~ParticleSystem()
{
    DXRelease(mInitVB);
    DXRelease(mDrawVB);
    DXRelease(mStreamVB);
}

float ParticleSystem::getAge()
{
    return age;
}

void ParticleSystem::setEyePos(const XMFLOAT3& eyePosW)
{
    mEyePosW = eyePosW;
}

void ParticleSystem::setEmitPosition(const XMFLOAT3& emitPosW)
{
    mEmitPositionW = emitPosW;
}

void ParticleSystem::setEmitDirection(const XMFLOAT3& emitDirW)
{
    mEmitDirectionW = emitDirW;
}

void ParticleSystem::setAcceleration(const XMFLOAT3& acc)
{
    mAcceleration = acc;
}

void ParticleSystem::setSizeParticle(const XMFLOAT2& size)
{
    mSizeParticle = size;
}



void ParticleSystem::init(ID3D11Device* device, ParticleEffect* fx, ID3D11ShaderResourceView* texArraySRV,
                          ID3D11ShaderResourceView* randomTexSRV, UINT maxP)
{
    maxParticles = maxP;

    effect = fx;

    mTexArraySRV = texArraySRV;
    mRandomSRV = randomTexSRV;

    mSizeParticle = XMFLOAT2(1.f, 1.f);
    mAcceleration = XMFLOAT3(0.f, 8.f, 0.f);

    buildVertexBuffer(device);
}

void ParticleSystem::reset()
{
    firstRun = true;
    age = 0.0f;
}

void ParticleSystem::update(float dt, float gt)
{
    gameTime = gt;
    timeStep = dt;

    age += dt;
}

void ParticleSystem::draw(ID3D11DeviceContext* dc, Camera& cam)
{
    XMMATRIX VP = cam.getViewProj();

    //
    // Set constants.
    //
    effect->SetViewProj(VP);
    effect->SetGameTime(gameTime);
    effect->SetTimeStep(timeStep);
    effect->SetEyePosW(mEyePosW);
    effect->SetEmitPosW(mEmitPositionW);
    effect->SetEmitDirW(mEmitDirectionW);
    effect->SetAccelerationW(mAcceleration);
    effect->SetSizeParticle(mSizeParticle);
    effect->SetTexArray(mTexArraySRV);
    effect->SetRandomTex(mRandomSRV);

    //
    // Set IA stage.
    //
    dc->IASetInputLayout(InputLayouts::Particle);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    UINT stride = sizeof(Vertex::Particle);
    UINT offset = 0;

    // On the first pass, use the initialization VB.  Otherwise, use
    // the VB that contains the current particle list.
    if (firstRun)
        dc->IASetVertexBuffers(0, 1, &mInitVB, &stride, &offset);
    else
        dc->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

    //
    // Draw the current particle list using stream-out only to update them.  
    // The updated vertices are streamed-out to the target VB. 
    //
    dc->SOSetTargets(1, &mStreamVB, &offset);

    D3DX11_TECHNIQUE_DESC techDesc;
    effect->StreamOutTech->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        effect->StreamOutTech->GetPassByIndex(p)->Apply(0, dc);

        if (firstRun)
        {
            dc->Draw(1, 0);
            firstRun = false;
        }
        else
        {
            dc->DrawAuto();
        }
    }

    // done streaming-out--unbind the vertex buffer
    ID3D11Buffer* bufferArray[1] = { 0 };
    dc->SOSetTargets(1, bufferArray, &offset);

    // ping-pong the vertex buffers
    std::swap(mDrawVB, mStreamVB);

    //
    // Draw the updated particle system we just streamed-out. 
    //
    dc->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

    effect->DrawTech->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        effect->DrawTech->GetPassByIndex(p)->Apply(0, dc);

        dc->DrawAuto();
    }
}

void ParticleSystem::buildVertexBuffer(ID3D11Device* device)
{
    //
    // Create the buffer to kick-off the particle system.
    //

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(Vertex::Particle) * 1;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    // The initial particle emitter has type 0 and age 0.  The rest
    // of the particle attributes do not apply to an emitter.
    Vertex::Particle p;
    ZeroMemory(&p, sizeof(Vertex::Particle));
    p.Age = 0.0f;
    p.Type = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &p;

    device->CreateBuffer(&vbd, &vinitData, &mInitVB);

    //
    // Create the ping-pong buffers for stream-out and drawing.
    //
    vbd.ByteWidth = sizeof(Vertex::Particle) * maxParticles;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

    device->CreateBuffer(&vbd, 0, &mDrawVB);
    device->CreateBuffer(&vbd, 0, &mStreamVB);
}