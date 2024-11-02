#include "SlamAttackParticle.hlsli"
#include "SlamChargeParticle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    ParticleData p = particleBuffer[id];
        
    if (p.age_ > 0.3f)
    {
        p.color_.a = 1;
        p.offsetPosition_ += p.velocity_ * deltaTime_;
        
        if (length(p.offsetPosition_) < 0.4f)
        {
            p.state_ = 1;
        }
    }
    
    p.age_ += deltaTime_;
        
    if(p.state_ == 1)
    {
        p.size_ = 0;

    }
    
    p.position_ = handPosition_ + p.offsetPosition_;
    
    
    particleBuffer[id] = p;
}