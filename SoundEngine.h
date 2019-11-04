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

#define MAX_CHANNELS 32

struct AudioData
{
    WAVEFORMATEX waveFormat;
    unsigned int waveLength;
    std::vector<BYTE> data;
    XAUDIO2_BUFFER audioBuffer;
};


class SoundChannel
{
private:
    IXAudio2SourceVoice* srcVoice = nullptr;
    AudioData* audio = nullptr;
    float timePlaying = 0;
    bool available = true;
    bool isPlaying = false;
public:
    SoundChannel() = default;
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

    void Init();
    void loadFile(const std::wstring& file, std::vector<BYTE>& data, WAVEFORMATEX** formatEx, unsigned int& length);

    /*collection*/
    std::map<std::string, AudioData*> soundCollection;
    std::vector<SoundChannel*> channels;

    /*xaudio2*/
    IXAudio2* soundMain;
    IXAudio2MasteringVoice* masterVoice;

    /*wmf*/
    IMFAttributes* srcReaderConfig;
};

