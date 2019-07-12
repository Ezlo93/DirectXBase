#pragma once

#include <Windows.h>

class GameTime
{
public:
    GameTime();

    float getTotalTime(); // in s
    float getDeltaTime(); // in s

    void Start();
    void Stop();
    void Inc();
    void Reset();

private:
    double secondsPerCount;
    double deltaTime;

    __int64 baseTime;
    __int64 pausedTime;
    __int64 stopTime;
    __int64 prevTime;
    __int64 currTime;

    bool stopped;

};