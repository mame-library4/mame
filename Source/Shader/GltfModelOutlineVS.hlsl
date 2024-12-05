#include "GltfModel.hlsli"

VS_OUT main(VS_IN vsIn)
{
    float sigma = vsIn.tangent.w;
    
    VS_OUT vsOut = (VS_OUT) 0;
    
    vsOut.wPosition = mul(vsIn.position, world);
    
    vsIn.normal.w = 0;
    vsOut.wNormal = normalize(mul(vsIn.normal, world));
    
    vsIn.tangent.w = 0;
    vsOut.wTangent = normalize(mul(vsIn.tangent, world));
    vsOut.wTangent.w = sigma;
    
    vsOut.texcoord = vsIn.texcoord;
    
    return vsOut;
}