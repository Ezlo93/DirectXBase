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

    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed = end - start;

    DBOUT("Loading finished in " << elapsed.count() << " seconds");

    return dxbase.Run();
}


DXTest::DXTest(HINSTANCE hProgramID) : DirectXBase(hProgramID)
{
    wndTitle = L"DirectXBaseTest";

    gCamera.setPosition(0.f, 4.f, -25.f);

    /*clear color to silver*/
    clearColor[0] = 0.75f;
    clearColor[1] = 0.75f;
    clearColor[2] = 0.75f;
    clearColor[3] = 1.f;

    clearColorSec[0] = 0.0f;
    clearColorSec[1] = 0.2f;
    clearColorSec[2] = 0.2f;
    clearColorSec[3] = 1.f;

}

DXTest::~DXTest()
{
    deviceContext->ClearState();

    delete input; input = 0;
    delete skybox; skybox = 0;
    delete res; res = 0;
    delete testLevel;

    DXRelease(mScreenQuadVB);
    DXRelease(mScreenQuadIB);
    DXRelease(mOffscreenSRV);
    DXRelease(mOffscreenUAV);
    DXRelease(mOffscreenRTV);

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

    Shaders::Init(device);
    InputLayouts::Init(device);
    RenderStates::Init(device);

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

    /*create 100 unit radius sized skysphere*/
    skybox = new Skybox(device, L"data/skybox/sunsetcube1024.dds", 100.f);

    /*create shadow map*/
    shadowMap = new ShadowMap(device, SHADOW_HIGH);

    sceneBounds.Center = XMFLOAT3(0.f, 0.f, 0.f);
    sceneBounds.Radius = sqrtf(4000);

    /*add static models for testing*/
    testLevel = new Level(res);
    testLevel->LoadLevel("test.lvl");

    //ASSERT(modelsStatic.size() == 3);

    for(int i = 0; i < 4; i++)
        playCharacters.push_back(new PlayableChar("bar", res));

    playCharacters[0]->Translation.z -= PLAYER_DISTANCE;
    playCharacters[1]->Translation.z += PLAYER_DISTANCE;
    playCharacters[2]->Translation.x -= PLAYER_DISTANCE;
    playCharacters[3]->Translation.x += PLAYER_DISTANCE;

    playCharacters[2]->Rotation.z += XM_PIDIV2;
    playCharacters[3]->Rotation.z -= XM_PIDIV2;
    playCharacters[2]->Orientation = true;
    playCharacters[3]->Orientation = true;

    playCharacters[2]->hitBox.Orientation = XMFLOAT4(0.f, 0.f, 0.7071068f, 0.7071068f);
    playCharacters[3]->hitBox.Orientation = XMFLOAT4(0.f, 0.f, -0.7071068f, 0.7071068f);

    playball = new Ball("defaultSphere", res, playCharacters);

    /*test light values*/
    gDirLights.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    gDirLights.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    gDirLights.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
    gDirLights.Direction = XMFLOAT3(-.57735f, -0.57735f, .57735f);

    originalLightDir = gDirLights.Direction;

    BuildScreenQuadGeometryBuffers();

    OnWindowResize();
    goFullscreen(true);

    return true;
}


void DXTest::OnWindowResize()
{

    DirectXBase::OnWindowResize();

    BuildOffscreenViews();
    blurEffect.Init(device, wndWidth, wndHeight, DXGI_FORMAT_R8G8B8A8_UNORM);


    /*recalc camera*/
    gCamera.setLens(0.2f * XM_PI, getAspectRatio(), .01f, 1000.f);

    for (auto& i : playCharacters)
    {
        i->getCamera()->setLens(0.2f * XM_PI, getAspectRatio(), .01f, 1000.f);
    }
}

bool DXTest::goFullscreen(bool s)
{
    if (!DirectXBase::goFullscreen(s))
    {
        return false;
    }

    OnWindowResize();

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

        playCharacters[0]->Translation.x += tlX * playCharacters[0]->Speed * deltaTime;

        if (playCharacters[0]->Translation.x <= -25.f)
        {
            playCharacters[0]->Translation.x = -25.f;
        }
        else if(playCharacters[0]->Translation.x >= 25.f)
        {
            playCharacters[0]->Translation.x = 25.f;
        }

        float yaw = 1.5f * deltaTime * trX;
        float pitch = 1.5f * deltaTime * trY;

        gCamera.yaw(yaw);
        gCamera.pitch(pitch);


        if (input->ButtonPressed(controllingInput, START))
        {
            renderWireFrame = !renderWireFrame;
        }

        if (input->ButtonReleased(controllingInput, BUTTON_X))
        {
            blurStrength++;
        }

        if (input->ButtonReleased(controllingInput, BUTTON_Y))
        {
            if(blurStrength > 0) blurStrength--;
        }

        if (input->ButtonReleased(controllingInput, BACK))
        {
            exit(0);
        }
    }


    //ball
    playball->Update(deltaTime);

    for (auto& i : playCharacters)
    {
        if (i->Orientation)
        {
            i->Translation.z = playball->Translation.z;
        }
        else
        {
            i->Translation.x = playball->Translation.x;
        }
        
        i->Update(deltaTime);
    }

    /*rotate light*/
    lightRotationAngle += 0.1f * deltaTime;
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
    Camera* activeCamera = &gCamera;
    //activeCamera = playCharacters[0]->getCamera();
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
        it->second->ShadowDraw(device, deviceContext, activeCamera, lview, lproj);
        it++;
    }

    playball->ShadowDraw(device, deviceContext, activeCamera, lview, lproj);

    for (auto& i : playCharacters)
    {
        i->ShadowDraw(device, deviceContext, activeCamera, lview, lproj);
    }

    deviceContext->RSSetState(0);
    /*end of shadow map*/


    /*reset to offscreen texture rendertarget*/
    /*clear buffers*/
    
    //ID3D11RenderTargetView* renderTargets[1] = { renderTargetView };
    ID3D11RenderTargetView* renderTargets[1] = { mOffscreenRTV };
    deviceContext->OMSetRenderTargets(1, renderTargets, depthStencilView);
    deviceContext->RSSetViewports(1, &mainViewport);

    //deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    deviceContext->ClearRenderTargetView(mOffscreenRTV, clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


    if (!renderWireFrame)
    {
        deviceContext->RSSetState(0);
    }
    else
    {
        deviceContext->RSSetState(RenderStates::wireFrame);
    }
    
    /*set shader constants that are not object dependant*/
    /*basic shader*/
    Shaders::basicTextureShader->SetEyePosW(activeCamera->getPosition());
    Shaders::basicTextureShader->SetDirLights(gDirLights);

    Shaders::basicTextureShader->SetShadowMap(shadowMap->DepthMapSRV());

    /*normal shader*/
    Shaders::normalMapShader->SetEyePosW(activeCamera->getPosition());
    Shaders::normalMapShader->SetDirLights(gDirLights);

    Shaders::normalMapShader->SetShadowMap(shadowMap->DepthMapSRV());


    /*draw static models*/
    XMMATRIX st = XMLoadFloat4x4(&shadowTransform);

    it = testLevel->modelsStatic.begin();
    while(it != testLevel->modelsStatic.end()){
         it->second->Draw(device, deviceContext, activeCamera, st);
        it++;
    }

    //draw ball
    playball->Draw(device, deviceContext, activeCamera, st);

    //play characters
    for (auto& i : playCharacters)
    {
        i->Draw(device, deviceContext, activeCamera, st);
    }

    //render sky box last
    skybox->Draw(deviceContext, *activeCamera);


    /*blur*/

    renderTargets[0] = renderTargetView;
    deviceContext->OMSetRenderTargets(1, renderTargets, depthStencilView);

    if (blurStrength > 0)
    {
        blurEffect.BlurSRV(deviceContext, mOffscreenSRV, mOffscreenUAV, blurStrength);
    }
    


    DrawScreenQuad(mOffscreenSRV);


    /*default*/
    deviceContext->RSSetState(0);
    deviceContext->OMSetDepthStencilState(0, 0);
    ID3D11ShaderResourceView* nullSRV[16] = { 0 };
    deviceContext->PSSetShaderResources(0, 16, nullSRV);


    //show backbuffer
                     //this value is vsync => 0 is off, 1 - 4 sync intervalls
    swapChain->Present(0, 0);
}



void DXTest::BuildScreenQuadGeometryBuffers()
{

    //
    // Extract the vertex elements we are interested in and pack the
    // vertices of all the meshes into one vertex buffer.
    //

    std::vector<Vertex::Standard> Vertices(4);
    std::vector<UINT> indices(6);

    // Position coordinates specified in NDC space.
    Vertices[0] = Vertex::Standard(
        -1.0f, -1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f
        );

    Vertices[1] = Vertex::Standard(
        -1.0f, +1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f
        );

    Vertices[2] = Vertex::Standard(
        +1.0f, +1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f
        );

    Vertices[3] = Vertex::Standard(
        +1.0f, -1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f
       );

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = (UINT)( sizeof(Vertex::Standard) * Vertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &Vertices[0];
    device->CreateBuffer(&vbd, &vinitData, &mScreenQuadVB);

    //
    // Pack the indices of all the meshes into one index buffer.
    //

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = (UINT)(sizeof(Vertex::Standard) * indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    device->CreateBuffer(&ibd, &iinitData, &mScreenQuadIB);
}


void DXTest::DrawScreenQuad(ID3D11ShaderResourceView* srv)
{

    UINT stride = sizeof(Vertex::Standard);
    UINT offset = 0;

    deviceContext->IASetInputLayout(InputLayouts::Standard);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->IASetVertexBuffers(0, 1, &mScreenQuadVB, &stride, &offset);
    deviceContext->IASetIndexBuffer(mScreenQuadIB, DXGI_FORMAT_R32_UINT, 0);

    // Scale and shift quad to lower-right corner.
    XMMATRIX world = XMMatrixIdentity();

    ID3DX11EffectTechnique* tech = Shaders::DebugTexFX->ViewStandard;
    D3DX11_TECHNIQUE_DESC techDesc;

    tech->GetDesc(&techDesc);
    for (UINT p = 0; p < techDesc.Passes; ++p)
    {
        Shaders::DebugTexFX->SetWorldViewProj(world);
        Shaders::DebugTexFX->SetTexture(srv);

        tech->GetPassByIndex(p)->Apply(0, deviceContext);
        deviceContext->DrawIndexed(6, 0, 0);
    }

}


void DXTest::BuildOffscreenViews()
{
    // We call this function everytime the window is resized so that the render target is a quarter
    // the client area dimensions.  So Release the previous views before we create new ones.
    DXRelease(mOffscreenSRV);
    DXRelease(mOffscreenRTV);
    DXRelease(mOffscreenUAV);

    D3D11_TEXTURE2D_DESC texDesc;

    texDesc.Width = wndWidth;
    texDesc.Height = wndHeight;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    ID3D11Texture2D* offscreenTex = 0;
    device->CreateTexture2D(&texDesc, 0, &offscreenTex);

    // Null description means to create a view to all mipmap levels using 
    // the format the texture was created with.
    device->CreateShaderResourceView(offscreenTex, 0, &mOffscreenSRV);
    device->CreateRenderTargetView(offscreenTex, 0, &mOffscreenRTV);
    device->CreateUnorderedAccessView(offscreenTex, 0, &mOffscreenUAV);

    // View saves a reference to the texture so we can release our reference.
    DXRelease(offscreenTex);
}