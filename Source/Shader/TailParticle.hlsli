#define NUMTHREADS_X 16

#define maxJointNum 6
cbuffer TailParticleConstants : register(b2)
{
    float4 jointPosition_[maxJointNum];
    float4 particleColor_;
    float3 direction_;
    float particleSize_;
    float time_;
    float deltaTime_;
    float speed_;    
    float radius_;
    float height_;
    float tailTrailTimer_;
};