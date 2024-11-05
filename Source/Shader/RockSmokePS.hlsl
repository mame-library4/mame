#include "Particle.hlsli"
Texture2D textureMap : register(t0);
SamplerState samplerState : register(s1);

float4 main(GS_OUT psIn) : SV_TARGET
{
    float4 color = textureMap.Sample(samplerState, psIn.texcoord);
    
    return color;
    
    //float3 color = psIn.color.rgb;
    //float alpha = psIn.color.a;
    //return float4(color * 10.0f, alpha);    
}