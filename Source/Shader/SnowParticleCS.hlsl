#include "SnowParticle.hlsli"
#include "Particle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    
    ParticleData particle = particleBuffer[id];
    
    //particle.position_ = particle.velocity_ * particle.age_ * 1.0f;
    particle.age_ -= deltaTime;
    
    //particle.size_ = float2(100, 100);
    //particle.color_ = float4(1, 0, 0, 1);
    //particle.color_ = particleColor;
    
    particleBuffer[id] = particle;
}