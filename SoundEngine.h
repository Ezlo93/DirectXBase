#pragma once

#include "xaudio2.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "xaudio2.lib")

class SoundEngine
{

public:
    SoundEngine() = default;
    ~SoundEngine();

    void Init();



private:
    IXAudio2* soundMain;
    IXAudio2MasteringVoice* masterVoice;

};