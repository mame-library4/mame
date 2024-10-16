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

        if (p.position_.y < -1.0)
        {
            p.velocity_ = 0;
            p.position_.y = -1.0;
            
            p.color_.a = 0;
        }
    }
    

    particleBuffer[id] = p;
}
