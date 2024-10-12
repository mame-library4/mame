#define NUMTHREADS_X 16

struct ParticleData
{
    float4 color_;
    float3 position_;
    float3 velocity_;
    float age_;
    int state_;
};

cbuffer ParticleConstants : register(b10)
{
    float3 emitterPosition_;
    float particleSize_;
    float time_;
    float deltaTime_;
    float speed_;
};