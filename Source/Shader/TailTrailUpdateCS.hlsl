#include "TailTrailParticle.hlsli"

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

    // Generate
    if (p.state_ == 0)
    {
        if (p.createTimer_ < tailTrailTimer_)
        {
            // ¶¬ˆÊ’u‚ð•ªŽU‚³‚¹‚é
            float3 createPosition = jointPosition_[5].xyz;
            createPosition.y = clamp(createPosition.y, 0.0, 1.5);
            
            p.position_ = createPosition + float3(Rand(id + 1), Rand(id + 2), Rand(id + 3));
            
            // ¶¬•ûŒü‚ð‚¢‚¢Š´‚¶‚É•ªŽU
            float3 dragonPosition = float3(jointPosition_[0].x, 0.0, jointPosition_[0].z);
            float3 tail4Position = float3(jointPosition_[4].x, 0.0, jointPosition_[4].z);
            float3 tail5Position = float3(jointPosition_[5].x, 0.0, jointPosition_[5].z);
            
            float3 vec0 = normalize(tail5Position - dragonPosition);
            float3 vec1 = normalize(tail4Position - dragonPosition);
            float3 vec2 = normalize(vec1 - vec0) / 11.0;
            
            float3 vec[10];
            for (int i = 1; i <= 10; ++i)
            {
                vec[i - 1] = vec1 + vec2 * i;
                vec[i - 1].y = 0.0f;
                vec[i - 1] = normalize(vec[i - 1]);
            }
            
            p.velocity_ = vec[id % 10] * p.speed_;
            
            p.state_ = 1;
        }
    }
    // XV
    else if (p.state_ == 1)
    {
        p.velocity_ += float3(0.0, -9.8, 0.0) * deltaTime_;
        p.position_ += p.velocity_ * deltaTime_;
        p.life_ -= deltaTime_;
        if (p.position_.y <= 0.0)
        {
            p.position_.y = 0.0;
            p.velocity_ = float3(0, 0, 0);
        }
        if (p.life_ <= 0.0)
        {
            p.state_ = 2;
        }
    }
    // ™X‚ÉÁ‚µ‚Ä‚¢‚­
    else
    {
        if (p.color_.a == 0.0)
        {
            p.position_.y = -1;
        }
        else
        {
            p.velocity_ += float3(0.0, -9.8, 0.0) * deltaTime_;
            p.position_ += p.velocity_ * deltaTime_;
            p.color_.a -= 1.5 * deltaTime_;
            p.color_.a = max(p.color_.a, 0.0f);
            if (p.position_.y <= 0.0)
            {
                p.position_.y = 0.0;
                p.velocity_ = float3(0, 0, 0);
            }
        }
    }
    
    particleBuffer[id] = p;
}