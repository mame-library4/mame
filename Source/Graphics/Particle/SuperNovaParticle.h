#pragma once
#include "ParticleSystem.h"

class SuperNovaParticle : public ParticleSystem
{
private:
#pragma region ----- 定数 -----
    struct Particle
    {
        DirectX::XMFLOAT4   color_      = { 1, 1, 1, 1 };
        DirectX::XMFLOAT3   position_   = {};
        DirectX::XMFLOAT3   velocity_   = {};
        float               size_       = 0.0f;
        float               age_        = 0.0f;
        int                 state_      = 0;

        DirectX::XMFLOAT3 dummy_ = {};
    };
    struct ChargeParticle
    {
        DirectX::XMFLOAT4   color_          = {};
        DirectX::XMFLOAT3   position_       = {};
        DirectX::XMFLOAT3   initPosition_   = {};
        DirectX::XMFLOAT3   offsetPosition_ = {};
        DirectX::XMFLOAT3   velocity_       = {};
        float               size_           = 0.0f;
        float               angle_ = 0.0f;
        
        int state_ = 0;
        float               dummy_ = {};
    };
    struct Constants
    {
        DirectX::XMFLOAT3 emitterPosition_  = {};
        float             deltaTime_        = 0.0f;
        
        // ----- チャージ用 -----
        DirectX::XMFLOAT3 chargeParticleCenter_ = { 0, 4, 0 };
        float             radius_               = 1.0f;
        DirectX::XMFLOAT3 rotationAxis_         = { 0, 1, 0 };
        float             rotationSpeed_        = 2.0f;


        float coreBurstParticleSpeed_   = 50.0f;
        float lavaCrawlerParticleSpeed_ = 10.0f;

        int state_ = 0;
        int removeState_ = 0;

        float fadeOutSpeed_ = 1.0f;
        DirectX::XMFLOAT3 dummy_ = {};
    };
#pragma endregion ----- 定数 -----

public:
    SuperNovaParticle();
    ~SuperNovaParticle() override {}

    void Update(const float& elapsedTime)     override;
    void Render()                             override;
    void DrawDebug()                          override;

    void PlayLavaCrawlerParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition); // 地面を這うパーティクル
    void PlayCoreBurstParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition);   // メインの爆発パーティクル

    void PlayChargeParticle(const DirectX::XMFLOAT3& emitterPosition);

    void SetLavaCrawlerParticleSpeed(const float& speed) { constants_.lavaCrawlerParticleSpeed_ = speed; }

    void SetChargeParticleRadius(const float& radius) { constants_.radius_ = radius; }

    void Remove(const float& lifeTime, const float& speed);

private:
    ParticleData lavaCrawlerParticle_;
    ParticleData chargeParticle_;

    Constants constants_;

    const int cbSlot_ = 2;
    const int csSlot_ = 0;
    const int gsSlot_ = 0;

    float   lifeTimer_ = 30.0f;  // 生存時間
    bool    isRemove_ = false;  // 消去準備フラグ
};

