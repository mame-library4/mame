#include "AfterimageParticle.hlsli"

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
    
    p.color_ = float4(0.0f, 0.0f, 1.0f, 1.0f);
    p.size_ = 0.05f;
    
    p.position_ = float3(0, 1, 0);
    
    p.age_ = Rand(id);
    //p.age_ = id % 3;
    p.state_ = 0;
    
    particleBuffer[id] = p;
}
