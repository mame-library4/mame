#include "Sprite.hlsli"

Texture2D colorMap : register(t0);
Texture2D noiseMap : register(t1);
SamplerState samplerState : register(s1);

float4 main(PSIn psIn) : SV_TARGET
{
    float4 color = colorMap.Sample(samplerState, psIn.texcoord_) * psIn.color_;
    float noise = noiseMap.Sample(samplerState, psIn.scrollTexcoord_).r;
    
    float alpha = step(threshold_, noise);
    //color.a *= alpha;
    color.rgb *= noise;
    //color.rgb *= alpha;
    
    return color;
}