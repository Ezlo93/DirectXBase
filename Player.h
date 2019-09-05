#pragma once
class Player
{
public:
    Player();

    void AssignCharacter(int i);
    void Init();

private:
    int assignedCharacter = -1;
    int hp;
};

