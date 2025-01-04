#pragma once
#include "Projectile.h"
#include "ConstantBuffer.h"
#include <memory>
#include "ComputeParticle/ComputeParticleEmitter.h"

class AquaBullet : public Projectile
{
public:
    AquaBullet();
    ~AquaBullet() override {}

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override;
    void DrawDebug()                                    override;
    void OnHit(const DirectX::XMFLOAT3& hitPosition)    override;

    // ----- ”­ŽË -----
    void Launch(const DirectX::XMFLOAT3& moveDirection, const float& moveSpeed); 

private:
    struct AquaBulletConstants
    {
        DirectX::XMFLOAT3   baseColor_          = { 0.0f, 0.34f, 0.67f };
        float               rimThreshold_       = 0.0f;
        DirectX::XMFLOAT3   rimColor_           = { 0.5f, 0.5f, 1.0f };
        float               rimAmount_          = 0.5f;
    };
    std::unique_ptr<ConstantBuffer<AquaBulletConstants>> aquaBulletConstants_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>           aquaBulletPS_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    shaderResourceView_;

    ComputeParticleEmitter  aquaBulletTrailEmitter_     = {};
    bool                    isTrailEffectGeneratable_   = false;

    // ----- HitEffect -----
    ComputeParticleEmitter hitEffectEmitter_            = {};


    DirectX::XMFLOAT3   moveDirection_  = {};
    float               moveSpeed_      = 0.0f;

    float               lifeTimer_      = 0.0f;
};

