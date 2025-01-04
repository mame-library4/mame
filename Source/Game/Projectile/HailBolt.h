#pragma once
#include "Projectile.h"
#include "ComputeParticle/ComputeParticleEmitter.h"

class HailBolt : public Projectile
{
public:
    HailBolt();
    ~HailBolt() override {}

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override;
    void DrawDebug()                                    override;
    void OnHit(const DirectX::XMFLOAT3& hitPosition)    override;

    void Launch(const DirectX::XMFLOAT3& emitPosition, const DirectX::XMFLOAT3& direction, const float& speed);

private:
    DirectX::XMFLOAT3   moveDirection_  = {};
    float               moveSpeed_      = 0.0f;

    bool isEffectGeneratable_ = true;

    float lifeTimer_ = 0.0f;

    ComputeParticleEmitter computeParticleEmitter_ = {};
    ComputeParticleEmitter hitEffectEmitter_ = {};
};

