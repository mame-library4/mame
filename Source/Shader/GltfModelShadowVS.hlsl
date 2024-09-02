#include "CsmOpaque.hlsli"
#include "GltfModel.hlsli"

VS_OUT_CSM main(float3 position : POSITION, uint4 joints : JOINTS, float4 weights : WEIGHTS, uint instanceId : SV_INSTANCEID)
{
    VS_OUT_CSM vout;
    
    if (skin > -1)
    {
        const float w0 = weights.x, w1 = weights.y, w2 = weights.z, w3 = weights.w;
        const float4x4 skin_matrix = w0 * jointMatrices[joints.x] + w1 * jointMatrices[joints.y] + w2 * jointMatrices[joints.z] + w3 * jointMatrices[joints.w];
        position = mul(float4(position.xyz, 1), skin_matrix).xyz;        
    }
    
    vout.position = mul(float4(position.xyz, 1), mul(world, viewPorjectionMatrices_[instanceId]));
    vout.slice = startInstanceLocation + instanceId;
    
    return vout;
}
