#pragma once
#include "ParticleSystem.h"

class FireBallParticle : public ParticleSystem
{
public:
    FireBallParticle();
    ~FireBallParticle() override {}

    void Initialize(const float& elapsedTime) override {}
    void Update(const float& elapsedTime)     override;
    void Render()                             override;
    void DrawDebug()                          override;

    void PlayFireBallParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition);
    void UpdateFireBallParticle(const DirectX::XMFLOAT3& position);

private:
    void CreateFireBallParticle();

private:
    struct ParticleData
    {
        DirectX::XMFLOAT4 color_        = { 1, 1, 1, 1 };
        DirectX::XMFLOAT3 position_     = {};
        DirectX::XMFLOAT3 velocity_     = {};
        float             age_          = 0.0f;
        float             dummy_        = {};
    };
    struct Constants
    {
        DirectX::XMFLOAT3   emitterPosition_ = { 0, 2, 0 };
        DirectX::XMFLOAT3   currentPosition_ = {};
        DirectX::XMFLOAT3   oldPosition_     = {};

        float               particleSize_    = 0.02f;
        float               time_            = 0.0f;
        float               deltaTime_       = 0.0f;
        float               speed_           = 1.0f;
        int                 isMoveStraight   = 1;

        float dummy_[2] = {};
    };
    Constants constants_;

    const int cbSlot_    = 2;
    const int csUAVSlot_ = 0;
    const int gsSRVSlot_ = 0;
};

