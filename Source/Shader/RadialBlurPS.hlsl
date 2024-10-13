#include "PostProcess.hlsli"

Texture2D colorMap : register(t0);
SamplerState samplerState : register(s1);

cbuffer RadialBlurConstants : register(b0)
{
    float2 uvOffset_;
    float strength_;
    float sampleCount_;
};

float4 main(PSIn psIn) : SV_TARGET
{
    float3 color = float3(0.0f, 0.0f, 0.0f);
    
    float2 uv = psIn.texcoord - uvOffset_;
    
    float factor = strength_ / sampleCount_ * length(uv);
    
    for (int i = 0; i < sampleCount_;++i)
    {
        float offset = 1 - factor * i;
        color += colorMap.Sample(samplerState, uv * offset + uvOffset_).rgb;
    }
    
    return float4(color / sampleCount_, 1.0f);
}