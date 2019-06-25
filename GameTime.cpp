/*GameTime.cpp
    provides a timer that keeps track of total and delta time between frames

    Nicolai Sehrt
*/


#include <Windows.h>
#include "GameTime.h"

GameTime::GameTime()
    : secondsPerCount(0), deltaTime(-1.0), baseTime(0), pausedTime(0), stopTime(0), prevTime(0),currTime(0), stopped(false)
{
    __int64 cPS;
    QueryPerformanceFrequency((LARGE_INTEGER*)& cPS);
    secondsPerCount = 1.0 / (double)cPS;
}


float GameTime::getTotalTime()
{
    if (stopped)
    {
        return (float)(((stopTime - pausedTime) - baseTime) * secondsPerCount);
    }
    else
    {
        return (float)(((currTime - pausedTime) - baseTime) * secondsPerCount);
    }
}

float GameTime::getDeltaTime()
{
    return (float)deltaTime;
}


void GameTime::Start()
{
    __int64 startTime;
    QueryPerformanceCounter((LARGE_INTEGER*)& startTime);

    if (stopped)
    {
        pausedTime += (startTime - stopTime);
        prevTime = startTime;
        stopTime = 0;
        stopped = false;
    }
}


void GameTime::Stop()
{
    if (!stopped)
    {
        __int64 cTime;
        QueryPerformanceCounter((LARGE_INTEGER*)& cTime);

        stopTime = cTime;
        stopped = true;
    }
}


void GameTime::Reset()
{
    __int64 cTime;
    QueryPerformanceCounter((LARGE_INTEGER*)& cTime);

    baseTime = cTime;
    prevTime = cTime;
    stopTime = 0;
    stopped = false;
}


void GameTime::Inc()
{
    if (stopped)
    {
        deltaTime = 0.0;
        return;
    }

    __int64 cTime;
    QueryPerformanceCounter((LARGE_INTEGER*)& cTime);
    currTime = cTime;
    deltaTime = (currTime - prevTime) * secondsPerCount;
    prevTime = currTime;

    if (deltaTime < 0.0)
    {
        deltaTime = 0.0;
    }


}