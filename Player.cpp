#include "Player.h"


Player::Player()
{
    hp = MAX_HP;
    inputID = -1;
    Color = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
}

void Player::AssignCharacter(int i)
{
    assignedCharacter = i;
}

void Player::AssignColor(const XMFLOAT4& col)
{
    Color = col;
}

void Player::AssignInput(int i)
{
    inputID = i;
}

int Player::getInput()
{
    return inputID;
}

XMFLOAT4& Player::getColor()
{
    return Color;
}

int Player::getCharacter()
{
    return assignedCharacter;
}
