#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#include <string>

#define DXRelease(x) { if(x){x->Release(); x = 0; }}

class DirectXBase {

public:
    DirectXBase(HINSTANCE hProgramID);
    virtual ~DirectXBase();

    HINSTANCE getProgramID();
    float getAspectRatio();
    HWND getWindowHandle();

    int Run();

    /*to be overriden by child class*/
    virtual bool Initialisation();
    virtual void Update(float deltaTime);
    virtual void Draw();
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    virtual void OnWindowResize();

protected:

    bool InitWindow();
    bool InitDirect3D();

    void UpdateFPSCounter();

    /*windows related*/
    std::wstring wndTitle;
    HINSTANCE programID;
    HWND wndHandle;

    bool wndInactive, wndMinimized, wndMaximized, wndResizing;
    UINT wndWidth, wndHeight;

    /*dx11 related*/
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;
    ID3D11Texture2D* depthStencilBuffer;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11DepthStencilView* depthStencilView;
    D3D11_VIEWPORT mainViewport;





};