#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>
#define NUMTHREADS_X 16

// パーティクルのデータ
class ParticleData
{
public:
    ParticleData(const size_t& particleCount);
    ~ParticleData() {}

    // ----- 生成 -----
    void CreateParticleData(const UINT& particleDataSize, const UINT& cbSize,
        const char* vsCsoName, const char* psCsoName, const char* gsCsoName,
        const char* csInitCsoName, const char* csUpdateCsoName);
        
    void PlayParticle(const int& csSlot, const int& cbSlot, void* data);    // 再生
    void Update(const int& csSlot, const int& cbSlot, void* data);          // 更新
    void Render(const int& gsSlot, const int& cbSlot, void* data);          // 描画

    UINT Align(UINT num, UINT alignment) { return (num + (alignment - 1)) & ~(alignment - 1); }

    [[nodiscard]] const bool GetIsActive() const { return isActive_; }
    void SetIsActive(const bool& flag) { isActive_ = flag; }

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer>                particleBuffer_;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   particleBufferUAV_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    particleBufferSRV_;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>          particleVS_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>           particlePS_;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>        particleGS_;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>         particleUpdateCS_;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>         particleInitializeCS_;

    Microsoft::WRL::ComPtr<ID3D11Buffer>                constantBuffer_;

    const size_t maxParticleCount_;

    bool isActive_ = false;
};

