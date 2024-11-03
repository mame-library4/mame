#include "TailMainParticle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    ParticleData p = particleBuffer[id];
    
    if (tailParticleState_ == 0)
    {
        float height[4] = { height_.x, height_.y, height_.z, height_.w };
        float radius[4] = { radius_.x, radius_.y, radius_.z, radius_.w };
    
        p.randomOffset_ = GetCylindricalPosition(id, radius[p.tailIndex_], height[p.tailIndex_], direction_[p.tailIndex_].xyz);
    
        p.position_ = jointPosition_[p.tailIndex_ + 1].xyz + p.randomOffset_;
        
        p.velocity_ = p.position_ - jointPosition_[p.tailIndex_ + 1].xyz;
    }
    else
    {
#if 1
        float height[4] = { height_.x, height_.y, height_.z, height_.w };
        float radius[4] = { radius_.x, radius_.y, radius_.z, radius_.w };
    
        p.randomOffset_ = GetCylindricalPosition(id, radius[p.tailIndex_], height[p.tailIndex_], direction_[p.tailIndex_].xyz);
    
        p.position_ = jointPosition_[p.tailIndex_ + 1].xyz + p.randomOffset_;
        
        p.color_.a -= deltaTime_ * 2.0;
        p.color_.a = max(p.color_.a, 0.0);
        
        if (p.color_.a <= 0.0)
        {
            p.position_.y = -1.0;
        }
#else
        if (p.state_ == 0)
        {
            p.velocity_ += float3(0.0, -9.8, 0.0) * deltaTime_;
            p.position_ += p.velocity_ * deltaTime_;
        
            if (p.position_.y <= 0.0)
            {
                p.position_.y = 0.0;
                p.state_ = 1;
            }
        }
        else
        {
            p.color_.a -= deltaTime_ * 2.0;
            p.color_.a = max(p.color_.a, 0.0);
        
            if (p.color_.a <= 0.0)
            {
                p.position_.y = -1.0;
            }
        }
#endif
    }
    
    particleBuffer[id] = p;
}