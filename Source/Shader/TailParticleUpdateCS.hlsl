#include "TailParticle.hlsli"

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
    
  
    
    p.position_ = tailPosition_[p.tailIndex_].xyz + p.randomOffset_;
    
    p.color_ = particleColor_;
    
    particleBuffer[id] = p;
}