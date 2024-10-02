struct VS_OUT_CSM
{
    float4 position : SV_POSITION;
    uint slice : INSTANCEID;
};
struct GS_OUTPUT_CSM
{
    float4 position : SV_POSITION;
    uint slice : SV_RENDERTARGETARRAYINDEX;
};

cbuffer CsmConstants : register(b3)
{
    row_major float4x4 viewPorjectionMatrices_[4];
    float4 cascadePlaneDistance_;
};