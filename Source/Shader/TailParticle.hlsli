#define NUMTHREADS_X 16

struct ParticleData
{
    float4 color_;
    float3 position_;
    float3 randomOffset_;
    float3 velocity_;
    int    tailIndex_;
};

#define MaxTailNum 5
cbuffer TailParticleConstants : register(b2)
{
    float4x4 tailWorld_[MaxTailNum];
    float4 tailPosition_[MaxTailNum];
    float4 particleColor_;
    
    float3 direction_;
    
    float particleSize_;
    float radius_;
    float height_;
};