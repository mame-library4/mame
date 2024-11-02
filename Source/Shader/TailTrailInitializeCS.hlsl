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
    
    p.size_ = min(0.01f, Rand(id) * 0.05f);
    p.createTimer_ = Rand(id) * 0.2f;
    p.life_ = 1.0f;
    p.state_ = 0;
    
    particleBuffer[id] = p;
}
