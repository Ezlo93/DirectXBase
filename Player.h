#pragma once
#include "util.h"

class Player
{
public:
    Player();

    void AssignCharacter(int i);
    void AssignColor(const XMFLOAT4& col);
    void AssignInput(int i);

    int getInput();
    XMFLOAT4& getColor();
    int getCharacter();

private:
    int assignedCharacter = -1;
    int hp;
    int inputID;
    XMFLOAT4 Color;
};

