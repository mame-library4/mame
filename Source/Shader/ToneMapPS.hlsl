#include "PostProcess.hlsli"

Texture2D textureMap : register(t0);
SamplerState samplerstate : register(s0);

static const float gamma = 2.3f;

float3 LinerToneMapping(float3 color, float exposure)
{
    color = clamp(exposure * color, 0.0f, 1.0f);
    color = pow(color, 1.0f / gamma);
    return color;
}
float3 SimpleReinhardToneMapping(float3 color, float exposure)
{
    color *= exposure / (1.0f + color / exposure);
    color = pow(color, 1.0f / gamma);
    return color;
}
float3 LumabasedReinhardToneMapping(float3 color)
{
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float toneMappedLuma = luma / (1.0f + luma);
    color *= toneMappedLuma / luma;
    color = pow(color, 1.0f / gamma);
    return color;
}
float3 WhitePreservingLumabasedReinhardToneMapping(float3 color)
{
    float white = 3.0f;
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float toneMappedLuma = luma * (1.0f + luma / (white * white)) / (1.0f + luma);
    color *= toneMappedLuma / luma;
    color = pow(color, 1.0f / gamma);
    return color;
}
float3 RomBinDaHouseToneMapping(float3 color)
{
    color = exp(-1.0f / (2.72f * color + 0.15f));
    color = pow(color, 1.0f / gamma);
    return color;
}
float3 FilmicToneMapping(float3 color)
{
    color = max(0.0f, color - 0.004f);
    color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f) + 0.06f);
    return color;
}
float3 Uncharted2ToneMapping(float3 color, float exposure)
{
    color = max(0.0f, color);
    
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;

    color *= exposure;
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    color /= white;
    color = pow(color, 1.0f / gamma);
    return color;
}

float4 main(PSIn psIn) : SV_TARGET
{
    float4 sampledColor = textureMap.Sample(samplerstate, psIn.texcoord);
    float3 fragmentColor = sampledColor.rgb;
    float alpha = sampledColor.a;
    
    //fragmentColor = WhitePreservingLumabasedReinhardToneMapping(fragmentColor);
    //fragmentColor = Uncharted2ToneMapping(fragmentColor, exposure_);
    fragmentColor = LinerToneMapping(fragmentColor, exposure_);
    
    return float4(fragmentColor * colorize_, alpha);
}