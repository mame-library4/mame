#include "TailTrailParticle.hlsli"

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
    
    p.color_ = float4(1.0, 0.42, 0.13, 1);
    p.position_ = float3(0, -1, 0);
    p.velocity_ = float3(0, 0, 0);
    
    p.speed_ = (id % 6 + 5);
    p.size_ = min(0.01f, Rand(id) * 0.05f);
    p.createTimer_ = id * 0.00013;
    p.life_ = 0.5f;
    p.state_ = 0;
    p.jointIndex_ = id % 5;
    
    particleBuffer[id] = p;
}
