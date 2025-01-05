#pragma once
#include "Projectile.h"
#include "ComputeParticle/ComputeParticleEmitter.h"

class AquaSeeker : public Projectile
{
public:
    AquaSeeker();
    ~AquaSeeker() override {}

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override;
    void DrawDebug()                                    override;
    void OnHit(const DirectX::XMFLOAT3& hitPosition)    override;

public:
    void SetMoveSpeed(const float& speed) { moveSpeed_ = speed; }
    void SetMoveType(const int& moveType);

private:
    ComputeParticleEmitter computeParticleEmitter_ = {};

    DirectX::XMFLOAT3 moveDirection_ = {};


    float moveSpeed_ = 0.0f;
    
    int moveState_ = 0;    
    float timer_ = 0.0f;
};

