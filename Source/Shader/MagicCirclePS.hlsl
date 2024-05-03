#include "FbxModel.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps[4] : register(t0);

Texture2D emissiveTexture : register(t16);

float4 main(PSIn psIn) : SV_TARGET
{
    float4 color = textureMaps[0].Sample(samplerStates[0], psIn.texcoord);
    float alpha = color.a;
    
    // ÉKÉìÉ}ï‚ê≥
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, GAMMA);

    float3 N = normalize(psIn.worldNormal.xyz);
#if 1
    float3 T = normalize(psIn.worldTangent.xyz);
    float sigma = psIn.worldTangent.w;
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);

    float4 normal = textureMaps[1].Sample(samplerStates[LINEAR], psIn.texcoord);
    normal = (normal * 2.0) - 1.0;
    N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));
#endif
    
    float3 directionLight = CalcLightFromDirectionLight(psIn);
    float3 finalColor = directionLight;
    
    // ÉÇÉfÉãÇÃìÒéüîΩéÀÇã^éóìIÇ…éZèo
    finalColor.x += 0.2;
    finalColor.y += 0.2;
    finalColor.z += 0.2;
    
    float3 emissive = emissiveTexture.Sample(samplerStates[POINT], psIn.texcoord).rgb;
    finalColor += emissive * emissiveIntensity * emissiveColor.rgb;
    
    return float4(finalColor, alpha) * psIn.color;
}