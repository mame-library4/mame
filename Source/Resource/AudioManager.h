#pragma once

#include <wrl.h>
#include <memory>
#include <xaudio2.h>

#include "Audio.h"


enum class BGM
{
    Title,      // タイトル
    Game,       // ゲーム

    Max,
};

enum class SE
{
    Dummy,

    Max,
};

enum class SE_NAME
{
    Dummy,

    Max,
};

class AudioManager
{
private:
    AudioManager();
    ~AudioManager() = default;

public:
    static AudioManager& Instance()
    {
        static AudioManager instance;
        return instance;
    }

    void LoadAudio(); // 音楽読み込み

    // BGM再生
    void PlayBGM(
        const BGM& bgm, 
        const bool isLoop = true, 
        const bool isIgnoreQueue = false
    );    

    // SE再生
    void PlaySE(
        const SE& se, 
        const bool isLoop = false, 
        const bool isIgnoreQueue = false
    );

    void StopBGM(const BGM& bgm);                   // BGM停止
    void StopSE(const SE& se);                      // SE停止

    void StopAllBGM();                              // 全BGM停止
    void StopAllSE();                               // 全SE停止
    void StopAllAudio();                            // 全音楽停止

public:
    std::unique_ptr<Audio>& GetBGM(const BGM& bgm); //　BGM取得
    std::unique_ptr<Audio>& GetSE(const SE& se);    //  SE取得

public:
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
    IXAudio2MasteringVoice*          masterVoice_ = nullptr;

private:
    std::unique_ptr<Audio> bgm_[static_cast<int>(BGM::Max)] = {};
    std::unique_ptr<Audio> se_[static_cast<int>(SE::Max)]= {};


public:// 何回も再生するためにいるやつ
    void PlaySE(SE_NAME who, SE startIndex, SE endIndex);

private:// 何回も再生するためにいるやつ
    int countIndex[static_cast<UINT>(SE_NAME::Max)] = {};
};

