#include "Particle.hlsli"

Texture2D colorMap : register(t0);
SamplerState samplerState : register(s0);

float4 main(GS_OUT psIn) : SV_TARGET
{    
    
    return float4(psIn.color);
    
    float4 color = colorMap.Sample(samplerState, psIn.texcoord);
    
    
    return float4(psIn.color.rgb, psIn.color.a * color.a);

}