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
    float2 texOffset;
    textureMap.GetDimensions(texOffset.x, texOffset.y); 
    texOffset = 1.0f / texOffset;
    
    float4 color[10];
    
    float2 center = float2(0.5f, 0.5f);
    float2 dir = center - psIn.texcoord;
    float len = length(dir);
    dir = normalize(dir) * (psIn.texcoord + texOffset);
    dir *= blurPower * len;

    float addOffset[10] = { 0.19f, 0.17f, 0.15f, 0.13f, 0.11f, 0.09f, 0.07f, 0.05f, 0.03f, 0.01f };
    color[0] = textureMap.Sample(samplerStates[LINEAR], psIn.texcoord) * addOffset[0];
    for (int i = 1; i < 10; ++i)
    {
        color[i] = textureMap.Sample(samplerStates[LINEAR], psIn.texcoord + dir * i) * addOffset[i];
    }
    float4 finalColor = 0;
    for (int j = 0; j < 10; ++j)
    {
        finalColor += color[j];
    }
    
    return finalColor;
}