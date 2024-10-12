#include "SuperNovaParticle.hlsli"
#include "LavaCrawlerParticle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u1);

float Rand(float n)
{
    return frac(sin(n) * 43758.5453123);
}

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;

    ParticleData p = particleBuffer[id];

    p.position_ = emitterPosition_;
    
    float angle = (id / float(384)) * 6.28318f;
    
    float x = cos(angle);
    float z = sin(angle);
    
    p.velocity_ = float3(x, 0.0, z);

    p.color_ = float4(1.0, 0.42, 0.13, 1);
    //p.color_ = float4(0.94, 0.35, 0.13, 1);
    
    float f0 = Rand(id);
    float f1 = Rand(f0);
    float f2 = Rand(f1);
    p.age_ = 3 * f2;

    particleBuffer[id] = p;
}
