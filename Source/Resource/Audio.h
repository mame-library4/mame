#pragma once

#include <xaudio2.h>
#include <mmreg.h>

#include "../Other/misc.h"

class Audio
{
private:
    WAVEFORMATEXTENSIBLE wfx         = { 0 };
    XAUDIO2_BUFFER       buffer      = { 0 };

    IXAudio2SourceVoice* sourceVoice = nullptr;

public:
    Audio(IXAudio2* xaudio2, const wchar_t* filename);
    virtual ~Audio();

    void Play(const int loopCount = 0 /*255 : XAUDIO2_LOOP_INFINITE*/);
    void Play(const bool isLoop = false, const bool isIgnoreQueue = false);
    void Stop(bool playTails = true, size_t afterSamplesPlayed = 0);
    void Volume(float volume);
    bool Queuing();
};

