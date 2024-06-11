#include "PostProcess.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4
Texture2D textureMap : register(t0);
SamplerState samplerStates[5] : register(s0);

float4 main(PSIn psIn) : SV_TARGET
{    
    //float4 color = textureMap.Sample(samplerStates[LINEAR_BORDER_BLACK], psIn.texcoord);
    float4 color = textureMap.Sample(samplerStates[LINEAR], psIn.texcoord);
    
    //color.rgb = mul(color.rgb, 1.5f);
    //color.rgb = mul(color.rgb, 2.2f);
    //color.rgb = color.rgb *  2.2f;
    //color.rgb = color.rgb *  1/2.2f;
	
    return color;
}