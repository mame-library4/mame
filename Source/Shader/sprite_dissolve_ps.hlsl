#include "SpriteDissolve.hlsli"

Texture2D color_map : register(t0);
SamplerState color_sampler_state : register(s0);
Texture2D mask_texture : register(t1);

float4 main(PSIn pin) : SV_TARGET
{
    float4 color = color_map.Sample(color_sampler_state,pin.texcoord) * pin.color;

    // マスク画像から赤色を取得
    float mask = mask_texture.Sample(color_sampler_state, pin.texcoord).r;

    //float mask_value = mask_texture.Sample(color_sampler_state, pin.texcoord);
    
    // step関数を用いて透過値を0/1にする
    //float alpha = step(mask, parameters.x);
    float alpha = step(parameters.x, mask);
    //float alpha = step(parameters.x, mask_value);
    
    // 縁の処理
    float edgeValue = step(mask - parameters.x, parameters.x) * step(parameters.x, mask) * step(mask, parameters.x + parameters.z);


    color.rgb += edgeColor.rgb * edgeValue;
    alpha = saturate(alpha + edgeValue);

    // colorの透過値に乗算する
    color.a *= alpha;

    

    // 色々いじってたらいい感じにフェードアウト的なやつ作れた
    //float temp = step(parameters.y, mask_value);
    //float temp1 = step(alpha, temp);
    //color.r *= temp1;
    //color.g *= temp1;
    //color.b *= temp1;

    return color;
}