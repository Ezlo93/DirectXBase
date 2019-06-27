/*DirectXBaseTest.cpp 
    minimum template for directx11 application

    extends DirectXBase class

    Nicolai Sehrt
*/

#include "DirectXBaseTest.h"
#include <fstream>
#include <vector>


#pragma warning (disable : 28251)

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nShowCmd)
{

    DXTest dxbase(hInstance);

    if (!dxbase.Initialisation())
        return 0;

    return dxbase.Run();
}


DXTest::DXTest(HINSTANCE hProgramID) : DirectXBase(hProgramID)
{
    wndTitle = L"DirectXBaseTest";

    /*clear color to silver*/
    clearColor[0] = 0.75f;
    clearColor[1] = 0.75f;
    clearColor[2] = 0.75f;
    clearColor[3] = 1.f;

}

DXTest::~DXTest()
{
    DXRelease(boxVB);
    DXRelease(boxIB);
    DXRelease(inputLayout);
    DXRelease(effect);
}

bool DXTest::Initialisation()
{
    if (!DirectXBase::Initialisation())
    {
        return false;
    }

    /*...*/

    //goFullscreen(true);

    XMMATRIX I = XMMatrixIdentity();
    XMStoreFloat4x4(&mWorld, I);
    XMStoreFloat4x4(&mView, I);
    XMStoreFloat4x4(&mProj, I);

    buildCube();
    buildShader();
    buildLayout();

    return true;
}


void DXTest::OnWindowResize()
{
    DirectXBase::OnWindowResize();

    /*recalc. aspect ratio*/
    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * XM_PI, getAspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&mProj, P);
}

bool DXTest::goFullscreen(bool s)
{
    if (!DirectXBase::goFullscreen(s))
    {
        return false;
    }

    OnWindowResize();

    /*potentially additional proj matrix etc recalc*/
    

    return true;
}


void DXTest::Update(float deltaTime)
{
    

    /*game logic*/


    XMVECTOR pos = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&mView, V);

}


void DXTest::Draw()
{
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    /*draw*/
    deviceContext->IASetInputLayout(inputLayout);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &boxVB, &stride, &offset);
    deviceContext->IASetIndexBuffer(boxIB, DXGI_FORMAT_R32_UINT, 0);

    // Set constants
    XMMATRIX world = XMLoadFloat4x4(&mWorld);
    XMMATRIX view = XMLoadFloat4x4(&mView);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);
    XMMATRIX resworldViewProj = world * view * proj;

    worldViewProj->SetMatrix(reinterpret_cast<float*>(&resworldViewProj));

    D3DX11_TECHNIQUE_DESC techDesc;
    technique->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        technique->GetPassByIndex(p)->Apply(0, deviceContext);

        // 36 indices for the box.
        deviceContext->DrawIndexed(36, 0, 0);
    }

    swapChain->Present(0, 0);
}


void DXTest::buildCube()
{
    // Create vertex buffer
    Vertex vertices[] =
    {
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.8f,0.1f,0.71f,1.f)   },
        { XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(0.8f,0.1f,0.71f,1.f)   },
        { XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(0.8f,0.1f,0.71f,1.f)   },
        { XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(0.8f,0.1f,0.71f,1.f)   },
        { XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(0.8f,0.5f,0.31f,1.f)   },
        { XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(0.8f,0.5f,0.31f,1.f)   },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(0.8f,0.5f,0.81f,1.f)   },
        { XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(0.8f,0.5f,0.71f,1.f)   },
    };

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * 8;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;
    device->CreateBuffer(&vbd, &vinitData, &boxVB);


    // Create the index buffer

    UINT indices[] = {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 6, 5,
        4, 7, 6,

        // left face
        4, 5, 1,
        4, 1, 0,

        // right face
        3, 2, 6,
        3, 6, 7,

        // top face
        1, 5, 6,
        1, 6, 2,

        // bottom face
        4, 0, 3,
        4, 3, 7
    };

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * 36;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;
    device->CreateBuffer(&ibd, &iinitData, &boxIB);
}


void DXTest::buildLayout()
{
    // Create the vertex input layout.
    D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    // Create the input layout
    D3DX11_PASS_DESC passDesc;
    technique->GetPassByIndex(0)->GetDesc(&passDesc);
    device->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
       passDesc.IAInputSignatureSize, &inputLayout);
}


void DXTest::buildShader()
{

    std::ifstream fin(L"shader/color.fxo", std::ios::binary);

    fin.seekg(0, std::ios_base::end);
    int size = (int)fin.tellg();
    fin.seekg(0, std::ios_base::beg);

    if (size <= 0)
    {
        MessageBox(wndHandle, L"Can't access shader!", L"Error", 0);
    }

    std::vector<char> compiledShader(size);

    fin.read(&compiledShader[0], size);
    fin.close();

    D3DX11CreateEffectFromMemory(&compiledShader[0], size,
       0, device, &effect);

    technique = effect->GetTechniqueByName("ColorTech");
    worldViewProj = effect->GetVariableByName("gWorldViewProj")->AsMatrix();
}