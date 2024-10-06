#pragma once
#include "ParticleSystem.h"

class SnowParticle : public ParticleSystem
{
public:
    SnowParticle();
    ~SnowParticle() override {}

    void Initialize(const float& deltaTime) override;
    void Update(const float& deltaTime)     override;
    void Render()                           override;
    void DrawDebug()                        override;

private:
    struct ParticleData
    {
        DirectX::XMFLOAT3 position_     = {}; // 位置
        DirectX::XMFLOAT2 size_         = {}; // 大きさ

        DirectX::XMFLOAT4 color_        = {}; // 色

        DirectX::XMFLOAT3 velocity_     = {}; // 速度
        DirectX::XMFLOAT3 acceleration_ = {}; // 加速度
        float             age_          = 0.0f;
    };
    struct Constants
    {
        DirectX::XMFLOAT3 emitterPosition_ = {}; // 発生位置
        DirectX::XMFLOAT4 color_ = {};
        float deltaTime_ = 0.0f;

        DirectX::XMFLOAT4 dummy_ = {};
    };
    Constants constants_;
};

