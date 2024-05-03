#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

Texture2D colorMap : register(t0);
SamplerState samplerStates[5] : register(s0);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
{
    uint mipLevel = 0, width, height, numberOfLevels;
    colorMap.GetDimensions(mipLevel, width, height, numberOfLevels);
    
    const float offset[3] = { 0.0f, 1.3846153846f, 3.2307692308f };
    const float weight[3] = { 0.2270270270, 0.3162162162, 0.0702702703 };
	
    float4 color = colorMap.Sample(samplerStates[LINEAR_BORDER_BLACK], texcoord) * weight[0];
    for (int i = 0; i < 3; ++i)
    {
        color += colorMap.Sample(samplerStates[LINEAR_BORDER_BLACK], texcoord + float2(offset[i] / width, 0.0f)) * weight[i];
        color += colorMap.Sample(samplerStates[LINEAR_BORDER_BLACK], texcoord - float2(offset[i] / width, 0.0f)) * weight[i];
    }

    return color;
}