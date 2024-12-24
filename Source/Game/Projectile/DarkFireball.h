#pragma once
#include "Projectile.h"
#include "ComputeParticle/ComputeParticleEmitter.h"

class DarkFireball : public Projectile
{
public:
    DarkFireball();
    ~DarkFireball() override {}

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override;
    void DrawDebug()                                    override;
    void OnHit()                                        override;

    // ----- ”­ŽË -----
    void Launch(const DirectX::XMFLOAT3& emitPosition, const DirectX::XMFLOAT3& moveDirection,
        const DirectX::XMFLOAT3& targetPosition, const float& moveSpeed, const float& changeTime);

private:
    DirectX::XMFLOAT3   targetPosition_ = {};
    DirectX::XMFLOAT3   moveDirection_  = {};
    float               moveSpeed_      = 0.0f;

    int     state_              = 0;
    float   stateChangeTimer_   = 0.0f;

    bool isLaunched_ = false;

    float deleteRange_ = 5.0f;
    
    ComputeParticleEmitter computeParticleEmitter_ = {};
};

