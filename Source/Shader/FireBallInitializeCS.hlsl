#include "FireBallParticle.hlsli"

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

    

    float f0 = frac(sin(id) * 43758.5453123);
    float f1 = frac(sin(id * 1.2345) * 43758.5453123);
    
    float theta = f0 * 2.0 * 3.14159265359;
    float phi = acos(1.0 - 2.0 * f1);
    
    float vx = sin(phi) * cos(theta);
    float vy = cos(phi);
    float vz = sin(phi) * sin(theta);
    
    p.velocity_ = float3(vx, vy, vz);
    
    if(id > 1500)
    {
        p.position_ = emitterPosition_ + (p.velocity_ * 0.5f);
        p.color_ = float4(1, 0, 0, 1);
    }
    else
    {
        p.position_ = emitterPosition_ + (p.velocity_ * 0.8f);
        p.color_ = float4(0.93f, 0.47f, 0, 1);
    }
    
    p.age_ = 1;
    
    particleBuffer[id] = p;
}
