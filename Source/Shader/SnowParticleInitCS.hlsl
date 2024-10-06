#include "SnowParticle.hlsli"
#include "Particle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);

float rand(float n)
{
    return frac(sin(n) * 43758.5453123);
}

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    
    ParticleData particle = particleBuffer[id];
    
    const float noise_scale = 1.0;
    float f0 = rand(id * noise_scale);
    float f1 = rand(f0 * noise_scale);
    float f2 = rand(f1 * noise_scale);
    
    particle.position_ = emitterPosition;
    
    particle.velocity_.x = 0.5 * sin(2 * 3.14 * f0);
    particle.velocity_.y = 2.0 * f1;
    particle.velocity_.z = 0.5 * cos(2 * 3.14 * f0);
    
    particle.color_ = float4(1, 0, 0, 1);
    
    particle.size_.xy = 0.2f;
    
    particleBuffer[id] = particle;
}