#pragma once

#include "util.h"
#include "ResourceManager.h"

class DrawableBitmap
{
public:
    DrawableBitmap() = default;

    void setup(const std::wstring& _id, D2D_RECT_F _pos, float _alpha = 1.f)
    {
        id = _id;
        currentPosition = _pos;
        alpha = _alpha;
    }

    D2D_RECT_F& getPosition() { return currentPosition; }
    std::wstring& getID() { return id; }

    void draw(ID2D1DeviceContext* context, ResourceManager* r)
    {
        context->DrawBitmap(r->getBitmap()->get(id), currentPosition, alpha, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, NULL);
    }

private:

    std::wstring id = L"default";
    D2D_RECT_F currentPosition;
    float alpha = 1.0f;
};

static D2D_RECT_F relativePos(float x, float y, float x2, float y2, int wndW, int wndH)
{
    return D2D1::RectF(x * wndW, y * wndH, x2 * wndW, y2 * wndH);
}