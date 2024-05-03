// BLOOM
#include "FullScreenQuad.hlsli"
#include "PostEffect.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4
#define COMPARISON_LINEAR_BORDER_WHITE 5
#define LINEAR_BORDER_OPAQUE_BLACK 6
#define POINT_CLAMP 7
SamplerState samplerStates[8] : register(s0);
Texture2D textureMaps[5] : register(t0);

// égÇ§Ç©Ç«Ç§Ç©ÇÃÉtÉâÉO
#define RGB_SHIFT 0
#define NOISE 0
#define SCAN_LINE 0
#define VIGNETTE 1
#define SHAKE 0

float3 reinhardToneMapping(float3 color)
{
    float luma = dot(color, float3(0.2126, 0.7152, 0.0722));
    float toneMappedLuma = luma / (1. + luma);
    color *= toneMappedLuma / luma;
    return color;
}


float4 main(PSIn psIn) : SV_TARGET
{    
    float4 color = textureMaps[0].Sample(samplerStates[POINT], psIn.texcoord);
    float4 bloom = textureMaps[1].Sample(samplerStates[POINT], psIn.texcoord);
    float4 fog = textureMaps[2].Sample(samplerStates[LINEAR_BORDER_BLACK], psIn.texcoord);
    
    // RGBÇ∏ÇÁÇµ
    {
#if RGB_SHIFT
        float2 samplePoint = psIn.texcoord;
        samplePoint.x += shiftSize.x;
        color.r = textureMaps[0].Sample(samplerStates[POINT], samplePoint).r;
        samplePoint = psIn.texcoord;
        samplePoint.x -= shiftSize.x;
        color.b = textureMaps[0].Sample(samplerStates[POINT], samplePoint).b;
        samplePoint = psIn.texcoord;
        samplePoint.y += shiftSize.y;
        color.g = textureMaps[0].Sample(samplerStates[POINT], samplePoint).g;
#endif // RGB_SHIFT
    }
    
    // Noise
    {
#if NOISE
        float2 samplePoint = psIn.texcoord;
        float noise = WhiteNoise(psIn.texcoord * noiseTimer) - 0.5;
        color.rgb += float3(noise, noise, noise) * noiseColor.rgb;
#endif // NOISE
    }
    
    // ScanLine
    {
#if SCAN_LINE
        float2 samplePoint = psIn.texcoord;
        float sinv = sin(psIn.texcoord.y * 2 + scanLineTimer * -0.1);
        float steped = step(0.99, sinv * sinv);
        color.rgb -= (1 - steped) * abs(sin(psIn.texcoord.y * 50.0 + scanLineTimer * 1.0)) * 0.05;
        color.rgb -= (1 - steped) * abs(sin(psIn.texcoord.y * 100.0 - scanLineTimer * 2.0)) * 0.08;
        color.rgb += steped * 0.1;        
#endif // SCAN_LINE
    }
    
    // vignette
    {
#if VIGNETTE
        float2 samplePoint = psIn.texcoord;
        float vignette = length(float2(0.5, 0.5) - psIn.texcoord);
        vignette = clamp(vignette - vignetteValue, 0, 1);
        //vignette = clamp(vignette - 0.2, 0, 1);
        color.rgb -= vignette;        
#endif // VIGNETTE
    }
    
    // shake
    {
#if SHAKE
        const float shakeLength = 0.1;  // óhÇÍÇÃîgÇÃêî
        const float shakeWidth = 0.01;  // óhÇÍÇÃïù
        const float speed = 5.0;        // óhÇÍÇÃÉXÉsÅ[Éh
        
        float offsetX = sin(psIn.texcoord.x * shakeLength + noiseTimer * speed) * shakeWidth;
        float offsetY = cos(psIn.texcoord.y * shakeLength + noiseTimer * speed) * shakeWidth;
        
        float4 shakeColor = textureMaps[0].Sample(samplerStates[POINT], float2(psIn.texcoord.x + offsetX, psIn.texcoord.y + offsetY));
        color += shakeColor;
#endif // SHAKE
    }
    
    
    
#if 0
    const float GAMMA = 1.0 / 2.2;
    bloom = pow(bloom, GAMMA);
#endif

    
    float3 fragmentColor = color.rgb + bloom.rgb + fog.rgb;
    float alpha = color.a;

    // Tone map
    fragmentColor = reinhardToneMapping(fragmentColor);
    
    // ÉKÉìÉ}ï‚ê≥
#if 0
    const float INV_GAMMA = 1.0 / 2.2;
    fragmentColor = pow(fragmentColor, INV_GAMMA);
#endif
    
    return float4(fragmentColor, alpha);
}