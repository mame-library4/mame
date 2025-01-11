#pragma once
#include "ComputeParticleSystem.h"
#include "Transform.h"

class ComputeParticleEmitter
{
public:
    struct EmitParameter
    {
        int emitNum_ = 100; // 一度に生成されるパーティクルの数

        float lifespan_             = 1.0f; // 生存時間
        float lifespanAmplitude_    = 0.0f; // 生存時間の振れ幅

        float emitTime_ = 1.0f; // 生成時間

        int textureType_ = 0; // 画像種類

        DirectX::XMFLOAT3 positionAmplitude_    = {};   // 生成位置の振れ幅
        
        DirectX::XMFLOAT3 velocity_             = { 0.0f, 0.0f, 0.0f }; // 初速度
        DirectX::XMFLOAT3 velocityAmplitude_    = { 0.0f, 0.0f, 0.0f };  // 初速度の振れ幅

        DirectX::XMFLOAT3 acceleration_         = { 0.0f, 0.0f, 0.0f };    //　初期加速度
        DirectX::XMFLOAT3 accelerationAmplitud_ = {};                       //　生成加速度の振れ幅

        DirectX::XMFLOAT3 scaleInit_                 = { 1.0f, 1.0f, 1.0f };    // 初期スケール
        DirectX::XMFLOAT2 scaleAmplitude_            = {};                      // 生成スケールの振れ幅
        DirectX::XMFLOAT2 scaleVelocity_             = {};                      // 初期スケール速度
        DirectX::XMFLOAT2 scaleVelocityAmplitude_    = {};                      // 生成スケール速度の振れ幅
        DirectX::XMFLOAT2 scaleAcceleration_         = {};                      // 初期スケール加速度
        DirectX::XMFLOAT2 scaleAccelerationAmplitud_ = {};                      // 生成スケール加速度の振れ幅

        DirectX::XMFLOAT3 rotationAmplitude_            = {}; // 生成回転の振れ幅
        DirectX::XMFLOAT3 rotationVelocity_             = {}; // 初期回転速度
        DirectX::XMFLOAT3 rotationVelocityAmplitude_    = {}; // 生成回転速度の振れ幅
        DirectX::XMFLOAT3 rotationAcceleration_         = {}; // 初期回転加速度
        DirectX::XMFLOAT3 rotationAccelerationAmplitud_ = {}; // 生成回転加速度の振れ幅

        float intensity_ = 1.0f;//colorの乗数

        DirectX::XMFLOAT4 color_ = { 1,1,1,1 };

        //　色の振れ幅
        DirectX::XMFLOAT4 colorAmplitud_ = { 0,0,0,0 };

        // ----- Sphere -----
        DirectX::XMFLOAT4 sphere_ = { -1.0f, 1.0f, 0.0f, 0.0f }; // x:Sphereの使用フラグ, y:半径,z:速度,w:加速度


        void DrawDebug();
    };

public:
    ComputeParticleEmitter() {}
    ~ComputeParticleEmitter() {}

    void EmitParticle(const EmitParameter& param);
    void EmitParticle(const std::string& filename);
    void EmitParticle();
    void DrawDebug();

    void SetEmitParameter(const std::string& filename);
    void SetEmitParameter(const EmitParameter& param) { emitParameter_ = param; }
    static EmitParameter GetJsonEmitParameter(const std::string& filename);

    void SetEmitPosition(const DirectX::XMFLOAT3& position) { transform_.SetPosition(position); }

private:
    void AssetCreation(const EmitParameter& param, const std::string& filename);

private:
    Transform transform_ = {};

    EmitParameter emitParameter_    = {};
};

