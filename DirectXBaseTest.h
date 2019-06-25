#pragma once

#include "DirectXBase.h"

class DXTest : public DirectXBase
{
public:
    DXTest(HINSTANCE hProgramID);
    ~DXTest();

    bool Initialisation();
    void OnWindowResize();
    void Update(float deltaTime);
    void Draw();

private:
    float clearColor[4];
};