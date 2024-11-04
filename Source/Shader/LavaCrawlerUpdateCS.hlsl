#include "LavaCrawlerParticle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;

    ParticleData p = particleBuffer[id];
    
    if (p.age_ > 3)
    {
        p.position_ += p.velocity_ * lavaCrawlerParticleSpeed_ * deltaTime_;
    }
    p.age_ += deltaTime_;

    particleBuffer[id] = p;
}
