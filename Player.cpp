#include "Player.h"


Player::Player()
{
    hp = 5;
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
