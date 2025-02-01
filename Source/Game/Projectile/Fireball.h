#pragma once
#include "Projectile.h"
#include "ComputeParticle/ComputeParticleEmitter.h"

class Fireball : public Projectile
{
public:
    Fireball();
    ~Fireball() override {}

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override;
    void DrawDebug()                                    override;
    [[nodiscard]] const bool OnHit(const DirectX::XMFLOAT3& hitPosition)    override;

    void Launch(const DirectX::XMFLOAT3& emitPosition, const DirectX::XMFLOAT3& moveDirection, const float& moveSpeed = 0.0f);

private:
    struct AquaConstants
    {
        DirectX::XMFLOAT3   baseColor_ = { 0.4f, 0.0f, 0.0f };
        float               rimThreshold_ = 0.0f;
        DirectX::XMFLOAT3   rimColor_ = { 1.0f, 0.0f, 0.0f };
        float               rimAmount_ = 0.5f;
    };
    std::unique_ptr<ConstantBuffer<AquaConstants>>      aquaConstants_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>           aquaPS_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    shaderResourceView_;

    // ----- Effect -----
    ComputeParticleEmitter computeParticleEmitter_ = {};
    ComputeParticleEmitter hitEffect0Emitter_ = {};
    ComputeParticleEmitter hitEffect1Emitter_ = {};
    
    float trailEffectTimer_ = 0.0f;

    int     hitEffect0EmitNum_       = 3;
    int     hitEffect0EmitCounter_   = 0;
    float   hitEffect0EmitTime_      = 1.0f / 60.0f;
    float   hitEffect0EmitTimer_     = hitEffect0EmitTime_;

    int     hitEffect1EmitNum_      = 3;
    int     hitEffect1EmitCounter_  = 0;
    float   hitEffect1EmitTime_     = 0.15f;
    float   hitEffect1EmitTimer_    = hitEffect1EmitTime_;


    DirectX::XMFLOAT3   moveDirection_  = {};
    float               moveSpeed_      = 0.0f;


    float lifeTimer_ = 0.0f;
};

