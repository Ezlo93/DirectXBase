#pragma once
#include "util.h"

#define MAX_HP 3

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

    int hp;
    int pID = -1;
private:
    int assignedCharacter = -1;
    
    int inputID;
    XMFLOAT4 Color;
};

