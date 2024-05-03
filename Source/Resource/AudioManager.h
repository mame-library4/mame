#pragma once

#include <wrl.h>
#include <memory>
#include <xaudio2.h>

#include "Audio.h"


enum class BGM
{
    Title,      // タイトルBGM
    Result,     // リザルトBGM
    Death,      // 死亡BGM
    Enviroment, // 環境音
    Game,       // Game
    Golem,      // ゴーレム
    BGM_Max,
};

enum class SE
{
    GolemEntry,     // ゴーレム登場
    GolemRoar,      // ゴーレム咆哮
    WaveBegin,      // ウェーブ開始音
    
    CardSelect0,    // 能力取った時の音
    CardSelect1,    // 能力取った時の音
    CardSelect2,    // 能力取った時の音
    CardSelect3,    // 能力取った時の音
    
    Enter,          // 選択
    
    SwordSlash1_0,    // 攻撃１
    SwordSlash1_1,    // 攻撃１
    SwordSlash1_2,    // 攻撃１
    SwordSlash1_3,    // 攻撃１
    SwordSlash1_4,    // 攻撃１
    SwordSlash1_5,    // 攻撃１
    SwordSlash1_6,    // 攻撃１

    SwordSlash2_0,    // 攻撃２
    SwordSlash2_1,    // 攻撃２

    SwordSlash3,    // 攻撃３

    SwordSlash4_0,    // 攻撃４
    SwordSlash4_1,    // 攻撃４


    GolemWalk0,     // ゴーレム歩き
    GolemWalk1,     // ゴーレム歩き
    GolemWalk2,     // ゴーレム歩き
    GolemWalk3,     // ゴーレム歩き
    GolemWalk4,     // ゴーレム歩き

    Hit_0, // ヒット音
    Hit_1, // ヒット音
    Hit_2, // ヒット音
    Hit_3, // ヒット音
    Hit_4, // ヒット音
    Hit_5, // ヒット音
    Hit_6, // ヒット音
    Hit_7, // ヒット音
    Hit_8, // ヒット音
    Hit_9, // ヒット音
    
    SlowMotion_0, // slowMotion
    SlowMotion_1, // slowMotion

    CounterBegin_0, // カウンター
    CounterBegin_1, // カウンター
    CounterBegin_2, // カウンター
    CounterBegin_3, // カウンター

    BlackHole_0, // ブラックホール
    BlackHole_1, // ブラックホール
    BlackHole_2, // ブラックホール

    Laser_0,
    Laser_1,

    PlayerDash,

    GolemPanchi_0, // ゴーレム殴り
    GolemPanchi_1, // ゴーレム殴り
    GolemPanchi_2, // ゴーレム殴り
    GolemPanchi_3, // ゴーレム殴り
    GolemPanchi_4, // ゴーレム殴り

    GolemEntry_0, // 
    GolemEntry_1, // 
    GolemEntry_2, // 



    SE_Max,
};

enum class SE_NAME
{
    CardSelect,
    GolemWalk,
    SwordSlash1,
    SwordSlash2,
    SwordSlash3,
    SwordSlash4,
    Hit,
    SlowMotion,
    CounterBegin,
    BlackHole,
    Laser,
    PlayerDash,

    GolemPanchi,
    GolemEntry,

    Max,
};

class AudioManager
{
private:
    AudioManager()  = default;
    ~AudioManager() = default;

public:
    static AudioManager& Instance()
    {
        static AudioManager instance;
        return instance;
    }

    void LoadAudio();                               // 音楽読み込み

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
    // XAUDIO2
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2     = nullptr;
    IXAudio2MasteringVoice*          masterVoice = nullptr;

private:
    std::unique_ptr<Audio> bgm_[static_cast<int>(BGM::BGM_Max)] = {};
    std::unique_ptr<Audio> se_[static_cast<int>(SE::SE_Max)]    = {};


public:// 何回も再生するためにいるやつ

    void PlaySE(SE_NAME who, SE startIndex, SE endIndex);

private:// 何回も再生するためにいるやつ
    int countIndex[static_cast<UINT>(SE_NAME::Max)] = {};
};

