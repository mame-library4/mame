#include "SuperNovaParticle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;

    ParticleData p = particleBuffer[id];

    if (p.age_ > 10.0)
    {
        const float g = -0.5;
        p.velocity_.y += g * deltaTime_;
        p.position_ += p.velocity_ * speed_ * deltaTime_;

        if (p.position_.y < 0)
        {
            p.velocity_ = 0;
            p.position_.y = 0;
            
            p.color_.a = 0;
        }
    }
    p.age_ += deltaTime_;

    particleBuffer[id] = p;
}
