#define NUMTHREADS_X 16

cbuffer Constants : register(b2)
{
    float3 handPosition_;
    float  deltaTime_;
    float3 emitterPosition_;
    float  speed_;
    float  size_;
}