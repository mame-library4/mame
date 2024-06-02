#include "DeferredRendering.hlsli"

Texture2D<float4> gBufferBaseColor : register(t0);
Texture2D<float4> gBufferEmissiveColor : register(t1);
Texture2D<float4> gBufferNormal : register(t2);
Texture2D<float4> gBufferParameter : register(t3);
Texture2D<float> gBufferDepth : register(t4);

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerStates[3] : register(s0);

float4 main(VS_OUT psIn) : SV_TARGET
{
    return gBufferBaseColor.Sample(samplerStates[POINT], psIn.texcoord);
}