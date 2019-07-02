/*DirectXBaseTest.cpp 
    minimum template for directx11 application

    extends DirectXBase class

    Nicolai Sehrt
*/

#include "DirectXBaseTest.h"
#include "RenderStates.h"
#include "Shader.h"
#include "InputLayout.h"
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

    dxbase.OnWindowResize();

    return dxbase.Run();
}


DXTest::DXTest(HINSTANCE hProgramID) : DirectXBase(hProgramID)
{
    wndTitle = L"DirectXBaseTest";

    gCamera.setPosition(0.f, 3.f, -20.f);

    /*clear color to silver*/
    clearColor[0] = 0.75f;
    clearColor[1] = 0.75f;
    clearColor[2] = 0.75f;
    clearColor[3] = 1.f;

}

DXTest::~DXTest()
{

    delete input; input = 0;
    delete skybox; skybox = 0;

    DXRelease(boxVB);
    DXRelease(boxIB);
    DXRelease(inputLayout);
    DXRelease(effect);

    RenderStates::Destroy();
    Shaders::Destroy();
    InputLayouts::Destroy();

}

bool DXTest::Initialisation()
{
    if (!DirectXBase::Initialisation())
    {
        return false;
    }

    /*...*/

    input = new InputManager();

    Shaders::Init(device);
    InputLayouts::Init(device);
    RenderStates::Init(device);


    skybox = new Skybox(device, L"data/skybox/sunsetcube1024.dds", 30.f);


    XMMATRIX I = XMMatrixIdentity();
    XMStoreFloat4x4(&boxWorld, I);

    buildCube();
    buildShader();
    buildLayout();

    //goFullscreen(true);

    return true;
}


void DXTest::OnWindowResize()
{
    DirectXBase::OnWindowResize();

    /*recalc camera*/
    gCamera.setLens(0.25f * XM_PI, getAspectRatio(), 1.f, 1000.f);
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

    input->Update(deltaTime);


    /*basic movement and camera*/
    InputData* in = input->getInput(0);

    float tlX = in->trigger[THUMB_LX];
    float tlY = in->trigger[THUMB_LY];

    float trX = in->trigger[THUMB_RX];
    float trY = in->trigger[THUMB_RY] * -1;

    float ws = tlY * 10.f * deltaTime;
    float ss = tlX * 10.f * deltaTime;

    gCamera.walk(ws);
    gCamera.strafe(ss);

    float yaw = 1.5f * deltaTime * trX;
    float pitch = 1.5f * deltaTime * trY;

    gCamera.yaw(yaw);
    gCamera.pitch(pitch);

}


void DXTest::Draw()
{
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    /*draw*/
    deviceContext->IASetInputLayout(inputLayout);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(Vertex::BasicColor);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &boxVB, &stride, &offset);
    deviceContext->IASetIndexBuffer(boxIB, DXGI_FORMAT_R32_UINT, 0);

    // Set constants
    gCamera.UpdateViewMatrix();


    XMMATRIX view = gCamera.getView();
    XMMATRIX proj = gCamera.getProj();
    XMMATRIX viewProj = gCamera.getViewProj();

    XMMATRIX world = XMLoadFloat4x4(&boxWorld);
    XMMATRIX wvp = world * view * proj;

    worldViewProj->SetMatrix(reinterpret_cast<float*>(&wvp));

    D3DX11_TECHNIQUE_DESC techDesc;
    technique->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        technique->GetPassByIndex(p)->Apply(0, deviceContext);

        // 36 indices for the box.
        deviceContext->DrawIndexed(36, 0, 0);
    }


    //render sky box last
    skybox->Draw(deviceContext, gCamera);
    deviceContext->RSSetState(0);
    deviceContext->OMSetDepthStencilState(0, 0);


    //show backbuffer
    swapChain->Present(0, 0);
}


void DXTest::buildCube()
{
    // Create vertex buffer
    Vertex::BasicColor vertices[] =
    {
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.8f,0.1f,0.71f,1.f)   },
        { XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(0.2f,0.7f,0.71f,1.f)   },
        { XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(0.8f,0.1f,0.71f,1.f)   },
        { XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(0.2f,0.7f,0.71f,1.f)   },
        { XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(0.8f,0.5f,0.31f,1.f)   },
        { XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(0.2f,0.7f,0.31f,1.f)   },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(0.8f,0.5f,0.81f,1.f)   },
        { XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(0.2f,0.7f,0.71f,1.f)   },
    };

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::BasicColor) * 8;
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

    std::ifstream fin(L"data/shader/color.fxo", std::ios::binary);

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