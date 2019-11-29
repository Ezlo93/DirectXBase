#pragma once

#include "DrawableBitmap.h"

class AnimatedBitmap : public DrawableBitmap
{

public:

    AnimatedBitmap() = default;

    void setAnim(float animTime, D2D_RECT_F start, D2D_RECT_F end)
    {
        aTime = animTime;
        hTime = aTime / 2.f;
        startPos = start;
        endPos = end;
    }

    void update(float deltaTime)
    {
        animTimer += deltaTime;

        if (animTimer > aTime)
        {
            animTimer -= aTime;
        }

        float temp = animTimer > hTime ? (hTime - (animTimer-hTime)) : animTimer;
 
        currentPosition.left = startPos.left + (endPos.left - startPos.left) * (temp/hTime);
        currentPosition.right = startPos.right + (endPos.right - startPos.right) * (temp / hTime);
        currentPosition.bottom = startPos.bottom + (endPos.bottom - startPos.bottom) * (temp / hTime);
        currentPosition.bottom = startPos.bottom + (endPos.bottom - startPos.bottom) * (temp / hTime);
    }

private:

    D2D_RECT_F startPos, endPos;
    float animTimer = 0.f, aTime = 0.f, hTime = 0.f;

};