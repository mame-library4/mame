#pragma once
#include <d3d11.h>
#include <memory>
#include <wrl.h>
#include "../Graphics/ConstantBuffer.h"
#include "../Graphics/FrameBuffer.h"
#include "../Graphics/FullscreenQuad.h"

class Bloom
{
public:
    Bloom();
    ~Bloom() = default;

    // ----- bloom ���s�֐� -----
    void Bloom::Execute(ID3D11DeviceContext* deviceContext, 
        ID3D11ShaderResourceView* colorMap);
    // ----- ImGui�p -----
    void DrawDebug();
    // ----- bloom ShaderResourceView -----
    ID3D11ShaderResourceView** GetShaderResourceView() { return bloom_->shaderResourceViews[0].GetAddressOf(); }

private:
    // ---------- �����o���p -----------------------------------
    std::unique_ptr<FullscreenQuad> renderer_;

    // ---------- �P�x���o -----------------------------------
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   luminancePS_;
    std::unique_ptr<FrameBuffer>                luminanceExtraction_;

    // ---------- �ڂ��� ( �u���[ ) ------------------------------
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   gaussianBlurHorizontalPS_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   gaussianBlurVerticalPS_;
    static const int                            maxBlurBufferCount_ = 8;
    std::unique_ptr<FrameBuffer>                gaussianBlur_[maxBlurBufferCount_];

    // ---------- �ŏI���� ( �u���[�� ) ----------------------------------------
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   bloomPS_;
    std::unique_ptr<FrameBuffer>                bloom_;

private:

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

    // ---------- �萔�o�b�t�@�[ -------------------------
    struct BloomConstants
    {
        float bloomExtractionThreshold_ = 0.85f;    // �P�x���o臒l
        float bloomIntensity_           = 0.35f;    // �u���[�����x
        float dummy_[2]                 = {};       // �_�~�[
    };
    BloomConstants bloomConstants_ = {};
    std::unique_ptr<ConstantBuffer<BloomConstants>> constantBuffer_;
};

