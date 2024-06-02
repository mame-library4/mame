#include "gltfModel.hlsli"
#include "GBuffer.hlsli"

#define BASECOLOR_TEXTURE 0
#define METALLIC_ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
#define OCCLUSION_TEXTURE 4

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
Texture2D<float4> materialTextures[5] : register(t1);
Texture2D<float4> shadowMap : register(t9);
SamplerState samplerStates[3] : register(s0);
SamplerComparisonState comparisonSamplerState : register(s5);

PSGBufferOut main(VS_OUT psIn, bool isFrontFace : SV_IsFrontFace)
{
    MaterialConstants m = materials[material];
	
    static const float GammaFactor = 2.2;
	
	// ベースカラーを取得
    float4 baseColor = m.pbrMetallicRoughness.baseColorFactor;
    if(m.pbrMetallicRoughness.baseColorTexture.index > -1)
    {
        float4 sampled = materialTextures[BASECOLOR_TEXTURE].Sample(samplerStates[ANISOTROPIC], psIn.texcoord);
        sampled.rgb = pow(sampled.rgb, GammaFactor);
        baseColor *= sampled;
    }
    clip(baseColor.a - 0.25f);
    
    // 自己発行色を取得
    float3 emissiveColor = m.emissiveFactor;
    if (m.emissiveTexture.index > -1)
    {
        float3 emissive = materialTextures[EMISSIVE_TEXTURE].Sample(samplerStates[ANISOTROPIC], psIn.texcoord).rgb;
        emissive.rgb = pow(emissive.rgb, GammaFactor);
        emissiveColor.rgb *= emissive.rgb;
    }
    
    // 法線/従法線/接線
    float3 N = normalize(psIn.wNormal.xyz);
    float3 T = hasTangent ? normalize(psIn.wTangent.xyz) : float3(1, 0, 0);
    float sigma = hasTangent ? psIn.wTangent.w : 1.0;
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);
    // 裏面描画の場合は反転しておく
    if (isFrontFace == false)
    {
        T = -T;
        B = -B;
        N = -N;
    }
    
    // 法線マッピング
    if(m.normalTexture.index > -1)
    {
        float4 sampled = materialTextures[NORMAL_TEXTURE].Sample(samplerStates[LINEAR], psIn.texcoord);
        float3 normalFactor = sampled.xyz;
        normalFactor = (normalFactor * 2.0f) - 1.0f;
        normalFactor = normalize(normalFactor * float3(m.normalTexture.scale, m.normalTexture.scale, 1.0f));
        N = normalize((normalFactor.x * T) + (normalFactor.y * B) + (normalFactor.z * N));
    }
	
    // 視線ベクトル
    float3 V = normalize(psIn.wPosition.xyz - cameraPosition.xyz);
    
    // 金属質/粗さを取得
    float roughness = m.pbrMetallicRoughness.roughnessFactor;
    float metalness = m.pbrMetallicRoughness.metallicFactor;
    if (m.pbrMetallicRoughness.metallicRoughnessTexture.index > -1)
    {
        float4 sampled = materialTextures[METALLIC_ROUGHNESS_TEXTURE].Sample(samplerStates[LINEAR], psIn.texcoord);
        roughness *= sampled.g;
        metalness *= sampled.b;
    }
    
    // 光の遮蔽地を取得
    float occlusionFactor = 1.0f;
    if(m.occlusionTexture.index > -1)
    {
        float4 sampled = materialTextures[OCCLUSION_TEXTURE].Sample(samplerStates[LINEAR], psIn.texcoord);
        occlusionFactor *= sampled.r;
    }
    const float occlusionStrength = m.occlusionTexture.strength;
    
    // GBufferDataに出力情報をまとめる
    GBufferData data;
    data.baseColor_ = baseColor;
    data.emissiveColor_ = emissiveColor;
    data.worldNormal_ = N;
    data.worldPosition_ = psIn.wPosition;
    //data.depth_
    data.roughness_ = roughness;
    data.metalness_ = metalness;
    data.occlusionFactor_ = occlusionFactor;
    data.occlusionStrength_ = occlusionStrength;
    
    return EncodeGBuffer(data, viewProjection);
}