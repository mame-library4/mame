#include "TailChargeParticle.hlsli"

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
    
    p.position_ = vec * 10.0f;
    
    p.velocity_ = normalize(p.velocity_ - vec);
    
    p.color_ = float4(1, 0, 0, 1);
    
    particleBuffer[id] = p;
}