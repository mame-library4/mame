#define NUMTHREADS_X 16

struct ParticleData
{
    float3 position_;
    float2 size_;
    
    float4 color_;  
    float3 velocity_;
    float3 acceleration_;
    float age_;   
};

cbuffer PARTICLE_CONSTANTS : register(b9)
{
    float3 emitterPosition;
    float4 particleColor;
    float deltaTime;
};