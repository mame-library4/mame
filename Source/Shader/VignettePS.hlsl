#include "PostProcess.hlsli"

Texture2D sceneMap : register(t0);
SamplerState samplerState : register(s1);

cbuffer VignetteCosntants : register(b0)
{
    float4 vignetteColor_;
    float2 vignetteCenter_;
    float  vignetteIntensity_;
    float  vignetteSmoothness_;

    float vignetteRounded_;
    float vignetteRoundness_;
    float vignetteOpacity_;
    float vignetteDummy_;
};

float4 main(PSIn psIn) : SV_TARGET
{
    float2 sceneMapSize;
    sceneMap.GetDimensions(sceneMapSize.x, sceneMapSize.y);
    
    float4 color = sceneMap.Sample(samplerState, psIn.texcoord);
    
    // ü•ÓŒ¸Œõˆ—
    float2 d = abs(psIn.texcoord - vignetteCenter_) * vignetteIntensity_;
    // Œ¸Œõ‚ğƒXƒNƒŠ[ƒ“‚É‡‚í‚·‚©‚Ç‚¤‚©
    d.x *= lerp(1.0f, sceneMapSize.x / sceneMapSize.y, vignetteRounded_);
    // ‹÷‚Ì”Z‚³
    d = pow(saturate(d), vignetteRoundness_);
    half vignetteFactor = pow(saturate(1.0f - dot(d, d)), vignetteSmoothness_);
    color.rgb *= lerp(vignetteColor_.rgb, (float3) 1.0f, vignetteFactor);
	
    return color;
}