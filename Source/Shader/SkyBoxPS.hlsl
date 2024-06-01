#include "SkyBox.hlsli"

Texture2D skyBox : register(t0);
SamplerState samplerState : register(s1);

float4 sample_skybox(float3 v, float roughness, float intensity = 1.0)
{
    uint width, height, numberOfLevels;
    skyBox.GetDimensions(0, width, height, numberOfLevels);
	
    float lod = roughness * float(numberOfLevels - 1);
	
    v = normalize(v);
    // Blinn/Newell Latitude Mapping
    float2 samplePoint;
    samplePoint.x = (atan2(v.z, v.x) + PI) / (PI * 2.0);
    samplePoint.y = 1.0 - ((asin(v.y) + PI * 0.5) / PI);
    return skyBox.SampleLevel(samplerState, samplePoint, lod) * intensity;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 ndc;
    ndc.x = (pin.texcoord.x * 2.0) - 1.0;
    ndc.y = 1.0 - (pin.texcoord.y * 2.0);
    ndc.z = 1;
    ndc.w = 1;
	
    float4 R = mul(ndc, inverseViewProjection);
    R /= R.w;
	
    //float roughness = scene_data.skybox_roughness;    
    
    //return sample_skybox(R.xyz, roughness);
    return sample_skybox(R.xyz, 1,0);
    
#if 0
    float3 E = normalize(pin.worldPosition.xyz - cameraPosition.xyz);

    // 視線ベクトルを正距円筒図法に則ったUV座標へ変換する
    float2 texcoord;
    {
        texcoord = float2(1.0f - atan2(E.z, E.x) / 2.0f, -atan2(E.y, length(E.xz)));
        texcoord = texcoord / PI + 0.5f;
    }
    
    return texture0.Sample(sampler0, texcoord);
#endif
}