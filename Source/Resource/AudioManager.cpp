#include "AudioManager.h"
#include "Misc.h"

// ----- �R���X�g���N�^ -----
AudioManager::AudioManager()
{
    HRESULT result = S_OK;
    
    result = XAudio2Create(xAudio2_.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    result = xAudio2_->CreateMasteringVoice(&masterVoice_);
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
}

// ----- BGM,SE �ǂݍ��� -----
void AudioManager::LoadAudio()
{
    // ���y�ǂݍ���
    {
        // BGM�ǂݍ���
        {            
            //bgm_[static_cast<int>(BGM::Title)       ] = std::make_unique<Audio>(xAudio2_.Get(), L"./resources/audio/BGM/title.wav");
        }

        // SE�ǂݍ���
        {
            //se_[static_cast<int>(SE::GolemEntry)    ] = std::make_unique<Audio>(xAudio2_.Get(), L"./resources/audio/SE/GolemEntry.wav");
        }
    }

    // ���ʒ���
    {    
        // BGM���ʒ���
        {
            //bgm_[static_cast<int>(BGM::Title)       ]->Volume(0.35f);
        }

        // SE���ʒ���
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
    // �Đ�������SE�̎��
    int currentIndex = countIndex[static_cast<UINT>(who)];

    // �Đ�����SE�̔z��擪
    int StartIndex = static_cast<UINT>(startIndex);

    // ������Đ�����SE�̏ꏊ���o��
    int playIndex = StartIndex + currentIndex;

    // SE�Đ�
    se_[playIndex]->Play(false);

    // ������SE�z��̍Ō��
    int EndIndex = static_cast<UINT>(endIndex);

    // ������SE�̐����擾
    int IndexCount = EndIndex - StartIndex;

    // ���ɍĐ�����ԍ��ɐݒ肷��
    if (currentIndex >= IndexCount)
    {// �ő�܂ōs������ŏ��ɖ߂�
        countIndex[static_cast<UINT>(who)] = 0;
    }
    else
    {// �܂���������ꍇ�͎���ݒ肷��
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
