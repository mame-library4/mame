struct PSIn
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState samplerStates[5] : register(s0);
SamplerComparisonState comparisonSamplerState : register(s5);

Texture2D colorMap : register(t0);
Texture2D depthMap : register(t1);
Texture2D bloomMap : register(t2);
Texture2DArray cascadedShadowMaps : register(t3);

cbuffer SceneConstantBuffer : register(b1)
{
    row_major float4x4 viewProjection_;
    float4 lightDirection_;
    float4 cameraPosition_;
    
    row_major float4x4 inverseProjection_;
    row_major float4x4 inverseViewProjection_;
};

cbuffer PostProcessConstants : register(b2)
{
    float shadowColor_;
    float shadowDepthBias_;
    bool colorizeCascadedLayer_;

    float blurPower;
};

cbuffer CsmConstants : register(b3)
{
    row_major float4x4 cascadedMatrices_[4];
    float4 cascadedPlaneDistances_;
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
    
    color += bloomMap.Sample(samplerStates[LINEAR], pin.texcoord).rgb;    
    
    // Tone mapping : HDR -> SDR
    const float exposure = 1.2;
    //color = 1 - exp(-color * exposure);
    
    // Gamma process
    const float GAMMA = 2.2;
    //color = pow(color, 1.0 / GAMMA);   
    
    return float4(color, alpha);
}