#include "TailParticle.hlsli"

struct ParticleData
{
    float4  color_;
    float3  position_;
    float3  velocity_;
    float   size_;
    float   createTimer_;
    float   life_;
    int     state_;
};