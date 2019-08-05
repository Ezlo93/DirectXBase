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
#pragma warning (disable : 6387)

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

    gCamera.setPosition(0.f, 2.f, -15.f);

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
    delete testLevel;

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
    res->getModelCollection()->AddModel(DEFAULT_PLANE, res->getModelCollection()->CreatePlaneModel(1.f, 1.f));
    res->getModelCollection()->AddModel(DEFAULT_CUBE, res->getModelCollection()->CreateCubeModel(1.f, 1.f, 1.f));
    res->getModelCollection()->AddModel(DEFAULT_SPHERE, res->getModelCollection()->CreateSphereModel(.5f, 32, 32));


    /*load all models*/
    if (!res->AddModelsFromFolder(modelPath))
    {
        MessageBox(wndHandle, L"Failed to load model!", L"Error", MB_OK);
    }
    /*load all textures*/

    if (!res->AddTexturesFromFolder(texturePath))
    {
        MessageBox(wndHandle, L"Failed to load texture!", L"Error", MB_OK);
    }

    res->getTextureCollection()->SetDefaultTexture("default");
    res->getModelCollection()->SetDefaultModel("defaultCube");

    Shaders::Init(device);
    InputLayouts::Init(device);
    RenderStates::Init(device);


    /*create 100 unit radius sized skysphere*/
    skybox = new Skybox(device, L"data/skybox/sunsetcube1024.dds", 100.f);

    /*create shadow map*/
    shadowMap = new ShadowMap(device, 2048, 2048);

    sceneBounds.Center = XMFLOAT3(0.f, 0.f, 0.f);
    sceneBounds.Radius = sqrtf(200);

    /*add static models for testing*/
    testLevel = new Level(res);
    testLevel->LoadLevel("test.lvl");

    //ASSERT(modelsStatic.size() == 3);




    /*test light values*/
    gDirLights.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    gDirLights.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    gDirLights.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
    gDirLights.Direction = XMFLOAT3(-.57735f, -0.57735f, .57735f);

    originalLightDir = gDirLights.Direction;

    /*lighting configuration to check it works*/
    //gDirLights.Ambient = XMFLOAT4(0.f, 0.3f, 0.f, 1.0f);
    //gDirLights.Diffuse = XMFLOAT4(0.f, 0.f, 0.8f, 1.0f);
    //gDirLights.Specular = XMFLOAT4(0.6f, 0.f, 0.f, 16.0f);
    //gDirLights.Direction = XMFLOAT3(.57735f, -0.57735f, .57735f);

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


void DXTest::buildShadowTransform()
{
    // Only the first "main" light casts a shadow.
    XMVECTOR lightDir = XMLoadFloat3(&gDirLights.Direction);
    XMVECTOR lightPos = -2.0f * sceneBounds.Radius * lightDir;
    XMVECTOR targetPos = XMLoadFloat3(&sceneBounds.Center);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

    // Transform bounding sphere to light space.
    XMFLOAT3 sphereCenterLS;
    XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

    // Ortho frustum in light space encloses scene.
    float l = sphereCenterLS.x - sceneBounds.Radius;
    float b = sphereCenterLS.y - sceneBounds.Radius;
    float n = sphereCenterLS.z - sceneBounds.Radius;
    float r = sphereCenterLS.x + sceneBounds.Radius;
    float t = sphereCenterLS.y + sceneBounds.Radius;
    float f = sphereCenterLS.z + sceneBounds.Radius;
    XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

    // Transform NDC space [-1,+1]^2 to texture space [0,1]^2
    XMMATRIX T(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f);

    XMMATRIX S = V * P * T;

    XMStoreFloat4x4(&lightView, V);
    XMStoreFloat4x4(&lightProj, P);
    XMStoreFloat4x4(&shadowTransform, S);

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

    /*wait for input device*/
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

            //if (i == INPUT_MAX-1)
            //{
            //    gCamera.UpdateViewMatrix();
            //    return;
            //}
        }

    }

    if (controllingInput != -1)
    {
        //handle input
        InputData* in = input->getInput(controllingInput);
        InputData* prevIn = input->getPrevInput(controllingInput);

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

        /*
        if (in->buttons[BUTTON_A])
        {
            testLevel->modelsStatic[10011]->Translation.x += 2 * deltaTime;
        }

        if (in->buttons[BUTTON_X])
        {
            testLevel->modelsStatic[10011]->Translation.y += 2 * deltaTime;
        }

        if (in->buttons[BUTTON_Y])
        {
            testLevel->modelsStatic[10011]->Translation.z += 2 * deltaTime;
        }
        */


        if (input->ButtonPressed(controllingInput, START))
        {
            renderWireFrame = !renderWireFrame;
        }

        if (input->ButtonReleased(controllingInput, BACK))
        {
            exit(0);
        }
    }


    /*rotate light*/
    lightRotationAngle += 0.25f * deltaTime;
    XMMATRIX R = XMMatrixRotationY(lightRotationAngle);
    XMVECTOR lDir = XMLoadFloat3(&originalLightDir);
    lDir = XMVector3TransformNormal(lDir, R);
    XMStoreFloat3(&gDirLights.Direction, lDir);

    /*build shadow transform*/
    buildShadowTransform();

    /*update camera position*/
    gCamera.UpdateViewMatrix();

}


void DXTest::Draw()
{

    /*draw to shadow map*/
    shadowMap->BindDsvAndSetNullRenderTarget(deviceContext);

    XMMATRIX lview = XMLoadFloat4x4(&lightView);
    XMMATRIX lproj = XMLoadFloat4x4(&lightProj);

    /*draw shadow of static models*/
    deviceContext->IASetInputLayout(InputLayouts::Standard);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    std::map<int, ModelInstanceStatic*>::iterator it = testLevel->modelsStatic.begin();
    while (it != testLevel->modelsStatic.end())
    {
        it->second->ShadowDraw(device, deviceContext, &gCamera, lview, lproj);
        it++;
    }


    /*reset to normal rendertarget*/
    deviceContext->RSSetState(0);

    ID3D11RenderTargetView* renderTargets[1] = { renderTargetView };
    deviceContext->OMSetRenderTargets(1, renderTargets, depthStencilView);
    deviceContext->RSSetViewports(1, &mainViewport);

    /*clear buffers*/
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


    if (!renderWireFrame)
    {
        deviceContext->RSSetState(0);
    }
    else
    {
        deviceContext->RSSetState(RenderStates::wireFrame);
    }
    
    deviceContext->IASetInputLayout(InputLayouts::Standard);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    /*set shader constants that are not object dependant*/
    /*basic shader*/
    Shaders::basicTextureShader->SetEyePosW(gCamera.getPosition());
    Shaders::basicTextureShader->SetDirLights(gDirLights);

    Shaders::basicTextureShader->SetShadowMap(shadowMap->DepthMapSRV());

    /*normal shader*/
    Shaders::normalMapShader->SetEyePosW(gCamera.getPosition());
    Shaders::normalMapShader->SetDirLights(gDirLights);

    Shaders::normalMapShader->SetShadowMap(shadowMap->DepthMapSRV());


    /*draw static models*/
    XMMATRIX st = XMLoadFloat4x4(&shadowTransform);

    it = testLevel->modelsStatic.begin();
    while(it != testLevel->modelsStatic.end()){
         it->second->Draw(device, deviceContext, &gCamera, st);
        it++;
    }

    //render sky box last
    skybox->Draw(deviceContext, gCamera);
    deviceContext->RSSetState(0);
    deviceContext->OMSetDepthStencilState(0, 0);


    //show backbuffer
                     //this value is vsync => 0 is off, 1 - 4 sync intervalls
    swapChain->Present(0, 0);
}