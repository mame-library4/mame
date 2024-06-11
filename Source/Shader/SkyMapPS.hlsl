#include "FullScreenQuad.hlsli"

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 viewProjection_;
    float4 lightDirection_;
    float4 cameraPosition_;
    row_major float4x4 lightViewProjection_;
    row_major float4x4 inverseViewProjection_;
}

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerStates[3] : register(s0);
Texture2D skybox : register(t0);

float4 sampleSkyBox(float3 v, float roughness)
{
    const float PI = 3.14159265358979;
    
    uint width, height, number_of_levels;
    skybox.GetDimensions(0, width, height, number_of_levels);

    float lod = roughness * float(number_of_levels - 1);
    v = normalize(v);
    
    float2 samplePoint;
    samplePoint.x = (atan2(v.z, v.x) + PI) / (PI * 2.0);
    samplePoint.y = 1.0 - ((asin(v.y) + PI * 0.5) / PI);
    return skybox.SampleLevel(samplerStates[LINEAR], samplePoint, lod);
}

float4 main(PSIn psIn) : SV_TARGET
{
    float4 ndc;
    ndc.x = (psIn.texcoord.x * 2.0) - 1.0;
    ndc.y = 1.0 - (psIn.texcoord.y * 2.0);
    ndc.z = 1;
    ndc.w = 1;

    float4 R = mul(ndc, inverseViewProjection_);
    R /= R.w;
    const float roughness = 0;
    return sampleSkyBox(R.xyz, roughness);
}