#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

#define NUMTHREADS_X 16

struct Particle
{
    DirectX::XMFLOAT4 color{ 1, 1, 1, 1 };
    DirectX::XMFLOAT3 position{ 0, 0, 0 };
    float age{};
    DirectX::XMFLOAT3 velocity{ 0, 0, 0 };
    int state{};

    DirectX::XMFLOAT2 size = {};
    DirectX::XMFLOAT2 texPos = {};
    float animationTime = 0.0f;
    DirectX::XMFLOAT3 dummy_ = {};
};

class Particles
{
public:
    Particles(size_t particleCount);
    Particles& operator=(const Particles&) = delete;
    Particles(Particles&&) noexcept = delete;
    Particles& operator=(Particles&&) noexcept = delete;
    virtual ~Particles() = default;

    void Integrate(float deltaTime);
    void Initialize(float deltaTime);
    void Render();
    void DrawDebug();

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleBufferUav;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleBufferSrv;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> particleVS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> particlePS;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader> particleGS;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> particleCS;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> particleInitializerCS;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

    const size_t maxParticleCount;
    struct ParticleConstants
    {
        DirectX::XMFLOAT3 emitterPosition{};
        float particleSize = 5.0f;
        //float particleSize{ 0.02f };

        DirectX::XMFLOAT4 color{ 0.1f,0.1f,1.0f,1.0f };
        
        float deltaTime = 0.0f;
        float animationSpeed = 1.0f;
        DirectX::XMFLOAT2 Size_ = {};
        DirectX::XMFLOAT2 texSize_ = {};

        bool animationLoopFlag = false;
        bool dummy[3];
        float dummy_;

    };
    ParticleConstants particleData;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;
};