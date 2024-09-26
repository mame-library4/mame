#include "CsmOpaque.hlsli"
#include "GltfModel.hlsli"

VS_OUT_CSM main(float3 position : POSITION, uint4 joints[2] : JOINTS, float4 weights[2] : WEIGHTS, uint instanceId : SV_INSTANCEID)
{
    VS_OUT_CSM vout;
    
    if (skin > -1)
    {
        const float totalWeight = dot(weights[0], 1) + dot(weights[1], 1);
        
        const row_major float4x4 skinMatrix =
            weights[0].x / totalWeight * jointMatrices[joints[0].x] +
			weights[0].y / totalWeight * jointMatrices[joints[0].y] +
			weights[0].z / totalWeight * jointMatrices[joints[0].z] +
			weights[0].w / totalWeight * jointMatrices[joints[0].w] +
			weights[1].x / totalWeight * jointMatrices[joints[1].x] +
			weights[1].y / totalWeight * jointMatrices[joints[1].y] +
			weights[1].z / totalWeight * jointMatrices[joints[1].z] +
			weights[1].w / totalWeight * jointMatrices[joints[1].w];
        
        position = mul(float4(position.xyz, 1), skinMatrix).xyz;
    }
    
    vout.position = mul(float4(position.xyz, 1), mul(world, viewPorjectionMatrices_[instanceId]));
    vout.slice = startInstanceLocation + instanceId;
    
    return vout;
}
