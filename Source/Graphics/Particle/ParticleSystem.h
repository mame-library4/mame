#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>

#define NUMTHREADS_X 16

class ParticleSystem
{
public:
    ParticleSystem(size_t particleCount);
    virtual ~ParticleSystem() {}

    virtual void Initialize(const float& deltaTime) = 0;
    virtual void Update(const float& deltaTime)     = 0;
    virtual void Render()                           = 0;
    virtual void DrawDebug()                        = 0;

    UINT Align(UINT num, UINT alignment) { return (num + (alignment - 1)) & ~(alignment - 1); }

protected:
    Microsoft::WRL::ComPtr<ID3D11Buffer>                particleBuffer_;    
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   particleBufferUAV_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    particleBufferSRV_;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>          particleVS_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>           particlePS_;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>        particleGS_;    
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>         particleUpdateCS_;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>         particleInitializeCS_;
    
    Microsoft::WRL::ComPtr<ID3D11InputLayout>           inputLayout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>                constantBuffer_;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;

    const size_t maxParticleCount_;
};