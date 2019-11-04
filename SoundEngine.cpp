#include "SoundEngine.h"
#include "util.h"

void SoundEngine::Init()
{
    //xaudio2 init
     XAudio2Create(&soundMain);
     soundMain->CreateMasteringVoice(&masterVoice);

    //wmf init
     MFStartup(MF_VERSION);

     MFCreateAttributes(&srcReaderConfig, 1);
     srcReaderConfig->SetUINT32(MF_LOW_LATENCY, true);

     //master volume
     masterVoice->SetVolume(0.7f);

}

void SoundEngine::loadFile(const std::wstring& file, std::vector<BYTE>& data, WAVEFORMATEX** formatEx, unsigned int& length)
{

    DWORD streamIndex = (DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM;

    /*open audio file*/
    IMFSourceReader* reader;
     MFCreateSourceReaderFromURL(file.c_str(), srcReaderConfig, &reader);
     reader->SetStreamSelection(streamIndex, true);

    /*get data type*/
    IMFMediaType* mediaType;
     reader->GetNativeMediaType(streamIndex, 0, &mediaType);

    /*check compressed or uncompressed*/
    GUID subType{};
     mediaType->GetGUID(MF_MT_MAJOR_TYPE, &subType);

    if (subType == MFAudioFormat_Float || subType == MFAudioFormat_PCM)
    {
        //uncompressed
    }
    else
    {
        //compressed
        IMFMediaType* partType = nullptr;
         MFCreateMediaType(&partType);

         partType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
         partType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
         reader->SetCurrentMediaType(streamIndex, NULL, partType);

    }

     IMFMediaType* uncompressedType = nullptr;
     reader->GetCurrentMediaType(streamIndex, &uncompressedType);
     MFCreateWaveFormatExFromMFMediaType(uncompressedType, formatEx, &length);
     reader->SetStreamSelection(streamIndex, true);

    //copy data
    IMFSample* sample = nullptr;
    IMFMediaBuffer* buffer = nullptr;
    BYTE* localAudioData = nullptr;
    DWORD localAudioDataLength = 0;

    while (true)
    {
        DWORD flags = 0;
         reader->ReadSample(streamIndex, 0, nullptr, &flags, nullptr, &sample);

        if (flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED || flags & MF_SOURCE_READERF_ENDOFSTREAM)
            break;

        if (sample == nullptr)
            continue;

         sample->ConvertToContiguousBuffer(&buffer);
         buffer->Lock(&localAudioData, nullptr, &localAudioDataLength);

        for (size_t i = 0; i < localAudioDataLength; i++)
            data.push_back(localAudioData[i]);

         buffer->Unlock();
        localAudioData = nullptr;
    }

    return;
}


void SoundEngine::loadFile(const std::wstring& fileName)
{
    AudioData* data = new AudioData();
    WAVEFORMATEX* wfx;
    loadFile(fileName, data->data, &wfx, data->waveLength);

    char id[128];
    char ext[8];

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &fileName[0], (int)fileName.size(), NULL, 0, NULL, NULL);
    std::string tStr(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &fileName[0], (int)fileName.size(), &tStr[0], size_needed, NULL, NULL);
    _splitpath_s(tStr.c_str(), NULL, 0, NULL, 0, id, 128, ext, 8);

    ZeroMemory(&data->audioBuffer, sizeof(XAUDIO2_BUFFER));
    data->waveFormat = *wfx;
    data->audioBuffer.AudioBytes = (UINT32)data->data.size();
    data->audioBuffer.pAudioData = (BYTE* const)& data->data[0];
    data->audioBuffer.pContext = nullptr;
    DBOUT("Length: " << data->waveLength << "\n");

    soundCollection.insert(std::make_pair(id, data));
}

void SoundEngine::add(const std::string& id)
{
    int usedChannel = -1;

    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        if (channels[i]->available)
        {
            usedChannel = i;
            channels[i]->available = false;
            break;
        }
    }

    channels[usedChannel]->audio = soundCollection[id];
    channels[usedChannel]->timePlaying = 0.f;
    HRESULT hr = soundMain->CreateSourceVoice(&channels[usedChannel]->srcVoice, &channels[usedChannel]->audio->waveFormat);
    if (FAILED(hr))
        std::cerr << "Failed to create Source Voice\n";

}

void SoundEngine::update(float deltaTime)
{
    /*check queue and play if necessary*/

    for (auto& c : channels)
    {
        if (c->available == false)
        {
            if (c->isPlaying == false)
            {
                c->srcVoice->SubmitSourceBuffer(&c->audio->audioBuffer);
                c->srcVoice->Start();
                c->isPlaying = true;
            }
            //check if time is over length
            else
            {

            }
        }
    }

}


SoundEngine::SoundEngine()
{
    Init();
    
    for (int i = 0; i < MAX_CHANNELS; i++)
    {
        channels.push_back(new SoundChannel());
    }
}

SoundEngine::~SoundEngine()
{
    for (auto& i : soundCollection)
    {
        SDelete(i.second);
    }

    MFShutdown();
    masterVoice->DestroyVoice();
    soundMain->StopEngine();

    SDelete(masterVoice);
    SDelete(soundMain);
    SDelete(srcReaderConfig);
}