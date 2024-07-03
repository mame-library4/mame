#include "Bloom.h"
#include "Graphics.h"
#include "Application.h"

// ----- �R���X�g���N�^ -----
Bloom::Bloom()
{
    // �s�N�Z���V�F�[�_�[�I�u�W�F�N�g����
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/LuminanceExtractionPS.cso", luminancePS_.GetAddressOf());
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/GaussianBlurHorizontalPS.cso", gaussianBlurHorizontalPS_.GetAddressOf());
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/GaussianBlurVerticalPS.cso", gaussianBlurVerticalPS_.GetAddressOf());
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/BloomPS.cso", bloomPS_.GetAddressOf());

    // FrameBuffer�Ȃǐ���
    renderer_ = std::make_unique<FullscreenQuad>();
    const uint32_t width = SCREEN_WIDTH;
    const uint32_t height = SCREEN_HEIGHT;
    luminanceExtraction_ = std::make_unique<FrameBuffer>(width, height);
    int shiftNum = 1;
    for (int downSamplingIndex = 0; downSamplingIndex < maxBlurBufferCount_; downSamplingIndex += 2)
    {
        // ���_�E���T���v�����O
        gaussianBlur_[downSamplingIndex] = std::make_unique<FrameBuffer>(width >> shiftNum, height >> max(0, shiftNum - 1));

        // �c�_�E���T���v�����O
        gaussianBlur_[downSamplingIndex + 1] = std::make_unique<FrameBuffer>(width >> shiftNum, height >> shiftNum);

        ++shiftNum;
    }
    bloom_ = std::make_unique<FrameBuffer>(width, height);

    // �萔�o�b�t�@�[�쐬
    constantBuffer_ = std::make_unique<ConstantBuffer<BloomConstants>>();
}

// ----- bloom���s -----
void Bloom::Execute(ID3D11ShaderResourceView* colorMap)
{
    // �萔�o�b�t�@�[�X�V
    constantBuffer_->GetData()->bloomExtractionThreshold_ = bloomConstants_.bloomExtractionThreshold_;
    constantBuffer_->GetData()->bloomIntensity_ = bloomConstants_.bloomIntensity_;
    constantBuffer_->Activate(0);

    // �P�x�𒊏o
    luminanceExtraction_->Clear();
    luminanceExtraction_->Activate();
    renderer_->Draw(&colorMap, 0, 1, luminancePS_.Get());
    luminanceExtraction_->Deactivate();

    // �u���[ ( �_�E���T���v�����O )
    for (int downSamplingIndex = 0; downSamplingIndex < maxBlurBufferCount_; downSamplingIndex += 2)
    {
        // �������Ƀu���[ ( ������ �_�E���T���v�����O )
        gaussianBlur_[downSamplingIndex]->Clear();
        gaussianBlur_[downSamplingIndex]->Activate();
        renderer_->Draw(luminanceExtraction_->shaderResourceViews_[0].GetAddressOf(), 0, 1, gaussianBlurHorizontalPS_.Get());
        gaussianBlur_[downSamplingIndex]->Deactivate();

        // �c�����Ƀu���[ ( �c���� �_�E���T���v�����O )
        gaussianBlur_[downSamplingIndex + 1]->Clear();
        gaussianBlur_[downSamplingIndex + 1]->Activate();
        renderer_->Draw(gaussianBlur_[downSamplingIndex]->shaderResourceViews_[0].GetAddressOf(), 0, 1, gaussianBlurVerticalPS_.Get());
        gaussianBlur_[downSamplingIndex + 1]->Deactivate();
    }

    // �u���[�����s
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

// ----- ImGui�p -----
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
