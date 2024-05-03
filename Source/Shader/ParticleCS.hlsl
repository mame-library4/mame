#include "Particle.hlsli"

RWStructuredBuffer<Particle> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    
    Particle p = particleBuffer[id];
    
    //if (p.age > 10.0)
    //{
    //}
    //p.position += p.velocity * deltaTime;
    //p.age += deltaTime;
    
    p.position = p.velocity * p.age * 1.0f;
    
    particleBuffer[id] = p;
}