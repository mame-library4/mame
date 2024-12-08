#include "ComputeParticle.hlsli"

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
    
    particleDataBuffer[particleIndex].position_             = emitParticleBuffer[emitIndex].position_;
    particleDataBuffer[particleIndex].rotation_             = emitParticleBuffer[emitIndex].rotation_;
    particleDataBuffer[particleIndex].scale_                = emitParticleBuffer[emitIndex].scale_;
    particleDataBuffer[particleIndex].velocity_             = emitParticleBuffer[emitIndex].velocity_;
    particleDataBuffer[particleIndex].acceleration_         = emitParticleBuffer[emitIndex].acceleration_;
    particleDataBuffer[particleIndex].rotationVelocity_     = emitParticleBuffer[emitIndex].rotationVelocity_;
    particleDataBuffer[particleIndex].rotationAcceleration_ = emitParticleBuffer[emitIndex].rotationAcceleration_;
    particleDataBuffer[particleIndex].scaleVelocity_        = emitParticleBuffer[emitIndex].scaleVelocity_;
    particleDataBuffer[particleIndex].scaleAcceleration_    = emitParticleBuffer[emitIndex].scaleAcceleration_;
    particleDataBuffer[particleIndex].color_                = emitParticleBuffer[emitIndex].color_;
}