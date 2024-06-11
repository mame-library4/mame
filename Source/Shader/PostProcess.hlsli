struct PSIn
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

cbuffer PostProcessConstants
{
    float blurPower;
    float3 dummy_;
};