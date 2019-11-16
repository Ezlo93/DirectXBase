#pragma once

#include "util.h"

class BitmapManager
{
public:
    BitmapManager(Microsoft::WRL::ComPtr<IWICImagingFactory2> fac, Microsoft::WRL::ComPtr<ID2D1DeviceContext1> con);
    void loadBitmap(LPWSTR file);
    ID2D1Bitmap1* get(const std::wstring& i);

private:
    Microsoft::WRL::ComPtr<IWICImagingFactory2> factory;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext1> context;
    std::map<std::wstring, ID2D1Bitmap1*> bitmaps;
};