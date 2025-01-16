#pragma once
#include "Projectile.h"
#include "ComputeParticle/ComputeParticleEmitter.h"

class AquaMeteor : public Projectile
{
public:
    AquaMeteor();
    ~AquaMeteor() override {}

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override;
    void DrawDebug()                                    override;
    void OnHit(const DirectX::XMFLOAT3& hitPosition)    override;

    void Launch(const DirectX::XMFLOAT3& emitPosition, const DirectX::XMFLOAT3& direction, const float& speed);

private:
    struct AquaConstants
    {
        DirectX::XMFLOAT3   baseColor_      = { 0.0f, 0.34f, 0.67f };
        float               rimThreshold_   = 0.0f;
        DirectX::XMFLOAT3   rimColor_       = { 0.5f, 0.5f, 1.0f };
        float               rimAmount_      = 0.5f;
    };
    std::unique_ptr<ConstantBuffer<AquaConstants>>      aquaConstants_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>           aquaPS_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    shaderResourceView_;


    float scaleTimer_ = 0.0f;
    float scaleSpeed_ = 0.5f;
    float positionY_ = 3.0f;


    DirectX::XMFLOAT3   moveDirection_  = {};
    float               moveSpeed_      = 0.0f;
    bool                isLaunched_     = false;

    // ---------- Effect ----------
    ComputeParticleEmitter computeParticleEmitter_ = {};
};

