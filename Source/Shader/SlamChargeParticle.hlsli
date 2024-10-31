#define NUMTHREADS_X 16

struct ParticleData
{
    float4 color_;
    float3 position_;
    float3 offsetPosition_;
    float3 velocity_;
    float  size_;
    float  age_;
    int    state_;
};

cbuffer Constants : register(b2)
{
    float3 handPosition_;
    float deltaTime_;
    float speed_;
    float size_;
}