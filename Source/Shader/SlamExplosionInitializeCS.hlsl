#include "SlamAttackParticle.hlsli"
#include "SlamExplosionParticle.hlsli"

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
    
    float ringHeight = Rand(id) * 1.5;
    float angle = Rand(id + 1.0) * 6.28318;
    float baseSpeed = 3.0;
    float radiusFactor = Rand(id + 2.0) + 1.;
    
    float3 direction = float3(cos(angle) * radiusFactor, ringHeight, sin(angle * radiusFactor));
    
    float adjustSpeed = baseSpeed * radiusFactor;
    
    p.velocity_ = normalize(direction) * adjustSpeed;
    p.position_ = emitterPosition_ + float3(0.0, ringHeight, 0.0);
    
    p.color_ = float4(color_.rgb, 1.0);
    
    float maxSize = 0.03;
    float size = pow(0.1, radiusFactor);
    size = min(size, maxSize);
    
    p.size_ = size;
    p.state_ = 0;
    
    particleBuffer[id] = p;
}
