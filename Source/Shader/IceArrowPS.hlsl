#include "gltfModel.hlsli"
#include "bidirectionalReflectanceDistributionFunction.hlsli"

#define BASECOLOR_TEXTURE 0
#define METALLIC_ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
#define OCCLUSION_TEXTURE 4
Texture2D<float4> materialTextures[5] : register(t1);
SamplerComparisonState comparisonSamplerState : register(s5);

float4 main(VS_OUT pin) : SV_TARGET
{
    const MaterialConstants m = materials[material];
    
    static const float GammaFactor = 2.2;
    float4 emissiveColor = float4(0, 0, 0, 0);
    float4 emissive = materialTextures[EMISSIVE_TEXTURE].Sample(samplerStates[ANISOTROPIC], pin.texcoord);
    emissive.rgb = pow(emissive.rgb, GammaFactor);
    emissiveColor = emissive * emissiveColor_;
        
    return emissiveColor;
}