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

    DBOUT("Loading finished in " << elapsed.count() << " seconds" << std::endl);

    return dxbase.Run();
}


DXTest::DXTest(HINSTANCE hProgramID) : DirectXBase(hProgramID)
{
    wndTitle = L"Pong3D";

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
    delete gameLevel; delete endLevel;

    for (auto& i : playCharacters)
        delete i;
    for (auto& i : players)
        delete i;

    DXRelease(mScreenQuadVB);
    DXRelease(mScreenQuadIB);
    DXRelease(mOffscreenSRV);
    DXRelease(mOffscreenUAV);
    DXRelease(mOffscreenRTV);

    RenderStates::Destroy();
    Shaders::Destroy();
    InputLayouts::Destroy();

    DirectXBase::~DirectXBase();
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
    res = new ResourceManager(device, deviceContext, WICFactory, d2dContext);

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

    /*load all bitmaps*/
    for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::path(BITMAP_PATH)))
    {
        DBOUT(L"Loading sprite " << entry.path().c_str() << std::endl);

        std::wstring wt = entry.path().wstring();
        LPWSTR t = &wt[0];
        res->getBitmap()->loadBitmap(t);
    }

    /*brushes*/
    d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(0.f, 0.f, 0.f, 1.0f)),
                                      &fadeBrush);



    /*ui bitmaps*/

    /*title screen*/
    bTitle.setup(L"title", relativePos(0.1f, 0.1f, 0.85f, 0.65f, wndWidth, wndHeight),0.9f);
    bPressA.setup(L"pressa", relativePos(0.35f, 0.7f, 0.65f, 0.75f, wndWidth, wndHeight), 0.9f);
    bPressStart.setup(L"pressstart", relativePos(0.35f, 0.8f, 0.65f, 0.85f, wndWidth, wndHeight), 0.9f);

    bPressA.setAnim(3.f, relativePos(0.35f, 0.7f, 0.65f, 0.75f, wndWidth, wndHeight), relativePos(0.33f, 0.68f, 0.67f, 0.77f, wndWidth, wndHeight));
    bPressStart.setAnim(3.f, relativePos(0.35f, 0.8f, 0.65f, 0.85f, wndWidth, wndHeight), relativePos(0.33f, 0.78f, 0.67f, 0.87f, wndWidth, wndHeight));

    /*ingame*/
    uiBase[0] = { 0.02f, 0.4f, 0.05f, 0.f };
    uiBase[1] = { 0.52f, 0.4f, 0.55f, 0.f };
    uiBase[2] = { 0.02f, 0.9f, 0.05f, 0.f };
    uiBase[3] = { 0.52f, 0.9f, 0.55f, 0.f };

    /*pause*/
    bPause.setup(L"pause", relativePos(0.35f, 0.2f, 0.65f, 0.4f, wndWidth, wndHeight), 0.8f);

    /*load all sounds*/
    for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::path(SOUND_PATH_MUSIC)))
    {
        DBOUT(L"Loading sound " << entry.path().c_str() << std::endl);
        res->getSound()->loadFile(entry.path().c_str(), SoundType::Music);
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(std::filesystem::path(SOUND_PATH_EFFECTS)))
    {
        DBOUT(L"Loading sound " << entry.path().c_str() << std::endl);
        res->getSound()->loadFile(entry.path().c_str(), SoundType::Effect);
    }

    /*create 100 unit radius sized skysphere*/
    skybox = new Skybox(device, L"data/skybox/grasscube1024.dds", 100.f);

    /*create shadow map*/
    shadowMap = new ShadowMap(device, SHADOW_HIGH);

    sceneBounds.Center = XMFLOAT3(0.f, 0.f, 0.f);
    sceneBounds.Radius = sqrtf(4000);

    /*add static models for testing*/
    gameLevel = new Level(res, device, deviceContext);
    gameLevel->LoadLevel("game.lvl");

    endLevel = new Level(res, device, deviceContext);
    endLevel->LoadLevel("end.lvl");

    activeLevel = gameLevel;

    /*particle system*/

    std::vector<std::wstring> raindrop;
    raindrop.push_back(L"data/textures/raindrop.dds");
    ID3D11ShaderResourceView* mDrop = CreateTexture2DArraySRV(device, deviceContext, raindrop);

    mRain.init(device, Shaders::rainShader, mDrop, CreateRandomTexture1DSRV(device), 10000);
    mRain.setAcceleration(XMFLOAT3(-1.f, -9.8f, 0.f));
    mRain.setSizeParticle(XMFLOAT2(12.5f, 12.5f));

    /*player character and ball*/

    playerColors[0] = XMFLOAT4(0.3f, 0.55f, 1.f, 1.0f);
    playerColors[1] = XMFLOAT4(1.f, 0.2f, 0.25f, 1.0f);
    playerColors[2] = XMFLOAT4(1.f, 0.8f, 0.22f, 1.0f);
    playerColors[3] = XMFLOAT4(0.f, 0.5f, 0.2f, 1.0f);

    for (int i = 0; i < 4; i++)
    {
        playCharacters.push_back(new PlayableChar("bar", res));
        playCharacters[i]->metaPosition = i;
    }
    PLAYER_MAX_MOVEMENT = PLAYER_DISTANCE - playCharacters[0]->hitBox.Extents.x - playCharacters[0]->hitBox.Extents.y;
    playball = new Ball("defaultSphere", res, playCharacters);

    clearData();

    introCamera.setPosition(10.f, 15.f, 10.f);

    introCamera.setLens(0.2f * XM_PI, getAspectRatio(), .01f, 1000.f);
    endScreenCamera.setLens(0.2f * XM_PI, getAspectRatio(), .01f, 1000.f);

    activeCamera = &introCamera;

    /*test light values*/
    gDirLights.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    gDirLights.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    gDirLights.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
    gDirLights.Direction = XMFLOAT3(-.57735f, -0.57735f, .57735f);

    originalLightDir = gDirLights.Direction;

    BuildScreenQuadGeometryBuffers();

    OnWindowResize();
    themeChannel = res->getSound()->add("theme", true);

    return true;
}


void DXTest::OnWindowResize()
{

    DirectXBase::OnWindowResize();

    BuildOffscreenViews();
    blurEffect.Init(device, wndWidth, wndHeight, DXGI_FORMAT_R8G8B8A8_UNORM);


    /*recalc camera*/

    activeCamera->setLens(0.2f * XM_PI, getAspectRatio(), .01f, 1000.f);

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
    DirectX::XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

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

void DXTest::switchLevel(Level& lvl)
{
    if (activeLevel == &lvl) return;
    lvl.Reset();
    activeLevel = &lvl;
}

bool DXTest::UpdateTransition(float deltaTime)
{

    transitionTimer += deltaTime;

    if (transitionTimer >= TRANSITION_TIME)
    {
        transitionTimer = 0;
        fadeValue = transitionInProgress == 1 ? 1.0f : 0.f;
        transitionInProgress = 0;
        return false;
    }

    fadeValue = transitionInProgress == 1 ? 1.f * (transitionTimer / TRANSITION_TIME) : 1 - 1.f * (transitionTimer / TRANSITION_TIME);

    return true;
}


void DXTest::Update(float deltaTime)
{

    /*game logic*/

    ///*dont update if window inactive*/
    //if (wndInactive)
    //{
    //    return;
    //}

    if (transitionInProgress > 0)
    {
        UpdateTransition(deltaTime);
    }

    prevGameState = gameState;

    /*update input independent of gamestate*/
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

    //SetCursorPos(wndCenterX, wndCenterY);

    /*but dont update when in transition*/
    if (transitionInProgress == 0)
    {
        input->UpdateMouse(mousePos);
        input->Update(deltaTime);
    }

    /*exit always possible*/
    if (input->ButtonReleased(0, BACK))
    {
        exit(0);
    }

    /*update depending on gamestate*/

    /*player registration*/

    if (gameState == MainGameState::PLAYER_REGISTRATION)
    {
        if (transToIngame && transitionInProgress == 0)
        {
            gameState = MainGameState::INGAME;
            regState = RegistrationState::BLANK;
            transitionInProgress = 2;
            transToIngame = 0;
            res->getSound()->forceStop(themeChannel);
        }

        if (input->ButtonPressed(0, BUTTON_X))
        {
            goFullscreen(!isFullscreen);
        }

        input->usedInputActive = false;
        activeCamera = &introCamera;
        switchLevel(*gameLevel);

        /*set position of introcamera*/

        introCameraTime += deltaTime;

        XMFLOAT3 introPos;
        introPos.y = INTROCAMERA_HEIGHT;
        introPos.x = INTROCAMERA_RADIUS * cos(2 * XM_PI * introCameraTime * INTROCAMERA_SPEED);
        introPos.z = INTROCAMERA_RADIUS * sin(2 * XM_PI * introCameraTime * INTROCAMERA_SPEED);

        introCamera.lookAt(introPos, XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(0.f, 1.0f, 0.f));

        /*first player press start to continue*/

        if (playerCount > 0)
        {

            if (input->ButtonPressed(players[0]->getInput(), START))
            {
                blurStrength = 0;
                input->usedInputActive = true;
                transitionInProgress = 1;
                transToIngame = true;
            }

        }

        /*debug*/
        if (input->ButtonPressed(0, BUTTON_Y))
        {
            renderWireFrame = !renderWireFrame;
        }

        /*up to 4 players can join*/

        for (int i = 0; i < INPUT_MAX; i++)
        {
            if (playerCount == 3)
            {
                break;
            }

            /*skip already registered inputs*/
            for (auto& p : players)
            {
                if (p->getInput() == i)
                {
                    if (playCharacters[p->getCharacter()]->Velocity.y == 0.f && input->ButtonPressed(i, BUTTON_A))
                    {
                        playCharacters[p->getCharacter()]->Velocity.y = 8.f;
                    }
                    goto cnt;
                }
            }

            if (input->ButtonPressed(i, BUTTON_A))
            {
                Player* p = new Player();
                p->AssignCharacter(playerCount++);
                p->AssignInput(i);
                p->AssignColor(playerColors[playerCount - 1]);
                p->pID = playerCount;
                playCharacters[p->getCharacter()]->Color = p->getColor();
                playCharacters[p->getCharacter()]->npc = false;
                players.push_back(p);

                playCharacters[p->getCharacter()]->Velocity.y = 8.f;
                playCharacters[p->getCharacter()]->controllingPlayer = p;

                input->addUsedInput(i);
                DBOUT("Player " << playerCount << " registered to input " << i << std::endl);

                if (playerCount > 3)
                {
                    regState = RegistrationState::FULL;
                }
                else
                {
                    regState = RegistrationState::JOINED;
                }

                break;
            }

        cnt:;
        }

        for (auto& p : playCharacters)
        {
            p->Update(deltaTime);
        }

    }
    else if (gameState == MainGameState::INGAME)
    {

        if (transToEndScreen && transitionInProgress == 0)
        {
            setupEndScreen();
            gameState = MainGameState::END_SCREEN;
            transitionInProgress = 2;
            lightRotationAngle = XM_PIDIV4;
            transToEndScreen = false;
            res->getSound()->add("cheer_long");
        }
        else if (!allDead)
        {

            /*pause screen*/
            if (input->ButtonPressed(players.front()->getInput(), START)){

                if (ingameState == InGameState::PAUSE && pauseFadeTimer >= PAUSE_FADE_TIME)
                {
                    ingameState = InGameState::PLAY;
                }
                else  if (ingameState == InGameState::PLAY)
                {
                    ingameState = InGameState::PAUSE;
                    pauseFadeTimer = 0.f;
                }

            }

            if (ingameState == InGameState::PAUSE)
            {
                pauseFadeTimer += deltaTime;
                if (pauseFadeTimer > PAUSE_FADE_TIME)
                {
                    pauseFadeTimer = PAUSE_FADE_TIME;
                }

                fadeValue = 0.6f * (pauseFadeTimer / PAUSE_FADE_TIME);

                return;
            }
            else
            {

                pauseFadeTimer -= deltaTime;
                if (pauseFadeTimer < 0)
                {
                    pauseFadeTimer = 0;
                }

                fadeValue = 0.6f * (pauseFadeTimer / PAUSE_FADE_TIME);
            }

            

            /*********/

            activeCamera = playCharacters[players.front()->getCharacter()]->getCamera();

            /*update players and then the ball*/

            /*player input*/
            for (auto& i : players)
            {

                int playerCharID = i->getCharacter();
                int inputID = i->getInput();

                InputData* in = input->getInput(inputID);

                float leftJoystickX = in->trigger[THUMB_LX];
                float leftJoystickY = in->trigger[THUMB_RX];

                if (input->ButtonPressed(inputID, LEFT_SHOULDER) || input->ButtonPressed(inputID, RIGHT_SHOULDER))
                {
                    if (playCharacters[playerCharID]->currState != PCState::DASH)
                    {
                        playCharacters[playerCharID]->dashDirection = input->ButtonPressed(inputID, LEFT_SHOULDER) ? -1 : 1;
                    }
                    playCharacters[playerCharID]->initDash();
                }

                if (playCharacters[playerCharID]->currState != PCState::DASH)
                {

                    if (playCharacters[playerCharID]->metaPosition == 0)
                    {
                        playCharacters[playerCharID]->Translation.x += leftJoystickX * playCharacters[playerCharID]->Speed * deltaTime;
                    }
                    else if (playCharacters[playerCharID]->metaPosition == 1)
                    {
                        playCharacters[playerCharID]->Translation.x -= leftJoystickX * playCharacters[playerCharID]->Speed * deltaTime;
                    }
                    else if (playCharacters[playerCharID]->metaPosition == 2)
                    {
                        playCharacters[playerCharID]->Translation.z -= leftJoystickX * playCharacters[playerCharID]->Speed * deltaTime;
                    }
                    else if (playCharacters[playerCharID]->metaPosition == 3)
                    {
                        playCharacters[playerCharID]->Translation.z += leftJoystickX * playCharacters[playerCharID]->Speed * deltaTime;
                    }

                }
                else
                {

                    float dashSpeed = (PLAYER_SPEED * 3.5f) * ((DASH_DURATION - playCharacters[playerCharID]->dashTimer) / DASH_DURATION);
                    playCharacters[playerCharID]->Speed = dashSpeed;

                    if (dashSpeed < PLAYER_SPEED)
                    {
                        dashSpeed = PLAYER_SPEED;
                    }


                    if (playCharacters[playerCharID]->metaPosition == 0)
                    {
                        playCharacters[playerCharID]->Translation.x += playCharacters[playerCharID]->dashDirection * dashSpeed * deltaTime;
                    }
                    else if (playCharacters[playerCharID]->metaPosition == 1)
                    {
                        playCharacters[playerCharID]->Translation.x -= playCharacters[playerCharID]->dashDirection * dashSpeed * deltaTime;
                    }
                    else if (playCharacters[playerCharID]->metaPosition == 2)
                    {
                        playCharacters[playerCharID]->Translation.z -= playCharacters[playerCharID]->dashDirection * dashSpeed * deltaTime;
                    }
                    else if (playCharacters[playerCharID]->metaPosition == 3)
                    {
                        playCharacters[playerCharID]->Translation.z += playCharacters[playerCharID]->dashDirection * dashSpeed * deltaTime;
                    }


                }

            }

            /*bot*/
            for (auto& i : playCharacters)
            {

                if (i->npc)
                {
                    if (i->Orientation)
                    {
                        i->Translation.z = playball->Translation.z;
                    }
                    else
                    {
                        i->Translation.x = playball->Translation.x;
                    }
                }

            }

            /*limit movement of all playable chars*/
            for (auto& i : playCharacters)
            {
                if (i->metaPosition == 0)
                {
                    if (i->Translation.x <= -PLAYER_MAX_MOVEMENT)
                    {
                        i->Translation.x = -PLAYER_MAX_MOVEMENT;
                    }
                    else if (i->Translation.x >= PLAYER_MAX_MOVEMENT)
                    {
                        i->Translation.x = PLAYER_MAX_MOVEMENT;
                    }
                }
                else if (i->metaPosition == 1)
                {
                    if (i->Translation.x <= -PLAYER_MAX_MOVEMENT)
                    {
                        i->Translation.x = -PLAYER_MAX_MOVEMENT;
                    }
                    else if (i->Translation.x >= PLAYER_MAX_MOVEMENT)
                    {
                        i->Translation.x = PLAYER_MAX_MOVEMENT;
                    }
                }
                else if (i->metaPosition == 2)
                {
                    if (i->Translation.z <= -PLAYER_MAX_MOVEMENT)
                    {
                        i->Translation.z = -PLAYER_MAX_MOVEMENT;
                    }
                    else if (i->Translation.z >= PLAYER_MAX_MOVEMENT)
                    {
                        i->Translation.z = PLAYER_MAX_MOVEMENT;
                    }
                }
                else
                {
                    if (i->Translation.z <= -PLAYER_MAX_MOVEMENT)
                    {
                        i->Translation.z = -PLAYER_MAX_MOVEMENT;
                    }
                    else if (i->Translation.z >= PLAYER_MAX_MOVEMENT)
                    {
                        i->Translation.z = PLAYER_MAX_MOVEMENT;
                    }
                }

                i->Update(deltaTime);
            }


            /*ball*/
            playball->Update(deltaTime);

            /*check if player dead*/
            allDead = true;
            int pAlive = 0;
            for (auto& i : playCharacters)
            {
                if (i->controllingPlayer == nullptr)
                {
                    continue;
                }

                if (i->controllingPlayer->hp <= 0)
                {
                    DBOUT("Player " << i->controllingPlayer->pID << " has died!\n");
                    i->npc = true;
                    winOrder.push_back(i->controllingPlayer);
                    DBOUT("Player " << i->controllingPlayer->pID << " added to win order\n");
                    i->controllingPlayer = nullptr;
                }
                else
                {
                    pAlive++;
                    allDead = false;
                }


            }

#ifndef _DEBUG
            if (pAlive == 1)
            {

                for (auto& i : playCharacters)
                {
                    if (i->controllingPlayer == nullptr) continue;

                    if (i->controllingPlayer->hp > 0)
                    {
                        DBOUT("Player " << i->controllingPlayer->pID << " wins!\n");
                        winOrder.push_back(i->controllingPlayer);
                        i->controllingPlayer = nullptr;
                        allDead = true;
                    }

                }

            }
#endif

            if (allDead)
            {
                DBOUT("Everyone is dead!\n");
                transToEndScreen = true;
                transitionInProgress = 1;
            }
        }
    }
    else if (gameState == MainGameState::END_SCREEN)
    {
        switchLevel(*endLevel);
        if (transToRegistration && transitionInProgress == 0)
        {
            clearData();
            gameState = MainGameState::PLAYER_REGISTRATION;
            transitionInProgress = 2;
            transToRegistration = false;
            themeChannel = res->getSound()->add("theme", true);
        }
        else
        {

            endTimer += deltaTime;

            /*jump on podium*/
            for (auto& i : players)
            {
                if (playCharacters[i->getCharacter()]->Velocity.y <= 0.f && input->ButtonPressed(i->getInput(), BUTTON_A))
                {
                    playCharacters[i->getCharacter()]->Velocity.y = 5.f;
                }

            }

            /*move camera*/
            /*start @ XMFLOAT3(-13.f, 22.f, -30.f)*/
            endCameraTime += deltaTime;

            XMFLOAT3 endPos;
            endPos.y = ENDCAMERA_Y - (.8f * endCameraTime);
            endPos.x = ENDCAMERA_X + (2.0f * endCameraTime);
            endPos.z = ENDCAMERA_Z;

            endScreenCamera.lookAt(endPos, XMFLOAT3(0.0,6.0,0.0),XMFLOAT3(0.0,1.0,0.0));


            for (auto& p : playCharacters)
            {
                p->Update(deltaTime);
            }

            /*init switch back to player reg*/
            if (endTimer >= END_TIME_V)
            {
                transToRegistration = true;
                transitionInProgress = 1;
            }

        }
    }

    ////handle input
    //InputData* in = input->getInput(controllingInput);
    //InputData* prevIn = input->getPrevInput(controllingInput);

    //float tlX = in->trigger[THUMB_LX];
    //float tlY = in->trigger[THUMB_LY];

    //float trX = in->trigger[THUMB_RX];
    //float trY = in->trigger[THUMB_RY] * -1;

    //float ws = tlY * 10.f * deltaTime;
    //float ss = tlX * 10.f * deltaTime;


    //gCamera.walk(ws);
    //gCamera.strafe(ss);

    //float yaw = 1.5f * deltaTime * trX;
    //float pitch = 1.5f * deltaTime * trY;

    //gCamera.yaw(yaw);
    //gCamera.pitch(pitch);

    /*particle system*/

    activeLevel->Update(deltaTime);
    mRain.update(deltaTime, gTime.getTotalTime());

    /*rotate light*/
    lightRotationAngle += 0.1f * deltaTime;
    XMMATRIX R = XMMatrixRotationY(lightRotationAngle);
    XMVECTOR lDir = XMLoadFloat3(&originalLightDir);
    lDir = XMVector3TransformNormal(lDir, R);
    DirectX::XMStoreFloat3(&gDirLights.Direction, lDir);

    /*build shadow transform*/
    buildShadowTransform();

    /*update 2d*/
    bPressA.update(deltaTime);
    bPressStart.update(deltaTime);

    /*update sound*/
    res->getSound()->update(deltaTime);

    /*update camera position*/
    activeCamera->UpdateViewMatrix();

}


void DXTest::Draw()
{
    ID3D11ShaderResourceView* tResourceView = 0;
    ID3D11UnorderedAccessView* tUAView = 0;
    ID3D11RenderTargetView* tRenderTargetView = 0;

    for (int f = 0; f < (gameState == MainGameState::INGAME ? 4 : 1); f++)
    {
        if (gameState == MainGameState::PLAYER_REGISTRATION)
        {
            activeCamera = &introCamera;
            tResourceView = mOffscreenSRV;
            tUAView = mOffscreenUAV;
            tRenderTargetView = mOffscreenRTV;
        }
        else if (gameState == MainGameState::END_SCREEN)
        {
            activeCamera = &endScreenCamera;
            tResourceView = mOffscreenSRV;
            tUAView = mOffscreenUAV;
            tRenderTargetView = mOffscreenRTV;
        }
        else
        {
            activeCamera = playCharacters[f]->getCamera();
            tResourceView = playCharacters[f]->splitScreenSRV;
            tUAView = playCharacters[f]->splitScreenUAV;
            tRenderTargetView = playCharacters[f]->splitScreenView;
        }

        /*draw to shadow map*/
        shadowMap->BindDsvAndSetNullRenderTarget(deviceContext);

        XMMATRIX lview = XMLoadFloat4x4(&lightView);
        XMMATRIX lproj = XMLoadFloat4x4(&lightProj);

        /*draw shadow of static models*/
        deviceContext->IASetInputLayout(InputLayouts::Standard);
        deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        std::map<int, ModelInstanceStatic*>::iterator it = activeLevel->modelsStatic.begin();
        while (it != activeLevel->modelsStatic.end())
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
        ID3D11RenderTargetView* renderTargets[1] = { tRenderTargetView };
        deviceContext->OMSetRenderTargets(1, renderTargets, depthStencilView);
        deviceContext->RSSetViewports(1, &mainViewport);

        //deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
        deviceContext->ClearRenderTargetView(tRenderTargetView, clearColor);
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

        it = activeLevel->modelsStatic.begin();
        while (it != activeLevel->modelsStatic.end())
        {
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

        /*particle system*/

        for (auto& i : activeLevel->particleSystems)
        {
            i.second->setEyePos(activeCamera->getPosition());
            i.second->draw(deviceContext, *activeCamera);
        }
        deviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);

        /*rain*/

        if (gameState == MainGameState::PLAYER_REGISTRATION)
        {
            mRain.setEyePos(activeCamera->getPosition());
            mRain.setEmitPosition(activeCamera->getPosition());

            mRain.draw(deviceContext, *activeCamera);
        }

        deviceContext->RSSetState(0);
        deviceContext->OMSetDepthStencilState(0, 0);
        deviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);

        /*blur*/

        renderTargets[0] = renderTargetView;
        deviceContext->OMSetRenderTargets(1, renderTargets, depthStencilView);

        if (blurStrength > 0)
        {
            blurEffect.BlurSRV(deviceContext, tResourceView, tUAView, blurStrength);
        }


        DrawScreenQuad(tResourceView);


        /*default*/
        deviceContext->RSSetState(0);
        deviceContext->OMSetDepthStencilState(0, 0);
        ID3D11ShaderResourceView* nullSRV[16] = { 0 };
        deviceContext->PSSetShaderResources(0, 16, nullSRV);

    }

    /*D2D and DWrite Rendering*/

    d2dContext->BeginDraw();

    /*test heart*/
    //D2D1_RECT_F r = D2D1::RectF(100, 100, 150, 150);
    //d2dContext->DrawBitmap(res->getBitmap()->get(L"heart"), r, .5f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, NULL);

    if (gameState == MainGameState::PLAYER_REGISTRATION)
    {
        bTitle.draw(d2dContext.Get(), res);

        if (regState != RegistrationState::FULL)
        {
            bPressA.draw(d2dContext.Get(), res);
        }

        if (regState != RegistrationState::BLANK)
        {
            bPressStart.draw(d2dContext.Get(), res);
        }
        
        
    }
    else if (gameState == MainGameState::INGAME)
    {

        /*draw player hp*/
        for (auto& p : playCharacters)
        {
            int i = p->metaPosition;
            /*draw bot icon*/
            if (p->npc || p->controllingPlayer == nullptr)
            {
                d2dContext->DrawBitmap(res->getBitmap()->get(L"bot"), relativePos(uiBase[i].left, uiBase[i].top, uiBase[i].right, wndWidth, wndHeight));
            }
            /*draw hp*/
            else
            {
                D2D1_RECT_F hpRect;

                for (int h = 0; h < p->controllingPlayer->hp; h++)
                {
                    hpRect = relativePos(uiBase[i].left + h * 0.035f, uiBase[i].top, uiBase[i].right + h*0.035f, wndWidth, wndHeight);
                    d2dContext->DrawBitmap(res->getBitmap()->get(L"heart2"), hpRect, 1.f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, NULL);
                }

            }
        }

    }

    /*fade in / out effect*/
    static D2D1_RECT_F fadeRect = D2D1::RectF(0, 0, (float)wndWidth, (float)wndHeight);
    if (fadeValue > 0)
    {
        fadeBrush->SetOpacity(fadeValue);
        d2dContext->FillRectangle(fadeRect, fadeBrush);
    }

    /*pause bitmap*/
    if (ingameState == InGameState::PAUSE)
    {
        bPause.draw(d2dContext.Get(), res);
    }


    drawFPSCounter();

    d2dContext->EndDraw();

    /* Ellipse test
    D2D1_ELLIPSE l;
    l.point.x = 50.f;
    l.point.y = 50.f;
    l.radiusX = 20.f;
    l.radiusY = 44.f;
    ID2D1StrokeStyle* str;

    float dashes[] = { 1.0f, 2.0f, 2.0f, 3.0f, 2.0f, 2.0f };


        d2dFactory->CreateStrokeStyle(
            D2D1::StrokeStyleProperties(
            D2D1_CAP_STYLE_FLAT,
            D2D1_CAP_STYLE_FLAT,
            D2D1_CAP_STYLE_ROUND,
            D2D1_LINE_JOIN_MITER,
            10.0f,
            D2D1_DASH_STYLE_CUSTOM,
            0.0f),
            dashes,
            ARRAYSIZE(dashes), &str
        );
    

    d2dContext->DrawEllipse(l, blackBrush.Get(), 1.f, str);
    */

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
    vbd.ByteWidth = (UINT)(sizeof(Vertex::Standard) * Vertices.size());
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
    
    for (int k = 0; k < (gameState == MainGameState::INGAME ? 4 : 1); k++)
    {
        UINT stride = sizeof(Vertex::Standard);
        UINT offset = 0;

        deviceContext->IASetInputLayout(InputLayouts::Standard);
        deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        deviceContext->IASetVertexBuffers(0, 1, &mScreenQuadVB, &stride, &offset);
        deviceContext->IASetIndexBuffer(mScreenQuadIB, DXGI_FORMAT_R32_UINT, 0);

        // 
       // XMMATRIX world = XMMatrixIdentity();
        XMMATRIX world;

        //splitscreen
        if (gameState == MainGameState::INGAME)
        {
            if (k == 0)
            {
                world = XMMATRIX(
                    0.5f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.5f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.f, 0.0f,
                    -0.5f, 0.5f, 0.0f, 1.f);
            }
            else if (k == 1)
            {
                world = XMMATRIX(
                    0.5f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.5f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.5f, 0.5f, 0.0f, 1.0f);
            }
            else if (k == 2)
            {
                world = XMMATRIX(
                    0.5f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.5f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    -0.5f, -0.5f, 0.0f, 1.0f);
            }
            else
            {
                world = XMMATRIX(
                    0.5f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.5f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.5f, -0.5f, 0.0f, 1.0f);
            }
        }
        //one screen
        else
        {
            world = XMMatrixIdentity();
        }
        ID3DX11EffectTechnique* tech = Shaders::fullscreenShader->ViewStandard;
        D3DX11_TECHNIQUE_DESC techDesc;

        tech->GetDesc(&techDesc);
        for (UINT p = 0; p < techDesc.Passes; ++p)
        {
            Shaders::fullscreenShader->SetWorldViewProj(world);

            if (gameState == MainGameState::INGAME)
            {
                Shaders::fullscreenShader->SetTexture(playCharacters[k]->splitScreenSRV);
            }
            else
            {
                Shaders::fullscreenShader->SetTexture(srv);
            }

            //Shaders::fullscreenShader->SetFadeValue(0.f);

            tech->GetPassByIndex(p)->Apply(0, deviceContext);
            deviceContext->DrawIndexed(6, 0, 0);
        }
    }
}


void DXTest::BuildOffscreenViews()
{

    DXRelease(mOffscreenSRV);
    DXRelease(mOffscreenRTV);
    DXRelease(mOffscreenUAV);

    for (auto& i : playCharacters)
    {
        DXRelease(i->splitScreenView);
    }

    /*main offscreen view*/
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


    /*split screen views*/
    for (auto& i : playCharacters)
    {
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

        device->CreateShaderResourceView(offscreenTex, 0, &(i->splitScreenSRV));
        device->CreateRenderTargetView(offscreenTex, 0, &(i->splitScreenView));
        device->CreateUnorderedAccessView(offscreenTex, 0, &(i->splitScreenUAV));

        DXRelease(offscreenTex);
    }
}

void DXTest::clearData()
{
    winOrder.clear();

    for (auto& i : players)
    {
        delete i;
    }
    players.clear();

    for (auto& i : playCharacters)
    {
        i->controllingPlayer = nullptr;
        i->npc = true;
        i->Color = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
        i->BaseHeight = 1.f;
    }

    playball->resetBallFull();

    playerCount = 0;
    allDead = false;
    blurStrength = 1;
    endTimer = 0.f;

    playCharacters[0]->Translation = XMFLOAT3(0.f, PLAYER_HEIGHT, 0.f);
    playCharacters[1]->Translation = XMFLOAT3(0.f, PLAYER_HEIGHT, 0.f);
    playCharacters[2]->Translation = XMFLOAT3(0.f, PLAYER_HEIGHT, 0.f);
    playCharacters[3]->Translation = XMFLOAT3(0.f, PLAYER_HEIGHT, 0.f);

    playCharacters[0]->Translation.z = -PLAYER_DISTANCE;
    playCharacters[1]->Translation.z = PLAYER_DISTANCE;
    playCharacters[2]->Translation.x = -PLAYER_DISTANCE;
    playCharacters[3]->Translation.x = PLAYER_DISTANCE;

    playCharacters[2]->Rotation.z = XM_PIDIV2;
    playCharacters[3]->Rotation.z = -XM_PIDIV2;
    playCharacters[2]->Orientation = true;
    playCharacters[3]->Orientation = true;

    playCharacters[2]->hitBox.Orientation = XMFLOAT4(0.f, 0.f, 0.7071068f, 0.7071068f);
    playCharacters[3]->hitBox.Orientation = XMFLOAT4(0.f, 0.f, -0.7071068f, 0.7071068f);
}

void DXTest::setupEndScreen()
{

    /*place camera*/
    endCameraTime = 0;
    endScreenCamera.lookAt(XMFLOAT3(ENDCAMERA_X, ENDCAMERA_Y, ENDCAMERA_Z), XMFLOAT3(0.f, 6.f, 0.f), XMFLOAT3(0.f, 1.0f, 0.f));


    activeCamera = &endScreenCamera;

    /*place players in right order*/
    DBOUT(winOrder.size() << " in the win order\n");
    ASSERT(winOrder.size() <= 4);


    int counter = 4 - (int)winOrder.size();

    for (auto& i : winOrder)
    {

        if (counter == 0)
        {
            playCharacters[i->getCharacter()]->Translation.x = 0;
            playCharacters[i->getCharacter()]->Translation.y = -10;
            playCharacters[i->getCharacter()]->Translation.z = 0;
        }
        else if (counter == 1)
        {
            playCharacters[i->getCharacter()]->Translation.x = -8.75;
            playCharacters[i->getCharacter()]->Translation.y = 7;
            playCharacters[i->getCharacter()]->Translation.z = -2;

            playCharacters[i->getCharacter()]->BaseHeight = 7;

            if (playCharacters[i->getCharacter()]->Orientation)
            {
                playCharacters[i->getCharacter()]->Rotation.z = 0;
            }
        }
        else if (counter == 2)
        {
            playCharacters[i->getCharacter()]->Translation.x = 8.75;
            playCharacters[i->getCharacter()]->Translation.y = 8.5;
            playCharacters[i->getCharacter()]->Translation.z = -2;

            playCharacters[i->getCharacter()]->BaseHeight = 8.5;

            if (playCharacters[i->getCharacter()]->Orientation)
            {
                playCharacters[i->getCharacter()]->Rotation.z = 0;
            }
        }
        else
        {
            playCharacters[i->getCharacter()]->Translation.x = 0;
            playCharacters[i->getCharacter()]->Translation.y = 10;
            playCharacters[i->getCharacter()]->Translation.z = 0;

            playCharacters[i->getCharacter()]->BaseHeight = 10;

            if (playCharacters[i->getCharacter()]->Orientation)
            {
                playCharacters[i->getCharacter()]->Rotation.z = 0;
            }
        }

        counter++;
    }

}