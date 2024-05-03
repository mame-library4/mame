#include "FullScreenQuad.hlsli"

float2 hash(float2 p)
{
    p = float2(dot(p, float2(127.1, 311.7)),
    dot(p, float2(269.5, 183.3)));
    return -1.0 + 2.0 * frac(sin(p) * 43758.5453123);
}

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 viewProjection;
    float4 lightDirection;
    float4 cameraPosition;
    float4 params;
};

float noise(in float2 p)
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;
    
    float2 i = floor(p + (p.x + p.y) * K1);
	
    float2 a = p - i + (i.x + i.y) * K2;
    float2 o = (a.x > a.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    float2 b = a - o + K2;
    float2 c = a - 1.0 + 2.0 * K2;
	
    float3 h = max(0.5 - float3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
	
    float3 n = h * h * h * h * float3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));
	
    return dot(n, 70.0);
}

float fbm(float2 uv)
{
    float f;
    float2x2 m = float2x2(1.6, 1.2, -1.2, 1.6);
    f = 0.5000 * noise(uv);
    uv = mul(uv, m);
    f += 0.2500 * noise(uv);
    uv = mul(uv, m);
    f += 0.1250 * noise(uv);
    uv = mul(uv, m);
    f += 0.0625 * noise(uv);
    uv = mul(uv, m);
    f = 0.5 + 0.5 * f;
    return f;
}
#define PI 3.14159265359

float3 hsb2rgb(in float3 c)
{
    float3 rgb = clamp(abs(fmod(c.x * 6.0 + float3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0,
                     0.0,
                     1.0);
    rgb = rgb * rgb * (3.0 - 2.0 * rgb);
    return c.z * lerp(1.0, rgb, c.y);
}

float2 rotate2D(float2 _st, float _angle)
{
    _st = mul(_st, float2x2(cos(_angle), -sin(_angle), sin(_angle), cos(_angle)));
    return _st;
}
float4 main(PSIn pin) : SV_TARGET
{
    const float time = params.x * 0.8;

	
#if 0
	float2 uv = float2(pin.texcoord.x, 1 - pin.texcoord.y);
	float2 q = uv;
	q.x *= 5.;
	q.y *= 2.;
	float strength = floor(q.x + 1.);
	//float strength = 0.8;
	float T3 = max(3., 1.25 * strength) * time * 0.5;
	q.x = fmod(q.x, 1.) - 0.5;
	q.y -= 0.25;
	float n = fbm(strength * q - float2(0, T3));
	float c = 1. - 16. * pow(max(0., length(q * float2(1.8 + q.y * 1.5, .75)) - n * max(0., q.y + .25)), 1.2);
	float c1 = n * c * (2.4 - pow(1.25 * uv.y, 4.));
//	float c1 = n * c * (1.5-pow(2.50*uv.y,4.));
	c1 = clamp(c1, 0., 1.);

	float3 color = float3(1.5 * c1, 1.5 * c1 * c1 * c1, c1 * c1 * c1 * c1 * c1 * c1);
	color = color.zyx;
	color.xy *= 0.5;
	
	float a = c * (1. - pow(uv.y, 3.));
	color = lerp(0, color, a);
	
	float alpha = any(max(0, color - 0.01)) ? exp(-q.y * 1.5) : 0.0;
	
	return float4(color, alpha);

#else
    float3 color = float3(152.0 / 255, 245.0 / 255, 250.0 / 255 * 3.0);
	
    const float N = 5.0;
    float2 p = float2((frac(pin.texcoord.x * N) - 0.5) * 2.0, 1 - pin.texcoord.y);
	
    float l0 = sin((p.y - time * 0.1) * 3.141592 * 4) * noise(pin.texcoord * 8) * 1;
    float l = abs(p.x - l0);
	
    const float w = 0.15;
    color *= (1.0 - smoothstep(0, w, l));
	
    const float alpha = any(color) ? exp(-p.y * 3.5) : 0;
	
    return float4(color, alpha);
#endif
}