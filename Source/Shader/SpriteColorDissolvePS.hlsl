#include "SpriteDissolve.hlsli"

Texture2D colorMap : register(t0);
SamplerState colorSamplerState : register(s0);
Texture2D maskTexture : register(t1);

float4 main(PSIn psIn) : SV_TARGET
{
    float4 color = colorMap.Sample(colorSamplerState, psIn.texcoord) * psIn.color;
    
    float mask = maskTexture.Sample(colorSamplerState, psIn.texcoord).r;
    
    float dissolve = step(parameters.x, mask);
    
    color.r *= dissolve;
    color.gb = 0;
    
    return color;
}