#include "DirectXBaseTest.h"


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
}


void DXTest::Update(float deltaTime)
{

}


void DXTest::Draw()
{

}