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

    // ----- bloom 実行関数 -----
    void Execute(ID3D11ShaderResourceView* colorMap);

    // ----- ImGui用 -----
    void DrawDebug();

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetColorMap() { return bloom_->shaderResourceViews_[0]; }

private:
    // ---------- 書き出し用 -----------------------------------
    std::unique_ptr<FullscreenQuad> renderer_;

    // ---------- 輝度抽出 -----------------------------------
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   luminancePS_;
    std::unique_ptr<FrameBuffer>                luminanceExtraction_;

    // ---------- ぼかし ( ブラー ) ------------------------------
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   gaussianBlurHorizontalPS_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   gaussianBlurVerticalPS_;
    static const int                            maxBlurBufferCount_ = 8;
    std::unique_ptr<FrameBuffer>                gaussianBlur_[maxBlurBufferCount_];

    // ---------- 最終結果 ( ブルーム ) ----------------------------------------
    Microsoft::WRL::ComPtr<ID3D11PixelShader>   bloomPS_;
    std::unique_ptr<FrameBuffer>                bloom_;

private:

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

    // ---------- 定数バッファー -------------------------
    struct BloomConstants
    {
        float bloomExtractionThreshold_ = 0.85f;    // 輝度抽出閾値
        float bloomIntensity_           = 0.05f;    // ブルーム強度
        float dummy_[2]                 = {};       // ダミー
    };
    std::unique_ptr<ConstantBuffer<BloomConstants>> constantBuffer_;
};

