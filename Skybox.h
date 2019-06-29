#pragma once

#include "DirectXBase.h"
#include "Camera.h"
#include <string>
#include <vector>

class Skybox
{

public:

    struct Vertex
    {
        Vertex() : Position(0, 0, 0), Normal(0, 0, 0), TangentU(0, 0, 0), TexC(0, 0) {}
        Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
            : Position(p), Normal(n), TangentU(t), TexC(uv)
        {
        }
        Vertex(
            float px, float py, float pz,
            float nx, float ny, float nz,
            float tx, float ty, float tz,
            float u, float v)
            : Position(px, py, pz), Normal(nx, ny, nz),
            TangentU(tx, ty, tz), TexC(u, v)
        {
        }

        XMFLOAT3 Position;
        XMFLOAT3 Normal;
        XMFLOAT3 TangentU;
        XMFLOAT2 TexC;
    };

    Skybox(ID3D11Device* device, const std::wstring& skyboxFile, float radius);
    ~Skybox();

    ID3D11ShaderResourceView* getSkyboxSRV();
    void Draw(ID3D11DeviceContext* context, Camera& cam);

private:
    ID3D11Buffer *vBuffer;
    ID3D11Buffer* iBuffer;

    ID3D11ShaderResourceView *skyboxSRV;

    std::vector<Vertex> skyVerticesFull;
    std::vector<XMFLOAT3> skyVertices;

    std::vector<UINT> skyIndices;

    UINT sizeIndexBuffer;

};