#include "FireBallParticle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;

    ParticleData p = particleBuffer[id];
    
    // íºêi
    if (isMoveStraight_ == 1)
    {
        float3 vec = currentPosition_ - oldPosition_;
        const float len = length(vec);
        p.position_ += vec;
    }
    else
    {
        const float g = -0.5;
        p.velocity_.y += g * deltaTime_;
        p.position_ += p.velocity_ * speed_ * deltaTime_;

        p.age_ -= 4.0f * deltaTime_;
        
        if(p.age_ < 0.0)
        {
            p.color_.a -= 4.0f * deltaTime_;
            p.color_.a = max(p.color_.a, 0.0);
        }
        
        if(p.color_.a == 0.0f)
        {
            p.position_ = float3(0, -1, 0);
            p.velocity_ = 0.0f;
        }
    }
    

    particleBuffer[id] = p;
}
