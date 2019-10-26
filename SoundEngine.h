#pragma once

#include "xaudio2.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <map>
#include "SharedQueue.h"

#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "xaudio2.lib")

class XAudio2SoundEngine
{

public:
    XAudio2SoundEngine() = default;
    ~XAudio2SoundEngine();

    void Init();
    void loadFile(const std::wstring& file, std::vector<BYTE>& data, WAVEFORMATEX** formatEx, unsigned int& length);

private:

    /*xaudio2*/
    IXAudio2* soundMain;
    IXAudio2MasteringVoice* masterVoice;

    /*wmf*/
    IMFAttributes* srcReaderConfig;

    friend class SoundEngine;
};

class SoundEvent
{
private:
    IXAudio2SourceVoice* srcVoice = nullptr;
    WAVEFORMATEX waveFormat;
    unsigned int waveLength = 0;
    std::vector<BYTE> audioData;
    XAUDIO2_BUFFER audioBuffer;
    unsigned int index = 0;

public:
    SoundEvent() = default;

    friend class SoundEngine;

};

class SoundEngine
{
public:
    SoundEngine();
    ~SoundEngine();

    void loadFile(const std::wstring& fileName);

    void add(const std::string& id);

    void update();

private:
    XAudio2SoundEngine* engine;
    std::map<std::string, SoundEvent*> soundCollection;
    SharedQueue<std::string> playList;
};

