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


    void PlayLavaCrawlerParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition); // �n�ʂ𔇂��p�[�e�B�N��
    void PlayCoreBurstParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition);   // ���C���̔����p�[�e�B�N��

private:
    void CreateLavaCrawlerParticle(); // �n�ʂ𔇂��p�[�e�B�N������
    void CreateCoreBurstParticle();   // ���C���̔����p�[�e�B�N������

    void UpdateLavaCrawlerParticle(const float& elapsedTime); // �n�ʂ𔇂��p�[�e�B�N���X�V
    void UpdateCoreBurstParticle(const float& elapsedTime);   // ���C���̔����p�[�e�B�N���X�V

    void RenderLavaCrawlerParticle(); // �n�ʂ𔇂��p�[�e�B�N���`��
    void RenderCoreBurstParticle();   // ���C���̔����p�[�e�B�N���`��

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
        CoreBurstParticle,   // ���C���̔����p�[�e�B�N��
        LavaCrawlerParticle, // �n�ʂ𔇂��p�[�e�B�N��
    };
    enum class CBSlot
    {
        CoreBurstParticle   = 10, // ���C���̔����p�[�e�B�N��
        LavaCrawlerParticle = 11, // �n�ʂ𔇂��p�[�e�B�N��
    };

private:

    // ----- �n�ʂ𔇂��p�[�e�B�N�� -----
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

    // ----- �����p�[�e�B�N�� -----
    bool isCoreBurstParticleActive_ = false;
};

