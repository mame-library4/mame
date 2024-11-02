struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

struct PS_OUT
{
    float4 color : SV_TARGET;
    //float depth : SV_DEPTH;
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 viewProjection_;
    float4 lightDirection_;
    float4 cameraPosition_;
    row_major float4x4 lightViewProjection_;
    row_major float4x4 inverseViewProjection_;
}

struct PointLights
{
    float4 position_;
    float4 color_;
    float range_;
    float intensity_;
    float2 dummy_;
};
struct HemisphereLights
{
    float4 skyColor_;
    float4 groundColor_;
    float weight_;
    float3 dummy_;
};
static const int maxPointLight = 8;
cbuffer LightConstants : register(b12)
{
    HemisphereLights hemisphereLight_;
    PointLights pointLights[maxPointLight];
}

#include "GBuffer.hlsli"