#include "Particle.hlsli"

float4 main(GS_OUT psIn) : SV_TARGET
{
    float3 color = psIn.color.rgb * 3.0f;
    
    return float4(color, 1.0f);
}