#define NUMTHREADS_X 16

struct ParticleData
{
    float4 color_;
    float3 position_;
    float3 velocity_;
    float age_;
    int state_;
};