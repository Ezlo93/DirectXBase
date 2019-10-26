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


SoundEngine::~SoundEngine()
{
    MFShutdown();
    masterVoice->DestroyVoice();
    soundMain->StopEngine();

    SDelete(masterVoice);
    SDelete(soundMain);
    SDelete(srcReaderConfig);
}
