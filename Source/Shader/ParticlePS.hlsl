#include "Particle.hlsli"

float4 main(GS_OUT psIn) : SV_TARGET
{    
    float4 color = psIn.color;
    
    return color;
}