#include "DeferredRendering.hlsli"
#include "bidirectionalReflectanceDistributionFunction.hlsli"

Texture2D<float4> gBufferBaseColor : register(t0);
Texture2D<float4> gBufferEmissiveColor : register(t1);
Texture2D<float4> gBufferNormal : register(t2);
Texture2D<float4> gBufferParameter : register(t3);
Texture2D<float> gBufferDepth : register(t4);

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
//SamplerState samplerStates[3] : register(s0);

float4 main(VS_OUT psIn) : SV_TARGET
{
    // GBufferテクスチャから情報をデコードする
    PSGBufferTextures gBufferTextures;
    gBufferTextures.baseColor_ = gBufferBaseColor;
    gBufferTextures.emissiveColor_ = gBufferEmissiveColor;
    gBufferTextures.normal_ = gBufferNormal;
    gBufferTextures.parameter_ = gBufferParameter;
    gBufferTextures.depth_ = gBufferDepth;
    gBufferTextures.state_ = samplerStates[POINT];
    GBufferData data;
    data = DecodeGBuffer(gBufferTextures, psIn.texcoord, inverseViewProjection_);
    
    float4 albedo = data.baseColor_;
    float3 emissiveColor = data.emissiveColor_;
    float3 N = normalize(data.worldNormal_);
    float3 V = normalize(data.worldPosition_.xyz - cameraPosition_.xyz);
    
    // 入射光農地拡散反射になる割合
    float3 diffuseReflectance = lerp(albedo.rgb, 0.0f, data.metalness_);
    
    // 垂直反射時のフレネル反射率 (非金属でも最低4%は鏡面反射する)
    float3 F0 = lerp(0.04f, albedo.rgb, data.metalness_);
    
#if 1
    //float3 diffuse = {};
    //float3 specular = {};
    //float3 L = normalize(lightDirection_.xyz);
    float3 diffuse = 0;
    float3 specular = 0;
    
    const float3 f90 = 1.0;
    float4 parameter = gBufferTextures.parameter_.Sample(samplerStates[LINEAR], psIn.texcoord);
    const float alphaRoughness = parameter.g * parameter.g;
    
    float3 L = normalize(-lightDirection_.xyz);
    float3 Li = float3(1.0, 1.0, 1.0); // Radiance of the light
    const float NoL = max(0.0, dot(N, L));
    const float NoV = max(0.0, dot(N, V));
    if (NoL > 0.0 || NoV > 0.0)
    {
        const float3 R = reflect(-L, N);
        const float3 H = normalize(V + L);

        const float NoH = max(0.0, dot(N, H));
        const float HoV = max(0.0, dot(H, V));

        diffuse += Li * NoL * brdfLambertian(F0, f90, diffuseReflectance, HoV);
        specular += Li * NoL * brdfSpecularGgx(F0, f90, alphaRoughness, HoV, NoL, NoV, NoH);
    }

    diffuse += iblRadianceLambertian(N, V, parameter.g, diffuseReflectance, F0);
    specular += iblRadianceGgx(N, V, parameter.g, F0);

    float3 emissive = gBufferEmissiveColor.Sample(samplerStates[LINEAR], psIn.texcoord).rgb;
    diffuse = lerp(diffuse, diffuse * parameter.r, parameter.a);
    specular = lerp(specular, specular * parameter.r, parameter.a);

    float3 Lo = diffuse + specular + emissive * 2.0f;
    
    float4 baseColor = gBufferBaseColor.Sample(samplerStates[POINT], psIn.texcoord);
    return float4(Lo, baseColor.a);
#endif
    
    //return gBufferBaseColor.Sample(samplerStates[POINT], psIn.texcoord);    
}