#include "Particle.hlsli"

float4 main(GS_OUT psIn) : SV_TARGET
{
    return psIn.color;
}