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
        DirectX::XMFLOAT3 position_     = {}; // �ʒu
        DirectX::XMFLOAT2 size_         = {}; // �傫��

        DirectX::XMFLOAT4 color_        = {}; // �F

        DirectX::XMFLOAT3 velocity_     = {}; // ���x
        DirectX::XMFLOAT3 acceleration_ = {}; // �����x
        float             age_          = 0.0f;
    };
    struct Constants
    {
        DirectX::XMFLOAT3 emitterPosition_ = {}; // �����ʒu
        DirectX::XMFLOAT4 color_ = {};
        float deltaTime_ = 0.0f;

        DirectX::XMFLOAT4 dummy_ = {};
    };
    Constants constants_;
};

