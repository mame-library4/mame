#pragma once
#include "ParticleSystem.h"
#include "Shader.h"

class SlamAttackParticle : public ParticleSystem
{
public:
    SlamAttackParticle();
    ~SlamAttackParticle() override {}

    void Update(const float& elapsedTime)     override;
    void Render()                             override;
    void DrawDebug()                          override;

    void Remove(); // 削除

    // ----- パーティクル再生 -----
    void PlayChargeParticle(const DirectX::XMFLOAT3& handPosition);
    void PlayExplosionParticle(const DirectX::XMFLOAT3& emitterPosition);

    void UpdateHandPosition(const DirectX::XMFLOAT3& handPosition);

    [[nodiscard]] const bool GetIsChargeParticleActive() const { return chargeParticleData_.GetIsActive(); }

private:
#pragma region ---------- ParticleData ----------
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

    // ---------- ExplosionParticle ---------- 
    struct ExplosionParticle
    {
        DirectX::XMFLOAT4   color_      = {};
        DirectX::XMFLOAT3   position_   = {};
        DirectX::XMFLOAT3   velocity_   = {};
        float               size_       = 0.0f;
        int                 state_      = 0;
    };

    struct Constants
    {
        DirectX::XMFLOAT3   handPosition_       = {};
        float               deltaTime_          = 0.0f;
        DirectX::XMFLOAT3   emitterPosition_    = {};
        float               speed_              = 15.0f;
        float               size_               = 0.04f; 
        DirectX::XMFLOAT3   dummy_        = {};
    };

#pragma endregion ---------- ParticleData ----------


    const int csSlot_ = 0;
    const int cbSlot_ = 2;
    const int gsSlot_ = 0;

    float easingTimer_ = 0.0f;

    // ----- チャージパーティクル -----
    ParticleData            chargeParticleData_;

    // ----- 爆発パーティクル -----
    ParticleData            explosionParticle_;
    
    Constants               constants_;

    float lifeTimer_ = 5.0f;

    // ----- ポイントライト -----
    Shader::PointLights pointLights_ = {};

    // ----- 削除用 -----
    bool isRemove_ = false;
};

