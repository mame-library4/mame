#include "GltfModel.hlsli"

struct GSOut
{
    float4  position_       : SV_POSITION;
    float4  worldPosition_  : POSITION;
    float4  worldNormal_    : NORMAL;
    float4  worldTangent_   : TANGENT;
    float2  texcoord_       : TEXCOORD;
    bool    isOutline_      : IS_OUTLINE;
};