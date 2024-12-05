#include "GltfModelOutline.hlsli"

[maxvertexcount(6)]
void main(triangle VS_OUT input[3], inout TriangleStream<GSOut> output)
{
    int i;
    
    // •\–Ê“o˜^
    for (i = 0; i < 3; ++i)
    {
        GSOut element = (GSOut) 0;
        element.position_ = mul(float4(input[i].wPosition.xyz, 1), viewProjection);
        element.worldPosition_ = input[i].wPosition;
        element.worldNormal_ = input[i].wNormal;
        element.worldTangent_ = input[i].wTangent;
        element.texcoord_ = input[i].texcoord;
        element.isOutline_ = false;
        output.Append(element); // ’¸“_¶¬
    }
    output.RestartStrip(); // ’Ç‰Á‚µ‚½’¸“_‚ð‚à‚Æ‚ÉƒvƒŠƒ~ƒeƒBƒu‚ð¶¬
    
    // — –Ê“o˜^
    for (i = 2; i >= 0; --i)
    {
        GSOut element = (GSOut) 0;
        element.worldPosition_.w = 1;
        element.worldPosition_.xyz = input[i].wPosition.xyz + input[i].wNormal.xyz * outlineSize_;
        element.worldNormal_ = input[i].wNormal;
        element.worldTangent_ = input[i].wTangent;
        element.texcoord_ = input[i].texcoord;
        element.position_ = mul(float4(element.worldPosition_.xyz, 1), viewProjection);
        element.isOutline_ = true;
        output.Append(element);
    }
    output.RestartStrip();
}