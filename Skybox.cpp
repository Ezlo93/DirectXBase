#include "Skybox.h"
#include "Shader.h"
#include "InputLayout.h"

Skybox::Skybox(ID3D11Device* device, const std::wstring& skyboxFile, float radius) : iBuffer(0), vBuffer(0),skyboxSRV(0),sizeIndexBuffer(0)
{
    CreateDDSTextureFromFile(device, skyboxFile.c_str(), nullptr, &skyboxSRV);

    // sphere with radius, 30 slice & stack
    UINT slices = 30;
    UINT stacks = 30;

    skyVerticesFull.clear();
    skyIndices.clear();

    Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    skyVerticesFull.push_back(topVertex);

    float phiStep = XM_PI / stacks;
    float thetaStep = 2.0f*XM_PI / slices;

   //vertices
    for (UINT i = 1; i <= stacks - 1; ++i)
    {
        float phi = i * phiStep;

        // Vertices of ring.
        for (UINT j = 0; j <= slices; ++j)
        {
            float theta = j * thetaStep;

            Vertex v;

            // spherical to cartesian
            v.Position.x = radius * sinf(phi) * cosf(theta);
            v.Position.y = radius * cosf(phi);
            v.Position.z = radius * sinf(phi) * sinf(theta);

            // Partial derivative of P with respect to theta
            v.TangentU.x = -radius * sinf(phi) * sinf(theta);
            v.TangentU.y = 0.0f;
            v.TangentU.z = +radius * sinf(phi) * cosf(theta);

            XMVECTOR T = XMLoadFloat3(&v.TangentU);
            XMStoreFloat3(&v.TangentU, XMVector3Normalize(T));

            XMVECTOR p = XMLoadFloat3(&v.Position);
            XMStoreFloat3(&v.Normal, XMVector3Normalize(p));

            v.TexC.x = theta / XM_2PI;
            v.TexC.y = phi / XM_PI;

            skyVerticesFull.push_back(v);
        }
    }
    skyVerticesFull.push_back(bottomVertex);

    //indices
    for (UINT i = 1; i <= slices; ++i)
    {
        skyIndices.push_back(0);
        skyIndices.push_back(i + 1);
        skyIndices.push_back(i);
    }

    UINT baseIndex = 1;
    UINT ringVertexCount = slices + 1;
    for (UINT i = 0; i < stacks - 2; ++i)
    {
        for (UINT j = 0; j < slices; ++j)
        {
            skyIndices.push_back(baseIndex + i * ringVertexCount + j);
            skyIndices.push_back(baseIndex + i * ringVertexCount + j + 1);
            skyIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

            skyIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
            skyIndices.push_back(baseIndex + i * ringVertexCount + j + 1);
            skyIndices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
        }
    }

    UINT southPoleIndex = (UINT)skyVerticesFull.size() - 1;

    baseIndex = southPoleIndex - ringVertexCount;

    for (UINT i = 0; i < slices; ++i)
    {
        skyIndices.push_back(southPoleIndex);
        skyIndices.push_back(baseIndex + i);
        skyIndices.push_back(baseIndex + i + 1);
    }

    sizeIndexBuffer = (UINT)skyIndices.size();


    //Vertex Buffer

    skyVertices.clear();
    skyVertices.resize(skyVerticesFull.size());

    for (UINT i = 0; i < skyVerticesFull.size(); i++)
    {
        skyVertices[i] = skyVerticesFull[i].Position;
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(XMFLOAT3) * (UINT)(skyVertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &skyVertices[0];

    device->CreateBuffer(&vbd, &vinitData, &vBuffer);

    //Index Buffer
    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(USHORT) * sizeIndexBuffer;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.StructureByteStride = 0;
    ibd.MiscFlags = 0;

    std::vector<USHORT> indices16;
    indices16.assign(skyIndices.begin(), skyIndices.end());

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices16[0];

    HRESULT hr = device->CreateBuffer(&ibd, &iinitData, &iBuffer);

    if (FAILED(hr))
    {
        int i = 1;
    }
}

Skybox::~Skybox()
{
    DXRelease(skyboxSRV);
    DXRelease(vBuffer);
    DXRelease(iBuffer);
}

ID3D11ShaderResourceView* Skybox::getSkyboxSRV()
{
    return skyboxSRV;
}

void Skybox::Draw(ID3D11DeviceContext* context, Camera& cam)
{
    //middle of skybox = camera position
    XMFLOAT3 eyePosition = cam.getPosition();
    XMMATRIX T = XMMatrixTranslation(eyePosition.x, eyePosition.y, eyePosition.z);

    XMMATRIX worldViewProj = XMMatrixMultiply(T, cam.getViewProj());

    Shaders::skyShader->SetWorldViewProj(worldViewProj);
    Shaders::skyShader->SetCubeMap(skyboxSRV);

    UINT stride = sizeof(XMFLOAT3);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
    context->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R16_UINT, 0);
    context->IASetInputLayout(InputLayouts::Pos);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3DX11_TECHNIQUE_DESC technique;
    Shaders::skyShader->SkyTech->GetDesc(&technique);

    for (UINT p = 0; p < technique.Passes; p++)
    {
        ID3DX11EffectPass* pass = Shaders::skyShader->SkyTech->GetPassByIndex(p);
        pass->Apply(0, context);
        context->DrawIndexed(sizeIndexBuffer, 0, 0);
    }

}
