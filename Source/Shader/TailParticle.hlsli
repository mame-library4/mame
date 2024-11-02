#define NUMTHREADS_X 16

#define MaxTailNum 5
cbuffer TailParticleConstants : register(b2)
{
    float4x4 tailWorld_[MaxTailNum];
    float4 tailPosition_[MaxTailNum];
    float4 particleColor_;
    float3 direction_;
    float particleSize_;
    float time_;
    float deltaTime_;
    float speed_;    
    float radius_;
    float height_;
};