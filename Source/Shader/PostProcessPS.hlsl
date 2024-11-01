#include "PostProcess.hlsli"

Texture2D colorMap : register(t0);
Texture2D depthMap : register(t1);
Texture2D bloomMap : register(t2);
Texture2DArray cascadedShadowMaps : register(t3);

float3 BrightnessContrast(float3 fragmentColor, float brightness, float contrast)
{
    fragmentColor += brightness;
    if (contrast > 0.0)
    {
        fragmentColor = (fragmentColor - 0.5) / (1.0 - contrast) + 0.5;
    }
    else if (contrast < 0.0)
    {
        fragmentColor = (fragmentColor - 0.5) * (1.0 + contrast) + 0.5;
    }
    return fragmentColor;
}
float3 HueSaturation(float3 fragmentColor, float hue, float saturation)
{
    float angle = hue * 3.14159265;
    float s = sin(angle), c = cos(angle);
    float3 weights = (float3(2.0 * c, -sqrt(3.0) * s - c, sqrt(3.0) * s - c) + 1.0) / 3.0;
    fragmentColor = float3(dot(fragmentColor, weights.xyz), dot(fragmentColor, weights.zxy), dot(fragmentColor, weights.yzx));
    float average = (fragmentColor.r + fragmentColor.g + fragmentColor.b) / 3.0;
    if (saturation > 0.0)
    {
        fragmentColor += (average - fragmentColor) * (1.0 - 1.0 / (1.001 - saturation));
    }
    else
    {
        fragmentColor += (average - fragmentColor) * (-saturation);
    }
    return fragmentColor;
}

float4 main(PSIn pin) : SV_TARGET
{       
    float4 sampledColor = colorMap.Sample(samplerStates[LINEAR_BORDER_BLACK], pin.texcoord);
    float3 color = sampledColor.rgb;
    float alpha = sampledColor.a;
    
    float depthNdc = depthMap.Sample(samplerStates[LINEAR_BORDER_BLACK], pin.texcoord).x;
    
    float4 positionNdc;
    // texure space to ndc
    positionNdc.x = pin.texcoord.x * +2 - 1;
    positionNdc.y = pin.texcoord.y * -2 + 1;
    positionNdc.z = depthNdc;
    positionNdc.w = 1;
    
    // ndc to view space
    float4 positionViewSpace = mul(positionNdc, inverseProjection_);
    positionViewSpace = positionViewSpace / positionViewSpace.w;
    
    // ndc to world space
    float4 positionWorldSpace = mul(positionNdc, inverseViewProjection_);
    positionWorldSpace = positionWorldSpace / positionWorldSpace.w;
    
    // Apply cascaded shadow mapping
    // Find a layer of cascaded view frustum volume
    float depthViewSpace = positionViewSpace.z;
    int cascadeIndex = -1;
    for (uint layer = 0; layer < 4; ++layer)
    {
        float distance = cascadedPlaneDistances_[layer];
        if (distance > depthViewSpace)
        {
            cascadeIndex = layer;
            break;
        }
    }
    float shadowFactor = 1.0;
    if (cascadeIndex > -1)
    {
        // world space to light view clip space, and to ndc
        float4 positionLightSpace = mul(positionWorldSpace, cascadedMatrices_[cascadeIndex]);
        positionLightSpace /= positionLightSpace.w;
        // ndc to texture space
        positionLightSpace.x = positionLightSpace.x * +0.5 + 0.5;
        positionLightSpace.y = positionLightSpace.y * -0.5 + 0.5;
        
        shadowFactor = cascadedShadowMaps.SampleCmpLevelZero(comparisonSamplerState, float3(positionLightSpace.xy, cascadeIndex), positionLightSpace.z - shadowDepthBias_).x;
        
        float3 layerColor = 1;        
#if 1
        // デバッグ用
        if (colorizeCascadedLayer_)
        {
            const float3 layerColors[4] =
            {
                { 1, 0, 0 },
                { 0, 1, 0 },
                { 0, 0, 1 },
                { 1, 1, 0 },
            };
            layerColor = layerColors[cascadeIndex];
        }
#endif
        color *= lerp(shadowColor_, 1.0, shadowFactor) * layerColor;
    }
    
    color = HueSaturation(color, hue_, saturation_);
    color = BrightnessContrast(color, brightness_, contrast_);    
    
    color += bloomMap.Sample(samplerStates[LINEAR], pin.texcoord).rgb;    
    
    return float4(color, alpha);
}