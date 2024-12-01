#pragma once
#include "Audio.h"
#include <wrl.h>
#include <memory>
#include <xaudio2.h>
#include <vector>

enum class BGM
{
    Title,              // É^ÉCÉgÉã
    Game,               // ÉQÅ[ÉÄ
    GameClear,
    GameClearJingle,

    Max,
};

enum class SE
{
    Press,
    Select,

    Put,
    Bomb,

    Dash,
    JustDodge,
    Slow,
    RushAttackMove0,
    RushAttackMove1,

    SowrdSlash0,
    SowrdSlash1,
    SowrdSlash2,

    Mikiri,

    Attack0,

    Damage,

    // ---------- Dragon ----------
    // ----- SlamAttack -----
    Explosion0,
    Explosion1,
    Charge0,    
    Breath0,
    // ----- TurnAttack -----
    Turn0,
    TailCharge,
    // ----- GuardAttack -----
    Guard,
    GuardAttack,
    // ----- TackleAttack -----
    FootSteps,
    // ----- SuperNova -----
    Flap0,
    Wind0,
    Wind1,
    Fire0,
    Explosion2,
    Explosion3,
    // ----- Roar -----
    Roar,
    // ----- Down -----
    Down,

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

    void LoadAudio(); // âπäyì«Ç›çûÇ›

    // BGMçƒê∂
    void PlayBGM(
        const BGM& bgm, 
        const bool isLoop = true, 
        const bool isIgnoreQueue = false
    );    

    const int PlaySE(const SE& se); // SEçƒê∂

    void StopBGM(const BGM& bgm);                   // BGMí‚é~
    void StopSE(const SE& se, const int& num);      // SEí‚é~

    void StopAllBGM();                              // ëSBGMí‚é~
    void StopAllSE();                               // ëSSEí‚é~
    void StopAllAudio();                            // ëSâπäyí‚é~

    void SetBGMVolume(const BGM& bgm, const float& volume);

public:
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
    IXAudio2MasteringVoice*          masterVoice_ = nullptr;

private:
    std::unique_ptr<Audio> bgm_[static_cast<int>(BGM::Max)] = {};

    struct SEData
    {
    public:
        SEData(IXAudio2* xaudio2, const wchar_t* filename, const int& loadNum = 1);
        [[nodiscard]] const int Play();
        void Stop(const int& num);
        void AllStop();
        void Volume(const float& volume);

    private:
        std::vector<std::unique_ptr<Audio>> se_;
        int     counter_    = 0;
        float   volume_     = 0.0f;
    };
    std::vector<SEData> se_;
};

