#include "SuperNovaParticle.hlsli"
#include "LavaCrawlerParticle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u1);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;

    ParticleData p = particleBuffer[id];
    
    if (p.age_ > 3.0)
    {
        p.position_ += p.velocity_ * speed_ * deltaTime_;
    }
    p.age_ += deltaTime_;

    particleBuffer[id] = p;
}
