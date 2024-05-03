#include "DeferredRendering.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

Texture2D colorMap	: register(t0);
Texture2D metalnessRoughness : register(t1);
Texture2D normalMap : register(t2);
Texture2D EmissiveMap : register(t3);
Texture2D OcclusionMap : register(t4);
Texture2D shadowMap : register(t5);

SamplerState samplerStates[5] : register(s0);
SamplerComparisonState shadowSampler : register(s5);

GBuffer main(PSIn psIn) : SV_TARGET
{
    GBuffer gBuffer;
	
    gBuffer.baseColor = colorMap.Sample(samplerStates[LINEAR], psIn.texcoord);
    
    gBuffer.metalnessRoughness = metalnessRoughness.Sample(samplerStates[LINEAR], psIn.texcoord);
    
    float3 L = normalize(-lightDirection.xyz);
    float3 N = normalize(psIn.worldNormal.xyz);
    float3 T = float3(1.0001, 0, 0);
    float3 B = normalize(cross(N, T));
    T = normalize(cross(B, N));
    gBuffer.normal = normalMap.Sample(samplerStates[LINEAR], psIn.texcoord);
    gBuffer.normal.xyz = (gBuffer.normal.xyz * 2.0) - 1.0;
    gBuffer.normal.xyz = normalize((gBuffer.normal.x * T) + (gBuffer.normal.y * B) + (gBuffer.baseColor.z * N));
    gBuffer.normal.xyz = 0.5 * gBuffer.normal.xyz + 0.5f;
    gBuffer.normal.w = 1.0f;
    
    gBuffer.worldPos = psIn.worldPosition;
    
    // Shadow Map
    {
        const float shadowDepthBias = 0.01f;
        float4 lightViewPosition = mul(psIn.worldPosition, lightViewProjection);
        lightViewPosition = lightViewPosition / lightViewPosition.w;
        float2 lightViewTexcoord = 0;
        lightViewTexcoord.x = lightViewPosition.x * 0.5 + 0.5;
        lightViewTexcoord.y = lightViewPosition.y * -0.5 + 0.5;
        float depth = saturate(lightViewPosition.z - shadowDepthBias);
        
        float shadowWidth;
        float shadowHight;
        shadowMap.GetDimensions(shadowWidth, shadowHight);

        float shadowWidthOffset = 1 / shadowWidth;
        float shadowHightOffset = 1 / shadowHight;
    
        gBuffer.shadow = 1.0f;
        // shdowFactorひとつだけだと、まわりを取って平均値を出すことできれいに見せる。
        {
            gBuffer.shadow = shadowMap.SampleCmpLevelZero(shadowSampler, lightViewTexcoord, depth);
            gBuffer.shadow += shadowMap.SampleCmpLevelZero(shadowSampler, float2(lightViewTexcoord.x + shadowWidthOffset, lightViewTexcoord.y), depth); // 右
            gBuffer.shadow += shadowMap.SampleCmpLevelZero(shadowSampler, float2(lightViewTexcoord.x - shadowWidthOffset, lightViewTexcoord.y), depth); // 左
            gBuffer.shadow += shadowMap.SampleCmpLevelZero(shadowSampler, float2(lightViewTexcoord.x, lightViewTexcoord.y - shadowHightOffset), depth); // 上
            gBuffer.shadow += shadowMap.SampleCmpLevelZero(shadowSampler, float2(lightViewTexcoord.x, lightViewTexcoord.y + shadowHightOffset), depth); // 下
        
            gBuffer.shadow += shadowMap.SampleCmpLevelZero(shadowSampler, float2(lightViewTexcoord.x + shadowWidthOffset, lightViewTexcoord.y - shadowHightOffset), depth); // 右上
            gBuffer.shadow += shadowMap.SampleCmpLevelZero(shadowSampler, float2(lightViewTexcoord.x - shadowWidthOffset, lightViewTexcoord.y - shadowHightOffset), depth); // 左上
            gBuffer.shadow += shadowMap.SampleCmpLevelZero(shadowSampler, float2(lightViewTexcoord.x + shadowWidthOffset, lightViewTexcoord.y + shadowHightOffset), depth); // 右下
            gBuffer.shadow += shadowMap.SampleCmpLevelZero(shadowSampler, float2(lightViewTexcoord.x - shadowWidthOffset, lightViewTexcoord.y + shadowHightOffset), depth); // 左下
        
            gBuffer.shadow /= 9;
        }
        gBuffer.shadow.a = 1;
    }   
	
    return gBuffer;
}