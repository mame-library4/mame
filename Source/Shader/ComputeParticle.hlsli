#include "Constants.hlsli"

// パーティクルスレッド数
static const int NumParticleThread = 1024;

// 生成パーティクル構造体
struct EmitParticleData
{
    float4 parameter_;
    
    float4 position_;
    float4 rotation_;
    float4 scale_;
    
    float4 velocity_;
    float4 acceleration_;
    
    float4 rotationVelocity_;
    float4 rotationAcceleration_;
    
    float4 scaleVelocity_;
    float4 scaleAcceleration_;
    
    float4 color_;
};

// パーティクル構造体
struct ParticleData
{
    float4 parameter_;
    
    float4 position_;
    float4 rotation_;
    float4 scale_;
    
    float4 velocity_;
    float4 acceleration_;
    
    float4 rotationVelocity_;
    float4 rotationAcceleration_;
    
    float4 scaleVelocity_;
    float4 scaleAcceleration_;
    
    float4 texcoord_;
    float4 color_;
};

// indirectDataBufferへのアクセス用バイトオフセット
static const uint IndirectArgumentsNumCurrentParticle           = 0;
static const uint IndirectArgumentsNumPraviousParticle          = 4;
static const uint IndirectArgumentsNumDeadParticle              = 8;
static const uint IndirectArgumentsEmitParticleDispatchIndirect = 12;

cbuffer ComputeParticleConstantBuffer : register(b10)
{
    float deltaTime_;
    uint2 textureSplitCount_;
    uint systemNumParticles_;
    
    uint totalEmitCount_; // 生成予定のパーティクル数
    float colorIntensity_;
    uint2 commonDummy;
};

// 頂点シェーダーからジオメトリシェーダーに転送する情報
struct GSIn
{
    uint vertexId_ : VERTEX_ID;
};

// ジオメトリシェーダーからピクセルシェーダーに転送する情報
struct PSIn
{
    float4 position_ : SV_POSITION;
    float4 color_    : COLOR;
    float2 texcoord_ : TEXCOORD;
};
