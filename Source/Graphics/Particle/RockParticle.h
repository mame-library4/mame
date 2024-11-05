#pragma once
#include "ParticleSystem.h"

class RockParticle : public ParticleSystem
{
public:
    RockParticle();
    ~RockParticle() override {}

    void Update(const float& elapsedTime)     override;
    void Render()                             override;
    void DrawDebug()                          override;

    void PlaySmokeParticle();

private:
    struct Particle
    {
        DirectX::XMFLOAT4   color_    = {};
        DirectX::XMFLOAT3   position_ = {};
        DirectX::XMFLOAT2   texPos_   = {};
        DirectX::XMFLOAT2   texSize_  = {};
        float size_ = 0.0f;
        float age_ = 0.0f;

        float dummy_[3] = {};
    };
    struct Constants
    {
        float deltaTime_ = 0.0f;
        DirectX::XMFLOAT3 dummy_ = {};
    };

    Constants constants_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;

    const int cbSlot_ = 2;
    const int csUAVSlot_ = 0;
    const int gsSRVSlot_ = 0;
};

