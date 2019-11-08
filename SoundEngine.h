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
#define SAMPLE_RATE 11025

enum class SoundType
{
    Music,
    Effect
};

struct AudioData
{
    WAVEFORMATEX waveFormat;
    unsigned int waveLength = 0;
    std::vector<BYTE> data;
    XAUDIO2_BUFFER audioBuffer;
    double length = 0;
    SoundType soundType = SoundType::Effect;
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

    void loadFile(const std::wstring& fileName, SoundType st);
    int add(const std::string& id, bool loop = false);
    void update(float deltaTime);
    void forceStop(unsigned char channel);

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

