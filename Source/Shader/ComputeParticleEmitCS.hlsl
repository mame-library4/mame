#include "ComputeParticle.hlsli"
#include "ComputeParticleEmit.hlsli"

RWStructuredBuffer<ParticleData> particleDataBuffer : register(u0); // パーティクル管理バッファ
ConsumeStructuredBuffer<uint> particlePoolBuffer : register(u1); // パーティクル番号管理バッファ (末尾から取出専用)
StructuredBuffer<EmitParticleData> emitParticleBuffer : register(t0); //パーティクル生成情報バッファ

[numthreads(1, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    // 未使用リストの末尾から未使用パーティクルのインデックスを取得
    uint particleIndex = particlePoolBuffer.Consume();
    uint emitIndex = dispatchThreadId.x;
    
    // パーティクル生成処理
    particleDataBuffer[particleIndex].parameter_.x = emitParticleBuffer[emitIndex].parameter_.x;
    particleDataBuffer[particleIndex].parameter_.y = emitParticleBuffer[emitIndex].parameter_.y;
    particleDataBuffer[particleIndex].parameter_.z = 1.0f;
    particleDataBuffer[particleIndex].parameter_.w = emitParticleBuffer[emitIndex].parameter_.w;
    
    // 位置設定    
    particleDataBuffer[particleIndex].position_ = emitParticleBuffer[emitIndex].position_;

    // 速度、速力設定
    particleDataBuffer[particleIndex].velocity_ = emitParticleBuffer[emitIndex].velocity_;
    particleDataBuffer[particleIndex].acceleration_ = emitParticleBuffer[emitIndex].acceleration_;
    
    // 球状にパーティクルを配置
    if(emitParticleBuffer[emitIndex].sphere_.x > 0.0f)
    {        
        // 中心位置から配置位置に向けたベクトルを作成
        particleDataBuffer[particleIndex].position_ = emitParticleBuffer[emitIndex].position_ + float4(CalculateSpherePosition(emitIndex), 0);        
        float3 vec = normalize(particleDataBuffer[particleIndex].position_.xyz - emitParticleBuffer[emitIndex].position_.xyz);
        
        // 半径に応じた位置を設定する
        particleDataBuffer[particleIndex].position_.xyz = emitParticleBuffer[emitIndex].position_.xyz + vec * emitParticleBuffer[emitIndex].sphere_.y;
        
        particleDataBuffer[particleIndex].velocity_ = float4(vec * -1.0f * emitParticleBuffer[emitIndex].sphere_.z, 1);
        particleDataBuffer[particleIndex].acceleration_ = float4(vec * emitParticleBuffer[emitIndex].sphere_.w, 1);
    }
    
    particleDataBuffer[particleIndex].rotation_             = emitParticleBuffer[emitIndex].rotation_;
    particleDataBuffer[particleIndex].scale_                = emitParticleBuffer[emitIndex].scale_;

    particleDataBuffer[particleIndex].rotationVelocity_     = emitParticleBuffer[emitIndex].rotationVelocity_;
    particleDataBuffer[particleIndex].rotationAcceleration_ = emitParticleBuffer[emitIndex].rotationAcceleration_;
    particleDataBuffer[particleIndex].scaleVelocity_        = emitParticleBuffer[emitIndex].scaleVelocity_;
    particleDataBuffer[particleIndex].scaleAcceleration_    = emitParticleBuffer[emitIndex].scaleAcceleration_;
    particleDataBuffer[particleIndex].color_                = emitParticleBuffer[emitIndex].color_;
}