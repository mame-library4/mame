struct PSIn
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

cbuffer SceneConstantBuffer : register(b1)
{
    row_major float4x4  viewProjection_;
    float4              lightDirection_;
    float4              cameraPosition_;
    
    row_major float4x4  inverseProjection_;
    row_major float4x4  inverseViewProjection_;
};

cbuffer CsmConstants : register(b3)
{
    row_major float4x4 cascadedMatrices_[4];
    float4 cascadedPlaneDistances_;
}

cbuffer PostProcessConstants : register(b2)
{
    float shadowColor_;
    float shadowDepthBias_;
    int colorizeCascadedLayer_;

    float blurPower;
};

cbuffer PostEffectConstants : register(b8)
{
    float3 colorize_;
    float exposure_;
    
    float brightness_;
    float contrast_;
    float hue_;
    float saturation_;
}

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 3
#define LINEAR_BORDER_WHITE 4

SamplerState samplerStates[5] : register(s0);
SamplerComparisonState comparisonSamplerState : register(s5);