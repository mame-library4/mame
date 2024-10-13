#include "PrimitiveRenderer.hlsli"

Texture2D textureMap : register(t0);
SamplerState samplerStates[3] : register(s0);

float4 main(PSIn psIn) : SV_TARGET
{
    //return float4(psIn.texcoord, 1, 1);

    return psIn.color;
    
    float4 color = textureMap.Sample(samplerStates[1], psIn.texcoord);
    
    
    //return float4(psIn.color.rgb, color.a);
    
    return color;
    
    return psIn.color * color;
}