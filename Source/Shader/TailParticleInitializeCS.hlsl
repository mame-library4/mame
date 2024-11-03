#include "TailMainParticle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    ParticleData p = particleBuffer[id];
    
    p.tailIndex_ = id % 4;
    
    float height[4] = { height_.x, height_.y, height_.z, height_.w };
    float radius[4] = { radius_.x, radius_.y, radius_.z, radius_.w };
    
    float3 initialPosition = GetCylindricalPosition(id, radius[p.tailIndex_], height[p.tailIndex_], direction_[p.tailIndex_].xyz);
    p.randomOffset_ = initialPosition;
    
    p.size_ = 0.05f;
    p.color_ = float4(1.0, 0.42, 0.13, 1);
    p.state_ = 0;
    
    particleBuffer[id] = p;
}