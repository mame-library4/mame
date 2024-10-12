#include "SuperNovaParticle.hlsli"

RWStructuredBuffer<ParticleData> particleBuffer : register(u1);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}