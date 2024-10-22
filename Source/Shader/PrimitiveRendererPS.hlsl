#include "PrimitiveRenderer.hlsli"

Texture2D textureMap : register(t0);
SamplerState samplerStates[3] : register(s0);

float4 main(PSIn psIn) : SV_TARGET
{
    float4 color = psIn.color;
    color.rgb *= 3.0f;
    
    return psIn.color;
}