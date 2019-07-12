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
#include <filesystem>

#pragma warning (disable : 28251)

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nShowCmd)
{
    auto start = chrono::system_clock::now();


    DXTest dxbase(hInstance);

    if (!dxbase.Initialisation())
        return 0;

    dxbase.OnWindowResize();

    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed = end - start;

    DBOUT("Loading finished in " << elapsed.count() << " seconds");

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
    delete res; res = 0;

    DXRelease(boxVB);
    DXRelease(boxIB);
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

    /*check data structure integrity*/
    std::filesystem::path modelPath(MODEL_PATH);
    std::filesystem::path texturePath(TEXTURE_PATH);

    if (!std::filesystem::exists(modelPath) || !std::filesystem::exists(texturePath))
    {
        return false;
    }

    /*...*/

    input = new InputManager();
    res = new ResourceManager(device, deviceContext);
    
    /*load all models*/
    if (!res->AddModelsFromFolder(modelPath))
    {
        MessageBox(wndHandle, L"Failed to load model!", L"Error", MB_OK);
    }

    /*load all textures*/
    if(!res->AddTexturesFromFolder(texturePath))
    {
        MessageBox(wndHandle, L"Failed to load texture!", L"Error", MB_OK);
    }

    res->getTextureCollection()->SetDefaultTexture("default");
    res->getModelCollection()->SetDefaultModel("default");

    Shaders::Init(device);
    InputLayouts::Init(device);
    RenderStates::Init(device);


    skybox = new Skybox(device, L"data/skybox/sunsetcube1024.dds", 30.f);


    XMMATRIX I = XMMatrixIdentity();
    XMStoreFloat4x4(&boxWorld, I);

    buildCube();

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

    /*need update mouse here*/
    if (!wndInactive)
    {
        int wndCenterX, wndCenterY;
        POINT mousePos;
        RECT r;

        GetWindowRect(wndHandle, &r);
        wndCenterX = r.left + wndWidth / 2;
        wndCenterY = r.top + wndHeight / 2;


        if (GetCursorPos(&mousePos))
        {

            mousePos.x -= wndCenterX;
            mousePos.y -= wndCenterY;
        }

        SetCursorPos(wndCenterX, wndCenterY);
        input->UpdateMouse(mousePos);
    }
    else
    {
        POINT m;
        m.x = 0;
        m.y = 0;
        input->UpdateMouse(m);
    }

    input->Update(deltaTime);


    /*basic movement and camera*/

    if (controllingInput == -1)
    {
        for (int i = 0; i < INPUT_MAX; i++)
        {
            InputData* n = input->getInput(i);

            if (n->buttons[BUTTON_A])
            {
                controllingInput = i;
                break;
            }

            if (i == INPUT_MAX-1)
            {
                return;
            }
        }

    }


    InputData* in = input->getInput(controllingInput);

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

    if (in->buttons[BACK])
    {
        exit(0);
    }

}


void DXTest::Draw()
{
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    gCamera.UpdateViewMatrix();

    XMMATRIX view = gCamera.getView();
    XMMATRIX proj = gCamera.getProj();
    XMMATRIX viewProj = gCamera.getViewProj();

    XMMATRIX world = XMLoadFloat4x4(&boxWorld);
    XMMATRIX wvp = world * view * proj;

    deviceContext->IASetInputLayout(InputLayouts::PosTexNormalTan);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(Vertex::PosTexNormalTan);
    UINT offset = 0;

    /*set used technique*/
    ID3DX11EffectTechnique* currentTech = Shaders::basicTextureShader->BasicTextureTechnique;

    D3DX11_TECHNIQUE_DESC techDesc;
    currentTech->GetDesc(&techDesc);

    for (auto& m : res->getModel("Wolf")->meshes)
    {

        for (UINT p = 0; p < techDesc.Passes; ++p)
        {
            deviceContext->IASetVertexBuffers(0, 1, &m->vertex, &stride, &offset);
            deviceContext->IASetIndexBuffer(m->index, DXGI_FORMAT_R32_UINT, 0);

            Shaders::basicTextureShader->SetWorldViewProj(wvp);
            Shaders::basicTextureShader->SetTexture(res->getTexture("lol"));
            currentTech->GetPassByIndex(p)->Apply(0, deviceContext);

            deviceContext->DrawIndexed((UINT)(m->indices.size()), 0, 0);

            // 36 indices for the box.
            //res->getModel("default")->Draw(deviceContext);
            //deviceContext->DrawIndexed(36, 0, 0);
        }

    }
#ifndef _DEBUG

    /*draw*/
    deviceContext->IASetInputLayout(InputLayouts::Basic32);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;

    // Set constants
    gCamera.UpdateViewMatrix();


    XMMATRIX view = gCamera.getView();
    XMMATRIX proj = gCamera.getProj();
    XMMATRIX viewProj = gCamera.getViewProj();

    XMMATRIX world = XMLoadFloat4x4(&boxWorld);
    XMMATRIX wvp = world * view * proj;

    /*set per frame constats*/

    /*set used technique*/
    ID3DX11EffectTechnique* currentTech = Shaders::basicTextureShader->BasicTextureTechnique;

    D3DX11_TECHNIQUE_DESC techDesc;
    currentTech->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        deviceContext->IASetVertexBuffers(0, 1, &boxVB, &stride, &offset);
        deviceContext->IASetIndexBuffer(boxIB, DXGI_FORMAT_R32_UINT, 0);

        Shaders::basicTextureShader->SetWorldViewProj(wvp);
        Shaders::basicTextureShader->SetTexture(res->getTexture("lol"));
        currentTech->GetPassByIndex(p)->Apply(0, deviceContext);

        // 36 indices for the box.
        //res->getModel("default")->Draw(deviceContext);
        deviceContext->DrawIndexed(36, 0, 0);
    }
#endif // !_DEBUG

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
    Vertex::Basic32 vertices[] =
    {
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.8f,0.1f,0.71f), XMFLOAT2(0,0)   },
        { XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT3(0.2f,0.7f,0.71f),  XMFLOAT2(1,0)   },
        { XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT3(0.8f,0.1f,0.71f),  XMFLOAT2(1,1)   },
        { XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT3(0.2f,0.7f,0.71f),  XMFLOAT2(0,1)   },
        { XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT3(0.8f,0.5f,0.31f),  XMFLOAT2(1,0)   },
        { XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT3(0.2f,0.7f,0.31f),  XMFLOAT2(0,0)   },
        { XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT3(0.8f,0.5f,0.81f),  XMFLOAT2(0,1)   },
        { XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT3(0.2f,0.7f,0.71f),  XMFLOAT2(1,1)   },
    };

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::Basic32) * 8;
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