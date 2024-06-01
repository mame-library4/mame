#include "PostProcess.h"
#include "Graphics.h"

// ----- �R���X�g���N�^ -----
PostProcess::PostProcess(const uint32_t& width, const uint32_t& height)
{
    renderer_ = std::make_unique<FullscreenQuad>();

    Graphics::Instance().CreatePsFromCso("./Resources/Shader/PostProcessPS.cso", postProcessPS_.GetAddressOf());

    postProcess_ = std::make_unique<FrameBuffer>(width, height);
}

// ----- �f�X�g���N�^ -----
PostProcess::~PostProcess()
{
}

// ----- �|�X�g�v���Z�X�J�n -----
void PostProcess::Activate()
{
    postProcess_->Clear();
    postProcess_->Activate();
}

// ----- �|�X�g�v���Z�X�I�� -----
void PostProcess::Deactivate()
{
    postProcess_->Deactivate();
    bloom_.Execute(postProcess_->shaderResourceViews_[0].Get());
}

// ----- �|�X�g�v���Z�X�`�� -----
void PostProcess::Draw()
{
    renderer_->Draw(bloom_.GetShaderResourceView(), 0, 1, postProcessPS_.Get());
}

// ----- ImGui�p -----
void PostProcess::DrawDebug()
{
    if (ImGui::TreeNode("PostProcess"))
    {
        ImGui::Image(reinterpret_cast<ImTextureID>(postProcess_->shaderResourceViews_[0].Get()), ImVec2(256.0, 256.0));

        bloom_.DrawDebug();

        ImGui::TreePop();
    }
}
