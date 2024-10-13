#pragma once
#include "ParticleSystem.h"

class SuperNovaParticle : public ParticleSystem
{
public:
    SuperNovaParticle();
    ~SuperNovaParticle() override {}

    void Initialize(const float& elapsedTime) override {}
    void Update(const float& elapsedTime)     override;
    void Render()                             override;
    void DrawDebug()                          override;


    void PlayLavaCrawlerParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition); // 地面を這うパーティクル
    void PlayCoreBurstParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition);   // メインの爆発パーティクル

private:
    void CreateLavaCrawlerParticle(); // 地面を這うパーティクル生成
    void CreateCoreBurstParticle();   // メインの爆発パーティクル生成

    void UpdateLavaCrawlerParticle(const float& elapsedTime); // 地面を這うパーティクル更新
    void UpdateCoreBurstParticle(const float& elapsedTime);   // メインの爆発パーティクル更新

    void RenderLavaCrawlerParticle(); // 地面を這うパーティクル描画
    void RenderCoreBurstParticle();   // メインの爆発パーティクル描画

private:
    struct ParticleData
    {
        DirectX::XMFLOAT4 color_ = { 1, 1, 1, 1 };
        DirectX::XMFLOAT3 position_ = {};
        DirectX::XMFLOAT3 velocity_ = {};
        float age_ = 0.0f;
        int state_ = 0;
    };
    struct Constants
    {
        DirectX::XMFLOAT3 emitterPosition_ = { 0, 2, 0 };
        float particleSize_ = 0.02f;
        float time_ = 0.0f;
        float deltaTime_ = 0.0f;
        float speed_ = 1.0f;
        float dummy_ = 0.0f;
    };
    Constants constants_;

    enum class CSShaderSlot
    {
        CoreBurstParticle,   // メインの爆発パーティクル
        LavaCrawlerParticle, // 地面を這うパーティクル
    };
    enum class CBSlot
    {
        CoreBurstParticle   = 10, // メインの爆発パーティクル
        LavaCrawlerParticle = 11, // 地面を這うパーティクル
    };

private:

    // ----- 地面を這うパーティクル -----
    Microsoft::WRL::ComPtr<ID3D11Buffer>                lavaCrawlerParticleBuffer_;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   lavaCrawlerParticleBufferUAV_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    lavaCrawlerParticleBufferSRV_;    
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    lavaCrawlerParticleSRV_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>                lavaCrawlerParticleConstantBuffer_;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>          lavaCrawlerParticleVS_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>           lavaCrawlerParticlePS_;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>        lavaCrawlerParticleGS_;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>         lavaCrawlerParticleUpdateCS_;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>         lavaCrawlerParticleInitializeCS_;
    const size_t    maxLavaCrawlerParticleCount_ = 3000;
    bool            isCrawlerParticleActive_ = false;
    Constants lavaCrawlerParticleConstants_;

    // ----- 爆発パーティクル -----
    bool isCoreBurstParticleActive_ = false;
};

