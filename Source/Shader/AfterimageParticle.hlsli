#define NUMTHREADS_X 16
#define maxJointNum 2

struct ParticleData
{
    float4 color_;
    float3 position_;
    float size_;
    float age_;
    int state_;
    
    float2 dummy_;
};

cbuffer AfterimageParticleConstants : register(b2)
{
    float4 jointPosition_[maxJointNum];
    float deltaTime_;
    float3 dummy_;
}