#include "Particle.hlsli"

float4 main(GS_OUT psIn) : SV_TARGET
{    
    float3 color = psIn.color.rgb;
    
    return float4(color, 1.0f);
    return float4(color * 3.0f, 1.0f);

}