#include "DirectXBaseTest.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

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

}

bool DXTest::Initialisation()
{
    if (!DirectXBase::Initialisation())
    {
        return false;
    }

    /*...*/

    return true;
}


void DXTest::OnWindowResize()
{
    DirectXBase::OnWindowResize();

    /*recalc. aspect ratio*/
}


void DXTest::Update(float deltaTime)
{
    

    /*game logic*/


}


void DXTest::Draw()
{
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    /*draw*/


    swapChain->Present(0, 0);
}