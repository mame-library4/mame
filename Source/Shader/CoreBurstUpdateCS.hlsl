#include "CoreBurstParticle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;

    ParticleData p = particleBuffer[id];
    
    if(p.state_ == 0)
    {
        p.velocity_ += float3(0.0, -9.8, 0.0) * deltaTime_;
        p.position_ += p.velocity_ * deltaTime_;
        
        if(p.position_.y <= 0.0)
        {
            p.position_.y = 0.0;
            p.state_ = 1;
        }
    }
    else
    {
        if (p.color_.a == 0.0)
        {
            p.position_.y = -1;
        }
        else
        {
            p.color_.a -= 1.5 * deltaTime_;
            p.color_.a = max(p.color_.a, 0.0f);
        }
    }   

    particleBuffer[id] = p;
}
