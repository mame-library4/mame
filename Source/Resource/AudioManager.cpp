#include "AudioManager.h"
#include "Misc.h"

// ----- コンストラクタ -----
AudioManager::AudioManager()
{
    HRESULT result = S_OK;
    
    result = XAudio2Create(xAudio2_.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    result = xAudio2_->CreateMasteringVoice(&masterVoice_);
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
}

// ----- BGM,SE 読み込み -----
void AudioManager::LoadAudio()
{
    // 音楽読み込み
    {
        // BGM読み込み
        {            
            //bgm_[static_cast<int>(BGM::Title)       ] = std::make_unique<Audio>(xAudio2_.Get(), L"./resources/audio/BGM/title.wav");
        }

        // SE読み込み
        {
            //se_[static_cast<int>(SE::GolemEntry)    ] = std::make_unique<Audio>(xAudio2_.Get(), L"./resources/audio/SE/GolemEntry.wav");
        }
    }

    // 音量調整
    {    
        // BGM音量調整
        {
            //bgm_[static_cast<int>(BGM::Title)       ]->Volume(0.35f);
        }

        // SE音量調整
        {
            //se_[static_cast<int>(SE::Enter)         ]->Volume(1.5f);
        }
    }

}


void AudioManager::PlayBGM(const BGM& bgm, const bool isLoop, const bool isIgnoreQueue)
{
    bgm_[static_cast<int>(bgm)]->Play(isLoop, isIgnoreQueue);
}

void AudioManager::PlaySE(const SE& se, const bool isLoop, const bool isIgnoreQueue)
{
    se_[static_cast<int>(se)]->Play(isLoop, isIgnoreQueue);
}


void AudioManager::StopBGM(const BGM& bgm)
{
    bgm_[static_cast<int>(bgm)]->Stop();
}

void AudioManager::StopSE(const SE& se)
{
    se_[static_cast<int>(se)]->Stop();
}


std::unique_ptr<Audio>& AudioManager::GetBGM(const BGM& bgmIndex)
{
    return bgm_[static_cast<int>(bgmIndex)];
}

std::unique_ptr<Audio>& AudioManager::GetSE(const SE& seIndex)
{
    return se_[static_cast<int>(seIndex)];
}


void AudioManager::PlaySE(SE_NAME who, SE startIndex, SE endIndex)
{
    // 再生数するSEの種類
    int currentIndex = countIndex[static_cast<UINT>(who)];

    // 再生するSEの配列先頭
    int StartIndex = static_cast<UINT>(startIndex);

    // 今から再生するSEの場所を出す
    int playIndex = StartIndex + currentIndex;

    // SE再生
    se_[playIndex]->Play(false);

    // 自分のSE配列の最後尾
    int EndIndex = static_cast<UINT>(endIndex);

    // 自分のSEの数を取得
    int IndexCount = EndIndex - StartIndex;

    // 次に再生する番号に設定する
    if (currentIndex >= IndexCount)
    {// 最大まで行ったら最初に戻す
        countIndex[static_cast<UINT>(who)] = 0;
    }
    else
    {// まだ次がある場合は次を設定する
        ++currentIndex;
        countIndex[static_cast<UINT>(who)] = currentIndex;
    }

}

void AudioManager::StopAllBGM()
{
    for (std::unique_ptr<Audio>& bgm :bgm_)
    {
        bgm->Stop();
    }
}

void AudioManager::StopAllSE()
{
    for (std::unique_ptr<Audio>& se : se_)
    {
        se->Stop();
    }
}

void AudioManager::StopAllAudio()
{
    for (std::unique_ptr<Audio>& bgm : bgm_)
    {
        if (bgm == nullptr) continue;

        bgm->Stop();
    }

    for (std::unique_ptr<Audio>& se : se_)
    {
        if (se == nullptr) continue;

        se->Stop();
    }
}
