// FOG
#include "FullScreenQuad.hlsli"
// NOISE
#include "Noise.hlsli"

cbuffer FOG_CONSTANT_BUFFER : register(b2)
{
    float4 fogColor;            // 色
    float fogDensity;           // 密度
    float fogHeightFalloff;     // 高さの減衰
    float startDistance;        // 開始距離
    float fogCutoffDistance;    // カットオフ距離
    float timeScale;            // タイムスケール
    float speedScale;           // スピードスケール
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 viewProjection;
    float4 lightDirection;
    float4 cameraPosition;
    
    row_major float4x4 shadowLightViewProjection;
    
    // FOG
    row_major float4x4 inverseProjection;
    row_major float4x4 inverseViewProjection;
    float time;
}

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define LINEAR_BORDER_BLACK 4
#define LINEAR_BORDER_WHITE 5
SamplerState samplerStates[5] : register(s0);
Texture2D textureMaps[4] : register(t0);

float FogDensityDistribution(float3 d /*ray path*/, float3 o /*camera position*/, float b /*global density*/, float c /*height falloff*/)
{
	// https://developer.download.nvidia.com/presentations/2007/D3DTutorial_Crytek.pdf page.17-18
    float f = 0; // fog density distribution
    f = b * exp(-c * o.y) * length(d) * (1.0 - exp(-c * d.y)) / (c * d.y);
    return max(0.0, 1.0 - exp(-f));
}

float4 main(PSIn psIn) : SV_TARGET
{
    float depth = textureMaps[0].Sample(samplerStates[LINEAR_BORDER_BLACK], psIn.texcoord).x;
    
    float4 position;
    // texture space to ndc
    position.x = psIn.texcoord.x * +2 - 1;
    position.y = psIn.texcoord.y * -2 + 1;
    position.z = depth;
    position.w = 1;
    
    // ndc to world space
    position = mul(position, inverseViewProjection);
    position = position / position.w;
    
    float3 v = position.xyz;
    float3 o = cameraPosition.xyz;      // cameraPosition
    
    float3 d = v - o;                   // ray path
    const float b = fogDensity;         // global density
    const float c = fogHeightFalloff;   // height falloff
    float f = FogDensityDistribution(d, o, b, c);
    
    
    
    const float distance = length(d);
    
    // startDistance : Distance from the camera that the fog will start.
    f *= exp(-0.1 * max(0.0, startDistance - distance));
    
    // fogCuttoffDistance : Scene elements past this distance will not have fog applied. this is useful for excluding skyboxes that already have fog baked into them.
    f *= exp(-0.1 * max(0.0, distance - fogCutoffDistance));
    
    
    float3 color = fogColor.rgb;
    float fogIntensity = fogColor.w;
    
    
    // Fractal Brownian Motion - lightweight version
    float t = time * timeScale;
    float3 p = position.xyz * speedScale;
    //color *= mod7(p + normalize(float3(1.0, 0.0, 0.5) /*wind velocity*/) * t);
    //color *= curl_noise(p + normalize(float3(1.0, 0.0, 0.5) /*wind velocity*/) * t);
    //color *= vector_field(p + normalize(float3(1.0, 0.0, 0.5) /*wind velocity*/) * t);
    //color *= noise(p + normalize(float3(1.0, 0.0, 0.5) /*wind velocity*/) * t);
    color *= fbm(p + normalize(float3(1.0, 0.0, 0.5) /*wind velocity*/) * t);    
    
    
    color = lerp(0, color, f);    
        
    return float4(color * fogIntensity, 1);
}
