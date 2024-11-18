struct VSIn
{
    float4 position_ : POSITION;
    float4 color_    : COLOR;
    float2 texcoord_ : TEXCOORD;
};

struct PSIn
{
    float4 position_        : SV_POSITION;
    float4 color_           : COLOR;
    float2 texcoord_        : TEXCOORD0;
    float2 scrollTexcoord_  : TEXCOORD1;
};

cbuffer ScrollConstants : register(b4)
{
    float2  direction_;
    float   timer_;
    float   threshold_;
};