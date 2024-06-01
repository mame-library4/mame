#include "PrimitiveRenderer.hlsli"

PSIn main(VSIn vsIn)
{
    PSIn vsOut;
	
    vsOut.position = mul(vsIn.position, viewProjection);
    vsOut.color = vsIn.color;
    vsOut.texcoord = vsIn.texcoord;
    
    return vsOut;
}