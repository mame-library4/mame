#define NUMTHREADS_X 16

#define maxJointNum 6
cbuffer TailParticleConstants : register(b2)
{
    float4 jointPosition_[maxJointNum];
    float4 direction_[4];
    float4 height_;
    float4 radius_;
    
    float deltaTime_;
    float tailTrailTimer_;
    int tailParticleState_;
    int dummy_;
};