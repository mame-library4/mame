#include "FbxModel.hlsli"

PSIn main(VSIn vsIn)
{
    // 頂点の位置と法線にボーン行列の影響を与える
    vsIn.normal.w = 0;
    float sigma = vsIn.tangent.w;
    vsIn.tangent.w = 0;

    float4 blendedPosition = { 0, 0, 0, 1 };
    float4 blendedNormal = { 0, 0, 0, 0 };
    float4 blendedTangent = { 0, 0, 0, 0 };
    for (int boneIndex = 0; boneIndex < 4; ++boneIndex)
    {
        blendedPosition += vsIn.boneWeights[boneIndex]
            * mul(vsIn.position, boneTransforms[vsIn.boneIndices[boneIndex]]);
        blendedNormal += vsIn.boneWeights[boneIndex]
            * mul(vsIn.normal, boneTransforms[vsIn.boneIndices[boneIndex]]);
        blendedTangent += vsIn.boneWeights[boneIndex]
            * mul(vsIn.tangent, boneTransforms[vsIn.boneIndices[boneIndex]]);
    }
    vsIn.position = float4(blendedPosition.xyz, 1.0f);
    vsIn.normal = float4(blendedNormal.xyz, 0.0f);
    vsIn.tangent = float4(blendedTangent.xyz, 0.0f);


    PSIn psIn;
    psIn.position = mul(vsIn.position, mul(world, viewProjection));

    psIn.worldPosition = mul(vsIn.position, world);
    psIn.worldNormal = normalize(mul(vsIn.normal, world));
    psIn.worldTangent = normalize(mul(vsIn.tangent, world));
    psIn.worldTangent.w = sigma;
        
    psIn.texcoord = vsIn.texcoord;
    
    psIn.color = materialColor;
    
    // LIM_LIGHT
    psIn.normalInView = mul(viewProjection, vsIn.normal); // カメラ空間の法線を求める
    
    // EMISSIVE : emissiveTextureUVScroll
    psIn.emissiveTexcoord = vsIn.texcoord + emissiveScrollDirection * emissiveOptions.xx;
    
    // ZELDA
    psIn.localPosition = vsIn.position;
    
    return psIn;
}