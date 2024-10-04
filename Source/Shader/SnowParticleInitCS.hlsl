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
    
    
    //float3 pos = float3((rand(deltaTime) % 30 - 15) * 0.1f, rand(deltaTime) % 30 * 0.1f + 1, (rand(deltaTime) % 30 - 15) * 0.1f + 3);
    //int max = 100;
       
    //particle.position_.x = pos.x + (rand(deltaTime) % 10001 - 5000) * 0.01f;
    //particle.position_.y = pos.y;
    //particle.position_.z = pos.z + (rand(deltaTime) % 10001 - 5000) * 0.01f;
    
    //particle.velocity_.y = -(rand(deltaTime) % 10001) * 0.0002f - 0.002f;
    
    //particle.acceleration_.x = (rand(deltaTime) % 10001) * 0.00001f + 0.1f;
    //particle.acceleration_.z = (rand(deltaTime) % 10001 - 5000) * 0.00001f;
    
    
    //particle.color_.xyz = particleColor.rgb;
    //particle.color_.w = 1.0f;
    
    //particle.age_ = 100.0;
    
    particle.color_ = float4(1, 0, 0, 1);
    
    particleBuffer[id] = particle;
}