#include "RockSmokeParticle.hlsli"

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
    
    p.position_ = float3(vx, vy, vz) * 10.0f;
    
    // 一枚のテクスチャを四分割する
    // texPos, texSize
    float2 texcoord[4][2] =
    {        
        { float2(0.0f, 0.0f), float2(0.5f, 0.5f) }, // 左上
        { float2(0.0f, 0.5f), float2(0.5f, 0.5f) }, // 左下
        { float2(0.5f, 0.0f), float2(0.5f, 0.5f) }, // 右上
        { float2(0.5f, 0.5f), float2(0.5f, 0.5f) }, // 右下
    };
    
    int texId = id % 4;
    p.texPos_ = texcoord[texId][0];
    p.texSize_ = texcoord[texId][1];
    
    p.size_ = 0.5f;
    
    particleBuffer[id] = p;
}
