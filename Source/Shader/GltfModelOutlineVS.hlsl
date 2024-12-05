#include "GltfModel.hlsli"

VS_OUT main(VS_IN vsIn)
{
#if 0
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
#else
    float sigma = vsIn.tangent.w;

    if (skin > -1)
    {
        // weight’l‚ð 1 ‚ÉŽû‚ß‚é‚½‚ß‚ÌŒvŽZ
        float totalWeight = dot(vsIn.weights[0], 1) + dot(vsIn.weights[1], 1);
        
        row_major float4x4 skinMatrix =
			vsIn.weights[0].x / totalWeight * jointMatrices[vsIn.joints[0].x] +
			vsIn.weights[0].y / totalWeight * jointMatrices[vsIn.joints[0].y] +
			vsIn.weights[0].z / totalWeight * jointMatrices[vsIn.joints[0].z] +
			vsIn.weights[0].w / totalWeight * jointMatrices[vsIn.joints[0].w] +
			vsIn.weights[1].x / totalWeight * jointMatrices[vsIn.joints[1].x] +
			vsIn.weights[1].y / totalWeight * jointMatrices[vsIn.joints[1].y] +
			vsIn.weights[1].z / totalWeight * jointMatrices[vsIn.joints[1].z] +
			vsIn.weights[1].w / totalWeight * jointMatrices[vsIn.joints[1].w];
        
        
        vsIn.position = mul(float4(vsIn.position.xyz, 1), skinMatrix);
        vsIn.normal = normalize(mul(float4(vsIn.normal.xyz, 0), skinMatrix));
        vsIn.tangent = normalize(mul(float4(vsIn.tangent.xyz, 0), skinMatrix));
    }

    VS_OUT vsOut = (VS_OUT) 0;

    vsIn.position.w = 1;
    vsOut.wPosition = mul(vsIn.position, world);

    vsIn.normal.w = 0;
    vsOut.wNormal = normalize(mul(vsIn.normal, world));

    vsIn.tangent.w = 0;
    vsOut.wTangent = normalize(mul(vsIn.tangent, world));
    vsOut.wTangent.w = sigma;

    vsOut.texcoord = vsIn.texcoord;

    return vsOut;
    
    
#endif
}