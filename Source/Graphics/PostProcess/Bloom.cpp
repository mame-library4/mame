#include "Bloom.h"
#include "Graphics.h"
#include "Application.h"

// ----- コンストラクタ -----
Bloom::Bloom()
{
    // ピクセルシェーダーオブジェクト生成
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/LuminanceExtractionPS.cso", luminancePS_.GetAddressOf());
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/GaussianBlurHorizontalPS.cso", gaussianBlurHorizontalPS_.GetAddressOf());
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/GaussianBlurVerticalPS.cso", gaussianBlurVerticalPS_.GetAddressOf());
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/BloomPS.cso", bloomPS_.GetAddressOf());

    // FrameBufferなど生成
    renderer_ = std::make_unique<FullscreenQuad>();
    const uint32_t width = SCREEN_WIDTH;
    const uint32_t height = SCREEN_HEIGHT;
    luminanceExtraction_ = std::make_unique<FrameBuffer>(width, height);
    int shiftNum = 1;
    for (int downSamplingIndex = 0; downSamplingIndex < maxBlurBufferCount_; downSamplingIndex += 2)
    {
        // 横ダウンサンプリング
        gaussianBlur_[downSamplingIndex] = std::make_unique<FrameBuffer>(width >> shiftNum, height >> max(0, shiftNum - 1));

        // 縦ダウンサンプリング
        gaussianBlur_[downSamplingIndex + 1] = std::make_unique<FrameBuffer>(width >> shiftNum, height >> shiftNum);

        ++shiftNum;
    }
    bloom_ = std::make_unique<FrameBuffer>(width, height);

    // 定数バッファー作成
    constantBuffer_ = std::make_unique<ConstantBuffer<BloomConstants>>();
}

// ----- bloom実行 -----
void Bloom::Execute(ID3D11ShaderResourceView* colorMap)
{
    // 定数バッファー更新
    constantBuffer_->GetData()->bloomExtractionThreshold_ = bloomConstants_.bloomExtractionThreshold_;
    constantBuffer_->GetData()->bloomIntensity_ = bloomConstants_.bloomIntensity_;
    constantBuffer_->Activate(0);

    // 輝度を抽出
    luminanceExtraction_->Clear();
    luminanceExtraction_->Activate();
    renderer_->Draw(&colorMap, 0, 1, luminancePS_.Get());
    luminanceExtraction_->Deactivate();

    // ブラー ( ダウンサンプリング )
    for (int downSamplingIndex = 0; downSamplingIndex < maxBlurBufferCount_; downSamplingIndex += 2)
    {
        // 横方向にブラー ( 横方向 ダウンサンプリング )
        gaussianBlur_[downSamplingIndex]->Clear();
        gaussianBlur_[downSamplingIndex]->Activate();
        renderer_->Draw(luminanceExtraction_->shaderResourceViews_[0].GetAddressOf(), 0, 1, gaussianBlurHorizontalPS_.Get());
        gaussianBlur_[downSamplingIndex]->Deactivate();

        // 縦方向にブラー ( 縦方向 ダウンサンプリング )
        gaussianBlur_[downSamplingIndex + 1]->Clear();
        gaussianBlur_[downSamplingIndex + 1]->Activate();
        renderer_->Draw(gaussianBlur_[downSamplingIndex]->shaderResourceViews_[0].GetAddressOf(), 0, 1, gaussianBlurVerticalPS_.Get());
        gaussianBlur_[downSamplingIndex + 1]->Deactivate();
    }

    // ブルーム実行
    ID3D11ShaderResourceView* shaderResourceViews[] =
    {
        colorMap,
        gaussianBlur_[1]->shaderResourceViews_[0].Get(),
        gaussianBlur_[3]->shaderResourceViews_[0].Get(),
        gaussianBlur_[5]->shaderResourceViews_[0].Get(),
        gaussianBlur_[7]->shaderResourceViews_[0].Get(),
    };
    bloom_->Clear();
    bloom_->Activate();
    renderer_->Draw(shaderResourceViews, 0, 5, bloomPS_.Get());
    bloom_->Deactivate();
}

// ----- ImGui用 -----
void Bloom::DrawDebug()
{
    if (ImGui::TreeNode("Bloom"))
    {
        ImGui::DragFloat("ExtractionThreshold", &bloomConstants_.bloomExtractionThreshold_, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Intencity", &bloomConstants_.bloomIntensity_, 0.01f, 0.0f, 1.0f);

        if (ImGui::TreeNode("Textures"))
        {
            ImGui::Image(reinterpret_cast<ImTextureID>(luminanceExtraction_->shaderResourceViews_[0].Get()), ImVec2(256.0, 256.0));
            ImGui::Image(reinterpret_cast<ImTextureID>(gaussianBlur_[1]->shaderResourceViews_[0].Get()), ImVec2(256.0, 256.0));
            ImGui::Image(reinterpret_cast<ImTextureID>(gaussianBlur_[3]->shaderResourceViews_[0].Get()), ImVec2(256.0, 256.0));
            ImGui::Image(reinterpret_cast<ImTextureID>(gaussianBlur_[5]->shaderResourceViews_[0].Get()), ImVec2(256.0, 256.0));
            ImGui::Image(reinterpret_cast<ImTextureID>(gaussianBlur_[7]->shaderResourceViews_[0].Get()), ImVec2(256.0, 256.0));
            ImGui::Image(reinterpret_cast<ImTextureID>(bloom_->shaderResourceViews_[0].Get()), ImVec2(256.0, 256.0));

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}
