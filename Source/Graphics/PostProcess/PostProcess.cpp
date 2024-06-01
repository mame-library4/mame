#include "PostProcess.h"
#include "Graphics.h"

// ----- コンストラクタ -----
PostProcess::PostProcess(const uint32_t& width, const uint32_t& height)
{
    renderer_ = std::make_unique<FullscreenQuad>();

    Graphics::Instance().CreatePsFromCso("./Resources/Shader/PostProcessPS.cso", postProcessPS_.GetAddressOf());

    postProcess_ = std::make_unique<FrameBuffer>(width, height);
}

// ----- デストラクタ -----
PostProcess::~PostProcess()
{
}

// ----- ポストプロセス開始 -----
void PostProcess::Activate()
{
    postProcess_->Clear();
    postProcess_->Activate();
}

// ----- ポストプロセス終了 -----
void PostProcess::Deactivate()
{
    postProcess_->Deactivate();
    bloom_.Execute(postProcess_->shaderResourceViews_[0].Get());
}

// ----- ポストプロセス描画 -----
void PostProcess::Draw()
{
    renderer_->Draw(bloom_.GetShaderResourceView(), 0, 1, postProcessPS_.Get());
}

// ----- ImGui用 -----
void PostProcess::DrawDebug()
{
    if (ImGui::TreeNode("PostProcess"))
    {
        ImGui::Image(reinterpret_cast<ImTextureID>(postProcess_->shaderResourceViews_[0].Get()), ImVec2(256.0, 256.0));

        bloom_.DrawDebug();

        ImGui::TreePop();
    }
}
