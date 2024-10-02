struct PSIn
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

cbuffer PostProcessConstants
{
    float shadowColor_;
    float shadowDepthBias_;
    bool colorizeCascadedLayer_;

    float blurPower;
};