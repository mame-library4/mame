#pragma once
#include "ParticleSystem.h"

class SuperNovaParticle : public ParticleSystem
{
public:
    SuperNovaParticle(const DirectX::XMFLOAT3& emitterPosition, const float& speed = 1.0f, const float& size = 1.0f);
    ~SuperNovaParticle() override {}

    void Initialize(const float& deltaTime) override;
    void Update(const float& deltaTime)     override;
    void Render()                           override;
    void DrawDebug()                        override;

private:
    struct ParticleData
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
    Constants constants_;

    bool isParticleActive_ = false; // 使用フラグ
};

