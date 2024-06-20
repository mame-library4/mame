#include "Particle.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerState[3] : register(s0);

Texture2D textureMap : register(t0);

float4 main(GS_OUT psIn) : SV_TARGET
{
    //return float4(1, 1, 1, 1);
    
    float4 color = textureMap.Sample(samplerState[POINT], psIn.texcoord);
    
    return color;
    //return float4(color.rgb * (psIn.color.rgb * 10.2), color.a);
}