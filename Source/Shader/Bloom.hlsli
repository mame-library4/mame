struct PSIn
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

cbuffer BloomConstants : register(b0)
{
    float bloomExtractionThreshold_;
    float bloomIntencity_;
}