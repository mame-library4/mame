#include "gltfModel.hlsli"

float4 main() : SV_TARGET
{
    float color = 0.5f;
    return float4(color.rrr, 1.0f);
    
    return float4(1, 1, 1, 1);
}