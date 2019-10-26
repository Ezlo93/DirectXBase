#pragma once

#include "xaudio2.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include "SharedQueue.h"

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

    void loadFile(const std::wstring& file, std::vector<BYTE>& data, WAVEFORMATEX** formatEx, unsigned int& length);

    /*xaudio2*/
    IXAudio2* soundMain;
    IXAudio2MasteringVoice* masterVoice;

    /*wmf*/
    IMFAttributes* srcReaderConfig;

    /**/
    SharedQueue<int> playList;

};