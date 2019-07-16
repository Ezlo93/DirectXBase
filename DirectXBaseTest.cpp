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

    for (auto& m : modelsStatic)
        delete m;

    modelsStatic.clear();

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
    
    /*create default cube*/
    res->getModelCollection()->AddModel("defaultCube", res->getModelCollection()->CreateCubeModel(2.f, 2.f, 2.f));
    res->getModelCollection()->AddModel("defaultSphere", res->getModelCollection()->CreateSphereModel(5.f, 64, 64));


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
    res->getModelCollection()->SetDefaultModel("defaultCube");

    Shaders::Init(device);
    InputLayouts::Init(device);
    RenderStates::Init(device);


    skybox = new Skybox(device, L"data/skybox/sunsetcube1024.dds", 30.f);

    /*add static models for testing*/
    ModelInstanceStatic *mis = new ModelInstanceStatic(device, deviceContext, res, "default");
    mis->Translation.x = 5.f;

    modelsStatic.push_back(mis);
    modelsStatic.push_back(new ModelInstanceStatic(device, deviceContext, res, "defaultSphere"));
    modelsStatic.push_back(new ModelInstanceStatic(device, deviceContext, res, "plant"));
    modelsStatic[2]->Translation.x = -8.f;


    /*test light values*/
    gDirLights[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    gDirLights[0].Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    gDirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
    gDirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

    gDirLights[1].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    gDirLights[1].Diffuse = XMFLOAT4(1.4f, 1.4f, 1.4f, 1.0f);
    gDirLights[1].Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
    gDirLights[1].Direction = XMFLOAT3(-0.307f, 0.5f, 0.707f);


    gDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    gDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    gDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    gDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

    //goFullscreen(true);

    return true;
}


void DXTest::OnWindowResize()
{
    DirectXBase::OnWindowResize();

    /*recalc camera*/
    gCamera.setLens(0.2f * XM_PI, getAspectRatio(), .01f, 1000.f);
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

    if (in->buttons[BUTTON_A])
    {
        modelsStatic[1]->Scale.x += 1.1f * deltaTime;
        modelsStatic[1]->Rotation.x = 90;
        modelsStatic[1]->Translation.z += 1.f * deltaTime;
       
    }


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

    deviceContext->RSSetState(RenderStates::noCullRS);
    deviceContext->IASetInputLayout(InputLayouts::PosTexNormalTan);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    /*keep this because not object specific*/
    Shaders::basicTextureShader->SetEyePosW(gCamera.getPosition());
    Shaders::basicTextureShader->SetDirLights(gDirLights);

    /*draw static models*/
    for (auto& m : modelsStatic)
    {
        m->Draw(&gCamera, Shaders::basicTextureShader);
    }

    //render sky box last
    skybox->Draw(deviceContext, gCamera);
    deviceContext->RSSetState(0);
    deviceContext->OMSetDepthStencilState(0, 0);


    //show backbuffer
    swapChain->Present(0, 0);
}