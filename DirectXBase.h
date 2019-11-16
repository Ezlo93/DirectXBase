#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "util.h"
#include "DDSTextureLoader.h"
#include <iostream>
#include <sstream>
#include "GameTime.h"

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
    virtual void Update(float deltaTime)=0;
    virtual void Draw()=0;
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    virtual void OnWindowResize();
    virtual bool goFullscreen(bool s);
    void drawFPSCounter();

protected:

    bool InitWindow();
    bool InitDirect3D();
    void createD2DRenderTarget();
    void initializeTextFormats();

    void UpdateFPSCounter();

    /*windows related*/
    std::wstring wndTitle;
    HINSTANCE programID;
    HWND wndHandle;

    bool wndInactive, wndMinimized, wndMaximized, wndResizing;
    UINT wndWidth, wndHeight;

    /*dx11 related*/
    GameTime gTime;
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;
    ID3D11Texture2D* depthStencilBuffer;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11DepthStencilView* depthStencilView;
    D3D11_VIEWPORT mainViewport;

    /*d2d & dwrite related*/
    Microsoft::WRL::ComPtr<ID2D1Factory2> d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device1> d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext1> d2dContext;

    Microsoft::WRL::ComPtr<IDWriteFactory> dwriteFactory;

    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> blackBrush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> whiteBrush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> redBrush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> blueBrush;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> yellowBrush;

    Microsoft::WRL::ComPtr<IDWriteTextFormat> stdTextFormat;

    Microsoft::WRL::ComPtr<IDWriteTextLayout> fpsOutLayout;
};