#include "BitmapManager.h"

BitmapManager::BitmapManager(Microsoft::WRL::ComPtr<IWICImagingFactory2> fac, Microsoft::WRL::ComPtr<ID2D1DeviceContext1> con)
{
    factory = fac;
    context = con;
}

void BitmapManager::loadBitmap(LPWSTR file)
{
    std::wstring t = file;
    wchar_t id[128];
    wchar_t ext[12];
    _wsplitpath_s(t.c_str(), NULL, 0, NULL, 0, id, 128, ext, 12);

    ID2D1Bitmap1* bitmap;
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> bitmapDecoder;
    if (FAILED(factory->CreateDecoderFromFilename(file, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, bitmapDecoder.GetAddressOf())))
    {
        return;
    }

    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
    if (FAILED(bitmapDecoder->GetFrame(0, frame.GetAddressOf())))
    {
        return;
    }

    Microsoft::WRL::ComPtr<IWICFormatConverter> fc;
    factory->CreateFormatConverter(fc.GetAddressOf());

    if (FAILED(fc->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom))){
        return;
    }

    if (FAILED(context->CreateBitmapFromWicBitmap(fc.Get(), &bitmap)))
    {
        return;
    }

    bitmaps.insert(std::make_pair(id, bitmap));
}

ID2D1Bitmap1* BitmapManager::get(const std::wstring& i)
{
    return bitmaps[i];
}
