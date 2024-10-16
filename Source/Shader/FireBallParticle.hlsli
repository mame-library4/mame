#define NUMTHREADS_X 16

struct ParticleData
{
    float4 color_;
    float3 position_;
    float3 velocity_;
    float age_;
    float dummy_;
};

cbuffer ParticleConstants : register(b2)
{
    float3 emitterPosition_;
    float3 currentPosition_;
    float3 oldPosition_;
    float particleSize_;
    float time_;
    float deltaTime_;
    float speed_;
    int isMoveStraight_;
    float2 dummmy_;
};