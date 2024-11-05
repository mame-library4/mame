#include "SuperNovaChargeParticle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);

float Rand(float n)
{
    return frac(sin(n) * 43758.5453123);
}

uint Hash(uint x)
{
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;

    ParticleData p = particleBuffer[id];
    
    float theta = (Hash(id * 2) % 10000) / 10000.0f * 6.28318f; // 0 ~ 2*PI
    float phi = (Hash(id * 2 + 1) % 10000) / 10000.0f * 3.14159f; // 0 ~ PI
    
    // ãÖñ ç¿ïWånÇ≈ÇÃà íuÇåvéZ
    float x = sin(phi) * cos(theta);
    float y = cos(phi);
    float z = sin(phi) * sin(theta);
    
    p.position_ = float3(0, 0, 0);
    p.initPosition_ = float3(x, y, z);
    p.offsetPosition_ = emitterPosition_;
    
    p.size_ = 0.05f;
    p.color_ = float4(1.0, 0.42, 0.13, 1);
    p.state_ = 0;
    
    particleBuffer[id] = p;
}
