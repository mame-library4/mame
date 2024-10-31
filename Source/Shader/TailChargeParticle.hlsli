#define NUMTHREADS_X 16

struct ParticleData
{
    float4 color_;
    float3 position_;
    float3 velocity_;
    int    tailIndex_;
};

static const int maxTailNum = 5;
cbuffer TailChargeParticleConstants : register(b2)
{
    float4 tailPosition_[maxTailNum];
    float4 particleColor_;
    float particleSize_;
    float time_;
    float deltaTime_;
    float speed_;
}