#pragma once
#include "Projectile.h"
#include "Particle/RockParticle.h"

class Rock : public Projectile
{
public:
    Rock(const DirectX::XMFLOAT3& createPosition, const DirectX::XMFLOAT3& targetPosition);
    ~Rock() override {}

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override;
    void DrawDebug()                                    override;
    void OnHit()                                        override;

private:
    RockParticle* rockParticle_ = nullptr;

    DirectX::XMFLOAT3 createPosition_ = {};
    DirectX::XMFLOAT3 targetPosition_ = {};

    float lerpTimer_ = 0.0f;
    float lerpSpeed_ = 1.0f;
};

