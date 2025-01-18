#pragma once
#include "Projectile.h"
#include "ComputeParticle/ComputeParticleEmitter.h"

class RainClouds : public Projectile
{
public:
    RainClouds();
    ~RainClouds() override {}

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override {}
    void DrawDebug()                                    override;
    [[nodiscard]] const bool OnHit(const DirectX::XMFLOAT3& hitPosition)    override { return true; }

private:
    float lifeTimer_ = 10.0f;

    // ---------- Effect ----------
    ComputeParticleEmitter  rainCloudsEffectEmitter_    = {};
    ComputeParticleEmitter  rainEffectEmitter_          = {};
    DirectX::XMFLOAT3       effectEmitPosition_         = {};
    float                   effectCreateTime_           = 0.1f;
    float                   effectCreateTimer_          = 0.0f;
};

