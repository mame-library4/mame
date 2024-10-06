#include "SnowParticle.hlsli"
#include "Particle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    
    ParticleData particle = particleBuffer[id];
    
    const float g = -0.5f;
    
    particle.velocity_.y += g * deltaTime;
    particle.position_ += particle.velocity_ * deltaTime;
    
    particleBuffer[id] = particle;
}