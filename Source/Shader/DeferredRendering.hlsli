struct GBuffer
{
    float4 baseColor    : SV_TARGET0;
    float4 normal       : SV_TARGET1;
    float4 worldPos     : SV_TARGET2;
    float4 shadow       : SV_TARGET3;
    float4 metalnessRoughness : SV_TARGET4;
};

struct PSIn
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 worldNormal : NORMAL;
    float4 worldTangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

cbuffer SceneConstants : register(b0)
{
    row_major float4x4 viewProjection;
    float4 lightDirection;
    float4 cameraPosition;
    row_major float4x4 lightViewProjection;
}