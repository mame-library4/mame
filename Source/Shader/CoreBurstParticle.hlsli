cbuffer ParticleConstants : register(b10)
{
    float3 emitterPosition_;
    float particleSize_;
    float time_;
    float deltaTime_;
    float speed_;
};