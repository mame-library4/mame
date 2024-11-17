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
            bgm_[static_cast<int>(BGM::Game)] = std::make_unique<Audio>(xAudio2_.Get(), L"./Resources/Audio/BGM/Game.wav");
        }

        // SE読み込み
        {
            // SE::Lockon
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Lockon.wav", 2));

            // SE::Dash, SE::JustDodge, SE::Slow, SE::RushAttackMove
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Player/Move/Dash.wav", 2));
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Player/Move/JustDodge.wav", 2));
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Player/Move/Slow.wav", 2));
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Player/Move/RushAttackMove0.wav", 2));
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Player/Move/RushAttackMove1.wav", 2));

            // SE::SowrdSlash0, SE::SowrdSlash1, SE::SowrdSlash2
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Player/SwordSlash/SwordSlash0.wav", 2));
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Player/SwordSlash/SwordSlash1.wav", 2));
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Player/SwordSlash/SwordSlash2.wav", 10));
            
            // SE::Mikiri
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Player/Attack/Mikiri0.wav", 1));

            // SE::Attack0,
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Player/Attack/Attack2.wav", 10));
            
            // SE::Explosion0, SE::Explosion1
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Dragon/SlamAttack/Explosion0.wav", 2));
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Dragon/SlamAttack/Explosion1.wav", 2));
            
            // SE::Charge0, SE::Breath0
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Dragon/SlamAttack/Charge0.wav", 2));
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Dragon/SlamAttack/Breath0.wav", 2));
            
            // SE::Turn0, SE::TailCharge
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Dragon/TurnAttack/Turn.wav", 1));
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Dragon/TurnAttack/TailCharge.wav", 4));
            
            // SE::Guard, SE::GuardAttack
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Dragon/GuardAttack/Guard.wav", 1));
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Dragon/GuardAttack/GuardAttack.wav", 1));
            
            // SE::FootSteps
            se_.emplace_back(SEData(xAudio2_.Get(), L"./Resources/Audio/SE/Dragon/TackleAttack/FootSteps.wav", 4));
        }
    }

    // 音量調整
    {    
        // BGM音量調整
        {
            bgm_[static_cast<int>(BGM::Game)]->Volume(0.06f);
        }

        // SE音量調整
        {
            se_[static_cast<int>(SE::Dash)].Volume(0.5f);
            se_[static_cast<int>(SE::JustDodge)].Volume(0.6f);
            se_[static_cast<int>(SE::Slow)].Volume(0.5f);
            se_[static_cast<int>(SE::RushAttackMove0)].Volume(1.5f);
            se_[static_cast<int>(SE::RushAttackMove1)].Volume(0.7f);

            se_[static_cast<int>(SE::SowrdSlash0)].Volume(0.3f);
            se_[static_cast<int>(SE::SowrdSlash1)].Volume(0.3f);
            se_[static_cast<int>(SE::SowrdSlash2)].Volume(0.2f);

            se_[static_cast<int>(SE::Attack0)].Volume(0.3f);

            se_[static_cast<int>(SE::Mikiri)].Volume(0.3f);
            
            se_[static_cast<int>(SE::Charge0)].Volume(0.5f);
            se_[static_cast<int>(SE::Breath0)].Volume(0.3f);

            se_[static_cast<int>(SE::Turn0)].Volume(0.4f);
            se_[static_cast<int>(SE::TailCharge)].Volume(0.8f);

            se_[static_cast<int>(SE::Guard)].Volume(1.1f);
            se_[static_cast<int>(SE::GuardAttack)].Volume(0.4f);
            
            se_[static_cast<int>(SE::FootSteps)].Volume(0.2f);
        }
    }

}


void AudioManager::PlayBGM(const BGM& bgm, const bool isLoop, const bool isIgnoreQueue)
{
    bgm_[static_cast<int>(bgm)]->Play(isLoop, isIgnoreQueue);
}

const int AudioManager::PlaySE(const SE& se)
{
    return se_[static_cast<int>(se)].Play();
}


void AudioManager::StopBGM(const BGM& bgm)
{
    bgm_[static_cast<int>(bgm)]->Stop();
}

void AudioManager::StopSE(const SE& se, const int& num)
{
    se_[static_cast<int>(se)].Stop(num);
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
    for (auto& se : se_)
    {
        se.AllStop();
    }
}

void AudioManager::StopAllAudio()
{
    for (std::unique_ptr<Audio>& bgm : bgm_)
    {
        if (bgm == nullptr) continue;

        bgm->Stop();
    }

    StopAllSE();
}

void AudioManager::SetBGMVolume(const BGM& bgm, const float& volume)
{
    bgm_[static_cast<int>(bgm)]->Volume(volume);
}

// ----- SEを必要な数だけロードする -----
AudioManager::SEData::SEData(IXAudio2* xaudio2, const wchar_t* filename, const int& loadNum)
{
    // ロード数が無い
    if (loadNum <= 0) return;

    for (int i = 0; i < loadNum; ++i)
    {
        se_.emplace_back(std::make_unique<Audio>(xaudio2, filename));
    }
}

// ----- 順番に再生する -----
const int AudioManager::SEData::Play()
{
    // SEが無い
    if (se_.size() == 0) return -1;
    if (counter_ >= se_.size()) counter_ = 0;

    se_.at(counter_)->Play(false);

    return counter_++;
}

// ----- 指定の番号を停止する -----
void AudioManager::SEData::Stop(const int& num)
{
    // 0以下の数字は受け付けない
    if (num < 0) return;
    // サイズオーバーも受け付けない
    if (num >= se_.size()) return;

    se_.at(num)->Stop();
}

// ----- 全て停止する -----
void AudioManager::SEData::AllStop()
{
    // そもそもロードしていない
    if (se_.size() == 0) return;

    for (auto& se : se_)
    {
        se->Stop();
    }
}

// ----- 音量設定 -----
void AudioManager::SEData::Volume(const float& volume)
{
    for (auto& se : se_)
    {
        se->Volume(volume);
    }
}
