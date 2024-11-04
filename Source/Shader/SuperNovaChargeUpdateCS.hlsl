#include "SuperNovaChargeParticle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);

float Rand(float n)
{
    return frac(sin(n) * 43758.5453123);
}

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;

    ParticleData p = particleBuffer[id];

    p.position_ = p.initPosition_ * radius_;
    
    // 球の中心からのベクトル
    float3 toCenter = normalize(p.position_ - chargeParticleCenter_);

    // 回転行列
    p.angle_ += rotationSpeed_ * deltaTime_;
    float c = cos(p.angle_);
    float s = sin(p.angle_); 
    
    float3x3 rotationMatrix = float3x3(
        c + rotationAxis_.x * rotationAxis_.x * (1 - c),
        rotationAxis_.x * rotationAxis_.y * (1 - c) - rotationAxis_.z * s,
        rotationAxis_.x * rotationAxis_.z * (1 - c) + rotationAxis_.y * s,

        rotationAxis_.y * rotationAxis_.x * (1 - c) + rotationAxis_.z * s,
        c + rotationAxis_.y * rotationAxis_.y * (1 - c),
        rotationAxis_.y * rotationAxis_.z * (1 - c) - rotationAxis_.x * s,

        rotationAxis_.z * rotationAxis_.x * (1 - c) - rotationAxis_.y * s,
        rotationAxis_.z * rotationAxis_.y * (1 - c) + rotationAxis_.x * s,
        c + rotationAxis_.z * rotationAxis_.z * (1 - c)
    );

    // パーティクルの位置を回転
    p.position_ = mul(rotationMatrix, toCenter * length(p.position_ - chargeParticleCenter_)) + chargeParticleCenter_;
    
    p.position_ += p.offsetPosition_;
    
    particleBuffer[id] = p;
}
