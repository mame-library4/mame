#define NUMTHREADS_X 16

cbuffer ParticleConstants : register(b2)
{
    float3 emitterPosition_;
    float deltaTime_;

    float3  chargeParticleCenter_;
    float   radius_;
    float3  rotationAxis_;
    float   rotationSpeed_;
    
    float coreBurstParticleSpeed_;
    float lavaCrawlerParticleSpeed_;
    
    int state_;
    int removeState_; // 1‚È‚çRemove‚·‚é
    float fadeOutSpeed_;
    float3 dummy_;
};