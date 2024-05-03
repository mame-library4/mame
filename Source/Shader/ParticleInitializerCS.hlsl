#include "Particle.hlsli"

RWStructuredBuffer<Particle> particleBuffer : register(u0);

float rand(float n)
{
    return frac(sin(n) * 43758.5453123);
}

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    
    Particle p = particleBuffer[id];
    
    const float noiseScale = 1.0;
    float f0 = rand(id * noiseScale);
    float f1 = rand(f0 * noiseScale);
    float f2 = rand(f1 * noiseScale);
    
    p.position = emitterPosition;
    
    p.velocity.x = 1.5 * sin(2 * 3.14 * f0);
    p.velocity.y = 2.0 * f1;
    p.velocity.z = 1.5 * cos(2 * 3.14 * f0);
    
    
    
    
    p.color.xyz = particleColor.rgb;
    p.color.w = 1.0f;
    
    p.age = 100.0 * f2;
    //p.age = 10.0 * f2;
    p.state = 0;
    
    particleBuffer[id] = p;
}