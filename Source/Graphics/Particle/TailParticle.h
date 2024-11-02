#pragma once
#include "ParticleSystem.h"
#include <vector>

class TailParticle : public ParticleSystem
{
public:
    TailParticle();
    ~TailParticle() override {}

    void Update(const float& elapsedTime)     override;
    void Render()                             override;
    void DrawDebug()                          override;

    void PlayTailParticle();
    void PlayChargeParticle();
    void PlayTailTrailParticle();

    // ---------- êKîˆÇÃà íuçXêV ----------
    void UpdateJointPosition(const std::vector<DirectX::XMFLOAT3>& jointPosition);
    void UpdateJointWorldMatrix(const std::vector<DirectX::XMMATRIX>& jointWorldMatrix);

private:
#pragma region ---------- ParticleData ----------
    struct Particle
    {
        DirectX::XMFLOAT4   color_          = {};
        DirectX::XMFLOAT3   position_       = {}; 
        DirectX::XMFLOAT3   randomOffset_   = {}; 
        DirectX::XMFLOAT3   velocity_       = {}; 
        int                 tailIndex_      = 0;  // êKîˆÇÃî‘çÜ
    };
    struct ChargeParticleData
    {
        DirectX::XMFLOAT4   color_ = {};
        DirectX::XMFLOAT3   position_ = {};
        DirectX::XMFLOAT3   velocity_ = {};
        int                 tailIndex_ = 0;  // êKîˆÇÃî‘çÜ
        int dummy_ = 0;
    };
    struct TrailParticle
    {
        DirectX::XMFLOAT4   color_       = {};
        DirectX::XMFLOAT3   position_    = {};
        DirectX::XMFLOAT3   velocity_    = {};
        float               size_        = 0.0f;
        float               createTimer_ = 0.0f;
        float               life_        = 0.0f;
        int                 state_       = 0;
        DirectX::XMFLOAT2   dummy_ = {};
    };

    static const int MaxTailNum_ = 5;
    struct Constants
    {
        DirectX::XMFLOAT4X4 tailWorld_[MaxTailNum_] = {};
        DirectX::XMFLOAT4 tailPosition_[MaxTailNum_] = {};
        DirectX::XMFLOAT4 particleColor_ = { 1, 1, 1, 1 };
        DirectX::XMFLOAT3 direction_;
        float             particleSize_ = 0.02f;

        float               time_ = 0.0f;
        float               deltaTime_ = 0.0f;
        float               speed_ = 1.0f;

        float radius_ = 1.0f;
        float height_ = 3.0f;

        float dummy_[3] = {};
    };

#pragma endregion ---------- ParticleData ----------

    Constants           constants_;

    const int cbSlot_ = 2;
    const int csUAVSlot_ = 0;
    const int gsSRVSlot_ = 0;


    ParticleData tailChargeParticle_;

    // ----- tailTrail -----
    ParticleData tailTrailParticle_;
};

