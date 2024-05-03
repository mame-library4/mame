#include "SpriteDissolve.hlsli"

SamplerState samplerState : register(s0);
Texture2D colorMap : register(t0);
Texture2D noiseMap : register(t3);

float4 main(PSIn psIn) : SV_TARGET
{
    float4 color = colorMap.Sample(samplerState, psIn.texcoord) * psIn.color;
	
    float noise = noiseMap.Sample(samplerState, psIn.texcoord).r;
	
    float alpha = step(parameters.x, noise);
    color.a *= alpha;
	
    return color;

}