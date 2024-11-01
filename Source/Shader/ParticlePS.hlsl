#include "Particle.hlsli"

float4 main(GS_OUT psIn) : SV_TARGET
{    
    float3 color = psIn.color.rgb;
    float alpha = psIn.color.a;
    
    return float4(color * 10.0f, alpha);
}