#include "TailParticle.hlsli"

struct ParticleData
{
    float4 color_;
    float3 position_;
    float3 velocity_;
    float size_;
    int    tailIndex_;
};