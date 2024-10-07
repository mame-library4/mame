#include "Particle.hlsli"

Texture2D textureMap : register(t0);
SamplerState samplerStates[3] : register(s0);

float4 main(GS_OUT psIn) : SV_TARGET
{
    float4 color = textureMap.Sample(samplerStates[1], psIn.texcoord);
    
    float alpha = min(color.a, psIn.color.a);
    
    float3 finalColor = psIn.color.rgb * 2.0f;
    
    return float4(finalColor, alpha);
}