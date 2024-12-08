#include "ComputeParticle.hlsli"

RWStructuredBuffer<ParticleData> particleDataBuffer : register(u0); //  パーティクル管理バッファ
AppendStructuredBuffer<uint> particleUnusedBuffer : register(u1); //  パーティクル番号管理バッファ(末尾への追加専用)

RWByteAddressBuffer indirectDataBuffer : register(u2); // インダイレクト用バッファー

[numthreads(NumParticleThread, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint index = dispatchThreadId.x;
    
    // 有効フラグが立っているものだけ処理
    if (particleDataBuffer[index].parameter_.z < 0.0f) return;
    
    // 経過時間分減少させる
    particleDataBuffer[index].parameter_.y -= deltaTime_;
    if(particleDataBuffer[index].parameter_.y < 0)
    {
        // 寿命が尽きたら未使用リストに追加
        particleDataBuffer[index].parameter_.z = -1.0f; // 生存フラグを初期化しておく
        particleUnusedBuffer.Append(index);
        
        // 死亡数をカウントする
        indirectDataBuffer.InterlockedAdd(IndirectArgumentsNumDeadParticle, 1);
        
        return;
    }
    
    // 速度更新
    particleDataBuffer[index].velocity_.xyz += particleDataBuffer[index].acceleration_.xyz * deltaTime_;
    
    // 位置更新
    particleDataBuffer[index].position_.xyz += particleDataBuffer[index].velocity_.xyz * deltaTime_;
    
    if (particleDataBuffer[index].position_.y <= 0.0f)
    {
        particleDataBuffer[index].position_.y;
        particleDataBuffer[index].velocity_ = 0;
        particleDataBuffer[index].acceleration_ = 0;
        
        if (particleDataBuffer[index].parameter_.y > 1.0f)
            particleDataBuffer[index].parameter_.y = 1.0f;
    }
    
    // 回転速度更新
    particleDataBuffer[index].rotationVelocity_.xyz += particleDataBuffer[index].rotationAcceleration_.xyz * deltaTime_;
    
    // 回転更新
    particleDataBuffer[index].rotation_.xyz += particleDataBuffer[index].rotationVelocity_.xyz * deltaTime_;
    
    // スケール速度変更
    particleDataBuffer[index].scaleVelocity_.xyz += particleDataBuffer[index].scaleAcceleration_.xyz * deltaTime_;
    
    // スケール更新
    particleDataBuffer[index].scale_.xyz += particleDataBuffer[index].scaleVelocity_.xyz * deltaTime_;
    
    // 切り取り座標を算出
    uint type = (uint) (particleDataBuffer[index].parameter_.x + 0.5f);
    
    float width  = 1.0 / textureSplitCount_.x;
    float height = 1.0 / textureSplitCount_.y;
    
    float2 uv = float2((type % textureSplitCount_.x) * width, (type / textureSplitCount_.x) * height);
    particleDataBuffer[index].texcoord_.xy = uv;
    particleDataBuffer[index].texcoord_.zw = float2(width, height);
    
    // 徐々に透明にしていく
    particleDataBuffer[index].color_.a = saturate(particleDataBuffer[index].parameter_.y * 0.5f);
}