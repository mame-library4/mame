struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 viewProjection_;
    float4 lightDirection_;
    float4 cameraPosition_;
    row_major float4x4 lightViewProjection_;
    row_major float4x4 inverseViewProjection_;
}

#include "GBuffer.hlsli"