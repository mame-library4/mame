#include "SkyBox.hlsli"

VS_OUT main(float4 position : POSITION)
{
    VS_OUT vout;

    position.z = 1.0f;
    vout.position = position;

    // ƒ[ƒ‹ƒh•ÏŠ·
    float4 p = mul(position, inverseViewProjection);
    vout.worldPosition = p / p.w;
    
    return vout;
}