#include "GltfModel.hlsli"

Texture2D noiseTexture : register(t9);
SamplerState samplerState : register(s1);

cbuffer AquaBulletConstants : register(b9)
{
    float3  baseColor_;
    float   rimThreshold_;    
    float3  rimColor_;
    float   rimAmount_;    
};

float4 main(VS_OUT psIn) : SV_TARGET
{
    float3 N = normalize(psIn.wNormal.xyz);
    float3 L = normalize(-lightDirection.xyz);
    float3 V = normalize(cameraPosition.xyz - psIn.wPosition.xyz);
    float3 H = normalize(L + V);
    float NoL = max(0, dot(N, L));
    float NoH = max(0, dot(N, H));
    float NoV = max(0, dot(N, V));
    float irradiance = smoothstep(0.0, 0.01, NoL);
    
    float noise = noiseTexture.Sample(samplerState, psIn.texcoord).r;
    
    float3 ambient = baseColor_.xyz * lerp(0.25, 0, NoV);
    ambient = lerp(0.4 * ambient, 0, irradiance);
    
    float3 diffuse = irradiance * baseColor_.xyz * lerp(0.25, 0, NoV);
    
    float specularIntensity = pow(NoH, 0.0f);
    float specularIntensitySmooth = smoothstep(0.005, 0.01, specularIntensity);
    float3 specular = specularIntensitySmooth * baseColor_.xyz;
    
    float rimDot = 1 - NoV;
    float rimIntensity = rimDot * pow(NoL, rimThreshold_);
    rimIntensity = smoothstep(rimAmount_ - 0.1, rimAmount_ + 0.1, rimIntensity);
    float3 rim = rimColor_.rgb * rimIntensity;   
        
    return float4((baseColor_.rgb * noise) + baseColor_.rgb + ambient + diffuse + specular + rim, 1.0f);
}