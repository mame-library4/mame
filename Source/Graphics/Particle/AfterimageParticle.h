#pragma once
#include "ParticleSystem.h"
#include <vector>

class AfterimageParticle : public ParticleSystem
{
public:
    AfterimageParticle();
    ~AfterimageParticle() override {}

    void Update(const float& elapsedTime)     override;
    void Render()                             override;
    void DrawDebug()                          override;

    void Play();
    void UpdateJointPosition(const std::vector<DirectX::XMFLOAT3>& jointPosition);

private:
    struct Particle
    {
        DirectX::XMFLOAT4   color_      = { 1, 1, 1, 1 };
        DirectX::XMFLOAT3   position_   = {};
        float               size_       = 0.0f;
        float                 age_        = 0.0f;
        int                 state_      = 0;
        
        DirectX::XMFLOAT2 dummy_ = {};
    };
    static const int maxJointNum_ = 2;
    struct Constants
    {
        DirectX::XMFLOAT4 jointPosition_[maxJointNum_] = {};

        float deltaTime_ = 0.0f;

        DirectX::XMFLOAT3 dummy_ = {};
    };
    Constants constants_;

    const int cbSlot_ = 2;
    const int csUAVSlot_ = 0;
    const int gsSRVSlot_ = 0;
};

