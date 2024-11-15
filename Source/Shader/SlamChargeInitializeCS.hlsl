#include "SlamAttackParticle.hlsli"
#include "SlamChargeParticle.hlsli"

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
    
    float3 vec = float3(0, 0, 0);
    
    vec.x = sin(phi) * cos(theta);
    vec.y = cos(phi);
    vec.z = sin(phi) * sin(theta);
    
    
    float speed = Rand(f1) * Rand(f0) * 30.0f;
    speed = clamp(speed, 5.0f, 20.0f);
    
    p.velocity_ = normalize(p.velocity_ - vec) * speed;
    
    p.size_ = size_;
    p.color_ = float4(color_.rgb, 0);
    p.offsetPosition_ = vec * 3.0f;
    if (speed > 15.0f)
    {
        p.size_ = 0.07;
        p.color_ = float4(1.0, 0.8, 0.5, 0);
        p.offsetPosition_ = vec * 8.0f;
    }
    
    
    p.age_ = id % 20 * 0.01f;
    p.state_ = 0;
    
    particleBuffer[id] = p;
}
