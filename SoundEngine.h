#pragma once

#include "xaudio2.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <map>
#include <vector>
#include <string>

#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "xaudio2.lib")

struct AudioData
{
    WAVEFORMATEX waveFormat;
    unsigned int waveLength;
    std::vector<BYTE> data;
    XAUDIO2_BUFFER audioBuffer;
};


class SoundEvent
{
private:
    IXAudio2SourceVoice* srcVoice = nullptr;
    AudioData* audio;
    float timePlaying = 0;

public:
    SoundEvent() = default;
    friend class SoundEngine;
};

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


class SoundEngine
{
public:
    SoundEngine();
    ~SoundEngine();

    void loadFile(const std::wstring& fileName);
    void add(const std::string& id);
    void update(float deltaTime);

private:
    XAudio2SoundEngine* engine;
    std::map<std::string, AudioData*> soundCollection;
    std::vector<SoundEvent*> playList;
};

