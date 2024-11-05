#define NUMTHREADS_X 16

struct ParticleData
{
    float4 color_;
    float3 position_;
    float2 texPos_;
    float2 texSize_;
    float size_;
    float age_;
    
    float3 dummy_;
};

cbuffer ParticleConstants : register(b2)
{
    float deltaTime_;
    
    float3 dummy_;
};