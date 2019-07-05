/*DirectXBase.cpp
    minimum template for directx11 application


    Nicolai Sehrt
*/


#include "DirectXBase.h"
#include <sstream>

namespace {
    DirectXBase* directXBase = 0;
}

/*used for windows message handling*/
#pragma warning(suppress: 28251)
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return directXBase->MsgProc(hwnd, msg, wParam, lParam);
}

/*constructor, init some default values*/
DirectXBase::DirectXBase(HINSTANCE hProgramID) :
    programID(hProgramID),
    wndTitle(L"DirectXBase"),
    wndWidth(1280),
    wndHeight(720),
    wndHandle(0),
    wndInactive(false),
    wndMaximized(false),
    wndMinimized(false),
    wndResizing(false),
    device(0),
    deviceContext(0),
    swapChain(0),
    depthStencilBuffer(0),
    renderTargetView(0),
    depthStencilView(0)
{

    RtlSecureZeroMemory(&mainViewport, sizeof(D3D11_VIEWPORT));
    directXBase = this;
}

/*destructor release all d3d11 vars*/
DirectXBase::~DirectXBase()
{
    DXRelease(renderTargetView);
    DXRelease(depthStencilView);
    DXRelease(swapChain);
    DXRelease(depthStencilBuffer);

    if (deviceContext)
        deviceContext->ClearState();

    DXRelease(deviceContext);
    DXRelease(device);

}

/*getter*/
HINSTANCE DirectXBase::getProgramID()
{
    return programID;
}

float DirectXBase::getAspectRatio()
{
    return (float)(wndWidth / wndHeight);
}

HWND DirectXBase::getWindowHandle()
{
    return wndHandle;
}

/*init functions*/

bool DirectXBase::Initialisation()
{
    if (!InitWindow() || !InitDirect3D())
    {
        return false;
    }
    else
    {
        return true;
    }
}


/*window*/

bool DirectXBase::InitWindow()
{
    /*win struct*/
    WNDCLASS wndClass;
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = MainWndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = programID;
    wndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(0, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wndClass.lpszMenuName = 0;
    wndClass.lpszClassName = L"DXWINDOW";

    /*register it*/
    if (!RegisterClass(&wndClass))
    {
        return false;
    }

    /*wnd size*/
    RECT rect = { 0,0,(LONG)wndWidth, (LONG)wndHeight };
    AdjustWindowRect(&rect, WS_EX_OVERLAPPEDWINDOW, false);

    wndHandle = CreateWindowEx(0, L"DXWINDOW", wndTitle.c_str(), WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT,
        (rect.right - rect.left), (rect.bottom - rect.top), 0, 0, programID, 0);

    if (!wndHandle)
    {
        return false;
    }

    /*only run once*/
    CreateMutexA(0, false, "___dxbasemutex___");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        MessageBox(wndHandle, L"Application already running!", L"Error", MB_OK);
        return 0;
    }
    SetWindowPos(wndHandle, wndHandle, 100, 100, 0, 0, 0);
    ShowWindow(wndHandle, SW_SHOW);
    UpdateWindow(wndHandle);
    ShowCursor(false);

    return true;

}


/*dx11*/

bool DirectXBase::InitDirect3D()
{
    /*create device*/
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT hr = D3D11CreateDevice(
        0,
        D3D_DRIVER_TYPE_HARDWARE,
        0,
        0,
        0,
        0,
        D3D11_SDK_VERSION,
        &device,
        &featureLevel,
        &deviceContext
    );

    if (FAILED(hr) || featureLevel != D3D_FEATURE_LEVEL_11_0)
    {
        return false;
    }

    /*swap chain*/
    DXGI_SWAP_CHAIN_DESC sDesc;
    sDesc.BufferDesc.Width = wndWidth;
    sDesc.BufferDesc.Height = wndHeight;
    sDesc.BufferDesc.RefreshRate.Numerator = 60; // !!!
    sDesc.BufferDesc.RefreshRate.Denominator = 1;
    sDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sDesc.SampleDesc.Count = 1;
    sDesc.SampleDesc.Quality = 0;
    sDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sDesc.BufferCount = 1;
    sDesc.OutputWindow = wndHandle;
    sDesc.Windowed = true;
    sDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL; // !!!
    sDesc.Flags = 0;

    /*what is happening*/

    IDXGIDevice* dxgiDevice = 0;
    IDXGIAdapter* dxgiAdapter = 0;
    IDXGIFactory* dxgiFactory = 0;

    device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
    dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)& dxgiFactory);
    dxgiFactory->CreateSwapChain(device, &sDesc, &swapChain);

    /*disable alt enter*/
    dxgiFactory->MakeWindowAssociation(wndHandle, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);


    DXRelease(dxgiDevice);
    DXRelease(dxgiAdapter);
    DXRelease(dxgiFactory);

    OnWindowResize();

    return true;
}

/*window resize*/
void DirectXBase::OnWindowResize()
{

    DXRelease(renderTargetView);
    DXRelease(depthStencilView);
    DXRelease(depthStencilBuffer);

    /*swapchain change, buffer creation*/
    swapChain->ResizeBuffers(2, wndWidth, wndHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    ID3D11Texture2D* backBuffer;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
#pragma warning(suppress: 6387)
    device->CreateRenderTargetView(backBuffer, 0, &renderTargetView);
    DXRelease(backBuffer);

    /*depth stencil view*/
    D3D11_TEXTURE2D_DESC dSDesc;

    dSDesc.Width = wndWidth;
    dSDesc.Height = wndHeight;
    dSDesc.MipLevels = 1;
    dSDesc.ArraySize = 1;
    dSDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dSDesc.SampleDesc.Count = 1;
    dSDesc.SampleDesc.Quality = 0;
    dSDesc.Usage = D3D11_USAGE_DEFAULT;
    dSDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    dSDesc.CPUAccessFlags = 0;
    dSDesc.MiscFlags = 0;

    device->CreateTexture2D(&dSDesc, 0, &depthStencilBuffer);
#pragma warning(suppress: 6387)
    device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView);

    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    /*viewport*/

    mainViewport.TopLeftX = 0;
    mainViewport.TopLeftY = 0;
    mainViewport.Width = static_cast<float>(wndWidth);
    mainViewport.Height = static_cast<float>(wndHeight);
    mainViewport.MinDepth = 0.0f;
    mainViewport.MaxDepth = 1.0f;

    deviceContext->RSSetViewports(1, &mainViewport);

}



/*run loop*/
int DirectXBase::Run()
{
    MSG msg = { 0 };

    gTime.Reset();
    
    while (msg.message != WM_QUIT)
    {
        /*if window message*/
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        /*continue game loop*/
        else
        {
            gTime.Inc();

            if (!wndInactive)
            {
                UpdateFPSCounter();
                Update(gTime.getDeltaTime());
                Draw();
            }
            else
            {
                /*wait for active again*/
                Sleep(10);
            }

        }
    }

    return (int)msg.wParam;
}



/*window message handler*/

LRESULT DirectXBase::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    /*window active/inactive*/
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            wndInactive = true;
            gTime.Stop();
        }
        else
        {
            wndInactive = false;
            gTime.Start();
        }
        return 0;

     // resizing window
    case WM_SIZE:

        wndWidth = LOWORD(lParam);
        wndHeight = HIWORD(lParam);
        if (device)
        {
            if (wParam == SIZE_MINIMIZED)
            {
                wndInactive = true;
                wndMinimized = true;
                wndMaximized = false;
            }
            else if (wParam == SIZE_MAXIMIZED)
            {
                wndInactive = false;
                wndMinimized = false;
                wndMaximized = true;
                OnWindowResize();
            }
            else if (wParam == SIZE_RESTORED)
            {

                if (wndMinimized)
                {
                    wndInactive = false;
                    wndMinimized = false;
                    OnWindowResize();
                }
                else if (wndMaximized)
                {
                    wndInactive = false;
                    wndMaximized = false;
                    OnWindowResize();
                }
                else if (wndResizing)
                {
                    // wait until user finished resizing
                }
                else
                {
                    OnWindowResize();
                }
            }
        }
        return 0;

        // grab resize bar
    case WM_ENTERSIZEMOVE:
        wndInactive = true;
        wndResizing = true;
        gTime.Stop();
        return 0;

        // finished resizing
    case WM_EXITSIZEMOVE:
        wndInactive = false;
        wndResizing = false;
        gTime.Start();
        OnWindowResize();
        return 0;

        // dead
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

        // whatever
    case WM_MENUCHAR:
        return MAKELRESULT(0, MNC_CLOSE);

        // not too small
    case WM_GETMINMAXINFO:
        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 800;
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 600;
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

/*show fps and frame time in title bar*/
void DirectXBase::UpdateFPSCounter()
{
    static int frameCount = 0;
    static float timeElapsed = 0.0f;

    frameCount++;

    if ((gTime.getTotalTime() - timeElapsed) >= 1.0f)
    {
        float fps = (float)frameCount;
        float mspf = 1000.0f / fps;

        std::wostringstream out;
        out.precision(6);

        out << wndTitle << L"     FPS: " << fps << L" Frame Time: " << mspf << L"ms";
        SetWindowText(wndHandle, out.str().c_str());

        frameCount = 0;
        timeElapsed += 1.0f;

    }


}

/*set fullscreen*/
bool DirectXBase::goFullscreen(bool s)
{
    IDXGIOutput* display = 0;

    HRESULT hr = swapChain->GetContainingOutput(&display);

    if (FAILED(hr))
    {
        return false;
    }

    hr = swapChain->SetFullscreenState(s, display);

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}