#include "AfterimageParticle.hlsli"

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
    
    if(p.state_ == 0)
    {
        p.age_ += deltaTime_;
        if(p.age_ > 1)
        {
            p.position_ = jointPosition_[id % maxJointNum].xyz; //+ float3(Rand(id + 1), Rand(id + 2), Rand(id + 3));
            p.state_ = 1;            
        }
    }
    else
    {
    }
    
    particleBuffer[id] = p;
}