#include "Particle.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerState[3] : register(s0);

// TODO : particle
// 仮で、emissiveNoiseTexture使ってる。後で変える
Texture2D textureMap : register(t16);

float4 main(GS_OUT psIn) : SV_TARGET
{
    return float4(1, 1, 1, 1);
    float4 color = textureMap.Sample(samplerState[POINT], psIn.texcoord);
    //float4 color = textureMap.Sample(samplerState[POINT], psIn.scrollTexcoord);
    
    // dissolve
    {
        // マスク画像から赤色を取得
        float mask = textureMap.Sample(samplerState[POINT], psIn.texcoord).r;

        // step関数を用いて透過度を0/1にする
        float alpha = step(dissolveParameters.x, mask);
        
        // 縁の処理
        float edgeValue = step(mask - dissolveParameters.x, dissolveParameters.x) * step(dissolveParameters.x, mask) * step(mask, dissolveParameters.x + dissolveParameters.y);
        color.rgb += edgeColor.rgb * edgeValue;
        alpha = saturate(alpha + edgeValue);
        
        color.a *= alpha;
    }
    
    //color.rgb *= psIn.color.rgb;
    
    return color;
}