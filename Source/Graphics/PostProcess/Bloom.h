#pragma once
#include <d3d11.h>
#include <memory>
#include <wrl.h>
#include "ConstantBuffer.h"
#include "FrameBuffer.h"
#include "FullscreenQuad.h"

class Bloom
{
public:
    Bloom();
    ~Bloom() = default;

    // ----- bloom ���s�֐� -----
    void Execute(ID3D11ShaderResourceView* colorMap);

    // ----- ImGui�p -----
    void DrawDebug();

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetColorMap() { return bloom_->shaderResourceViews_[0]; }

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
        float bloomIntensity_           = 0.05f;    // �u���[�����x
        float dummy_[2]                 = {};       // �_�~�[
    };
    std::unique_ptr<ConstantBuffer<BloomConstants>> constantBuffer_;
};

