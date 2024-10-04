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