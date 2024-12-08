#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <vector>

class ComputeParticleSystem
{
public:
    // パーティクルスレッド数
    static constexpr UINT numParticleThread_ = 1024;

    // パーティクル生成用構造体
    struct EmitParticleData
    {
        DirectX::XMFLOAT4 parameter_; // x : パーティクル処理タイプ, y : 生存時間, zw : 空き

        DirectX::XMFLOAT4 position_             = {}; // 生成座標
        DirectX::XMFLOAT4 rotation_             = {}; // 回転
        DirectX::XMFLOAT4 scale_                = {}; // スケール

        DirectX::XMFLOAT4 velocity_             = {}; // 初速
        DirectX::XMFLOAT4 acceleration_         = {}; // 加速度

        DirectX::XMFLOAT4 rotationVelocity_     = {}; // 回転初速
        DirectX::XMFLOAT4 rotationAcceleration_ = {}; // 回転加速度

        DirectX::XMFLOAT4 scaleVelocity_        = {}; // スケール初速
        DirectX::XMFLOAT4 scaleAcceleration_    = {}; // スケール加速度

        DirectX::XMFLOAT4 color_                = {}; // 色

        DirectX::XMFLOAT4 sphere_ = {};
    };

    // パーティクル構造体
    struct ParticleData
    {
        DirectX::XMFLOAT4 parameter_;
        
        DirectX::XMFLOAT4 position_;
        DirectX::XMFLOAT4 rotation_;
        DirectX::XMFLOAT4 scale_;

        DirectX::XMFLOAT4 velocity_;
        DirectX::XMFLOAT4 acceleration_;

        DirectX::XMFLOAT4 rotationVelocity_;
        DirectX::XMFLOAT4 rotationAcceleration_;

        DirectX::XMFLOAT4 scaleVelocity_;
        DirectX::XMFLOAT4 scaleAcceleration_;

        DirectX::XMFLOAT4 texcoord_;
        DirectX::XMFLOAT4 color_;
    };

    struct Constants
    {
        float               deltaTime_;
        DirectX::XMUINT2    textureSplitCount_;  // テクスチャの分割数
        UINT                systemNumParticles_; // パーティクル総数

        UINT totalEmitCount_; // 現在のフレームでのパーティクル総生成数
        
        float colorIntensity_ = 2.0f;
        
        UINT constantDummy_[2];
    };

    //DispatchIndirect用構造体
    using DispatchIndirect = DirectX::XMUINT3; //UINT3で十分

    // 00バイト目：現在のパーティクル総数
    // 04バイト目：１F前のパーティクル総数
    // 08バイト目：パーティクル破棄数
    // 12バイト目：パーティクル生成用DispatchIndirect情報
    static constexpr UINT numCurrentParticleOffset_ = 0;
    static constexpr UINT numPreviousParticleOffset_ = numCurrentParticleOffset_ + sizeof(UINT);
    static constexpr UINT numDeadParticleOffset_ = numPreviousParticleOffset_ + sizeof(UINT);
    static constexpr UINT emitDispatchIndirectOffset_ = numDeadParticleOffset_ + sizeof(UINT);
    static constexpr UINT drawIndirectSize_ = emitDispatchIndirectOffset_ + sizeof(DispatchIndirect);


private:
    ComputeParticleSystem(UINT particlesCount, DirectX::XMUINT2 splitCount);
    ~ComputeParticleSystem();

public:
    static ComputeParticleSystem& Instance()
    {
        static ComputeParticleSystem instance(1000000, { 4, 4 });

        return instance;
    }

    void Emit(const EmitParticleData& data);
    void Update(const float& deltaTime);
    void Render();

    void DrawDebug();

    const DirectX::XMUINT2& GetTextureSplitCount() const { return textureSplitCount_; }

private:
    UINT numParticles_;
    UINT numEmitParticles_;
    bool oneShotInitialize_;
    DirectX::XMUINT2 textureSplitCount_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;

    std::vector<EmitParticleData> emitParticles_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;

    Constants constants_;

    // パーティクルバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer>                particleDataBuffer_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    particleDataSRV_;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   particleDataUAV_;

    // 未使用パーティクル番号を格納した Append/Cosumeバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer>                particleAppendConsumeBuffer_;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   particleAppendConsumeUAV_;

    // パーティクル生成情報を格納したバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer>                particleEmitBuffer_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    particleEmitSRV_;

    // 各種シェーダー
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>         initShader_;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>         emitShader_;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>         updateShader_;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>          vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>        geometryShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>           pixelShader_;

    Microsoft::WRL::ComPtr<ID3D11Buffer>                indirectDataBuffer_;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   indirectDataUAV_;

    Microsoft::WRL::ComPtr<ID3D11ComputeShader>         beginFrameShader_;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>         endFrameShader_;
};
