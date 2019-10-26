#include "SoundEngine.h"
#include "util.h"

void SoundEngine::Init()
{
    HRESULT hr;
    //xaudio2 init
    hr = XAudio2Create(&soundMain);
    hr = soundMain->CreateMasteringVoice(&masterVoice);

    //wmf init
    hr = MFStartup(MF_VERSION);
}


SoundEngine::~SoundEngine()
{
    MFShutdown();
    masterVoice->DestroyVoice();
    soundMain->StopEngine();

    SDelete(masterVoice);
    SDelete(soundMain);
}
