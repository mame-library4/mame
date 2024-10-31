#pragma once
#include "ParticleSystem.h"

class SuperNovaParticle : public ParticleSystem
{
private:
#pragma region ----- 定数 -----
    struct Particle
    {
        DirectX::XMFLOAT4 color_ = { 1, 1, 1, 1 };
        DirectX::XMFLOAT3 position_ = {};
        DirectX::XMFLOAT3 velocity_ = {};
        float age_ = 0.0f;
        int state_ = 0;
    };
    struct Constants
    {
        DirectX::XMFLOAT3 emitterPosition_ = { 0, 2, 0 };
        float particleSize_ = 0.02f;
        float time_ = 0.0f;
        float deltaTime_ = 0.0f;
        float speed_ = 1.0f;
        float dummy_ = 0.0f;
    };

    enum class CSShaderSlot
    {
        CoreBurstParticle,   // メインの爆発パーティクル
        LavaCrawlerParticle, // 地面を這うパーティクル
    };
    enum class CBSlot
    {
        CoreBurstParticle = 10, // メインの爆発パーティクル
        LavaCrawlerParticle = 11, // 地面を這うパーティクル
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

    void SetLavaCrawlerParticleSpeed(const float& speed) { lavaCrawlerParticleConstants_.speed_ = speed; }

private:
    ParticleData lavaCrawlerParticle_;

    Constants constants_;
    Constants lavaCrawlerParticleConstants_;
};

