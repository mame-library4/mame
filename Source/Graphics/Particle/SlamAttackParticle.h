#pragma once
#include "ParticleSystem.h"

class SlamAttackParticle : public ParticleSystem
{
public:
    SlamAttackParticle();
    ~SlamAttackParticle() override {}

    void Update(const float& elapsedTime)     override;
    void Render()                             override;
    void DrawDebug()                          override;

    void PlayChargeParticle(const DirectX::XMFLOAT3& handPosition);
    void UpdateHandPosition(const DirectX::XMFLOAT3& handPosition);

    [[nodiscard]] const bool GetIsChargeParticleActive() const { return chargeParticleData_.GetIsActive(); }

private:
    struct Particle
    {
        DirectX::XMFLOAT4 color_    = {};
        DirectX::XMFLOAT4 position_ = {};
    };
    struct ChargeParticle
    {
        DirectX::XMFLOAT4   color_          = {};
        DirectX::XMFLOAT3   position_       = {};
        DirectX::XMFLOAT3   offsetPosition_ = {};
        DirectX::XMFLOAT3   velocity_       = {};
        float               size_       = 0.0f;
        float               age_        = 0.0f;
        int                 state_      = 0.0f;
    };
    struct ChargeParticleConstants
    {
        DirectX::XMFLOAT3 handPosition_ = {};
        float deltaTime_    = 0.0f;
        float speed_        = 15.0f;
        float size_         = 0.04f; 
        DirectX::XMFLOAT2 dummy_ = {};
    };

    ParticleData            chargeParticleData_;
    ChargeParticleConstants chargeParticleConstants_;

    const int csSlot_ = 0;
    const int cbSlot_ = 2;
    const int gsSlot_ = 0;

    float easingTimer_ = 0.0f;
};

