#pragma once

#include <wrl.h>
#include <memory>
#include <xaudio2.h>

#include "Audio.h"


enum class BGM
{
    Title,      // �^�C�g��
    Game,       // �Q�[��

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

    void LoadAudio(); // ���y�ǂݍ���

    // BGM�Đ�
    void PlayBGM(
        const BGM& bgm, 
        const bool isLoop = true, 
        const bool isIgnoreQueue = false
    );    

    // SE�Đ�
    void PlaySE(
        const SE& se, 
        const bool isLoop = false, 
        const bool isIgnoreQueue = false
    );

    void StopBGM(const BGM& bgm);                   // BGM��~
    void StopSE(const SE& se);                      // SE��~

    void StopAllBGM();                              // �SBGM��~
    void StopAllSE();                               // �SSE��~
    void StopAllAudio();                            // �S���y��~

public:
    std::unique_ptr<Audio>& GetBGM(const BGM& bgm); //�@BGM�擾
    std::unique_ptr<Audio>& GetSE(const SE& se);    //  SE�擾

public:
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
    IXAudio2MasteringVoice*          masterVoice_ = nullptr;

private:
    std::unique_ptr<Audio> bgm_[static_cast<int>(BGM::Max)] = {};
    std::unique_ptr<Audio> se_[static_cast<int>(SE::Max)]= {};


public:// ������Đ����邽�߂ɂ�����
    void PlaySE(SE_NAME who, SE startIndex, SE endIndex);

private:// ������Đ����邽�߂ɂ�����
    int countIndex[static_cast<UINT>(SE_NAME::Max)] = {};
};

