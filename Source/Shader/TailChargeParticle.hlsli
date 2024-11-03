#include "TailParticle.hlsli"

struct ParticleData
{
    float4 color_;
    float3 position_;
    float3 velocity_;
    int    tailIndex_;
    int dummy_;
};