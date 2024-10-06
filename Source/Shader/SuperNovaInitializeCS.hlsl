#include "SuperNovaParticle.hlsli"

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

    p.position_ = emitterPosition_;
#if 0
    const float noise_scale = 1.0;
    float f0 = Rand(id * noise_scale);
    float f1 = Rand(f0 * noise_scale);
    float f2 = Rand(f1 * noise_scale);


    //p.velocity_.x = Rand(deltaTime_);//0.5 * sin(2 * 3.14 * f0);
    //p.velocity_.z = Rand(deltaTime_);
    
    //p.velocity_.x = 0.5 * sin(2 * 3.14 * f0);
    //p.velocity_.y = 2.0 * f1;
    //p.velocity_.z = 0.5 * cos(2 * 3.14 * f0);
    
    p.velocity_.x = sin(2 * 3.14 * f0);
    p.velocity_.y = 2 * 3.14 * Rand(id);
    p.velocity_.z = cos(2 * 3.14 * f0);
#else
    float f0 = frac(sin(id) * 43758.5453123);
    float f1 = frac(sin(id * 1.2345) * 43758.5453123);
    
    float theta = f0 * 2.0 * 3.14159265359;
    float phi = acos(1.0 - 2.0 * f1);
    
    float vx = sin(phi) * cos(theta);
    float vy = cos(phi);
    float vz = sin(phi) * sin(theta);
    
    p.velocity_ = float3(vx, vy, vz);
    
#endif

    p.color_ = float4(1, 0, 0, 1);

    p.age_ = 10.0;
    //p.age_ = 10.0 * f2;
    p.state_ = 0;

    particleBuffer[id] = p;
}
