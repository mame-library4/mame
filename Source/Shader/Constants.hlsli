#ifndef __CONSTANTS__
#define __CONSTANTS__

struct SceneConstants
{
    row_major float4x4 viewProjection_;
    float4 lightDirection_;
    float4 eyePosition;
    row_major float4x4 inverseProjection_;
    row_major float4x4 inverseViewProjection_;
};
cbuffer sceneConstants : register(b1)
{
    SceneConstants sceneData;
}

struct EffectConstants
{
    float shadowDepthBias_;
    float shadowColor_;
    float shadowFilterRadius_;
    uint shadowSampleCount_;
    uint colorizeCascadedLayer_;
};
cbuffer effectConstants : register(b11)
{
    EffectConstants effectData_;
}

struct CsmConstants
{
    row_major float4x4 viewProjectionMatrices_[4];
    float4 cascadePlaneDistances_;
};
cbuffer csmConstants : register(b12)
{
    CsmConstants csmData_;
}

#endif // __CONSTANTS__