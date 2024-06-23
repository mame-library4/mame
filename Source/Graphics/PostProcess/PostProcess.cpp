#include "PostProcess.h"
#include "Graphics.h"
#include "Application.h"
#include "Character/Enemy/EnemyManager.h"

// ----- コンストラクタ -----
PostProcess::PostProcess()
{
    renderer_ = std::make_unique<FullscreenQuad>();

    Graphics::Instance().CreatePsFromCso("./Resources/Shader/PostProcessPS.cso", postProcessPS_.GetAddressOf());
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/PostProcessRoarPS.cso", roarPS_.GetAddressOf());

    postProcess_ = std::make_unique<FrameBuffer>(SCREEN_WIDTH, SCREEN_HEIGHT);

    constant_ = std::make_unique<ConstantBuffer<Constants>>();
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
    constant_->Activate(0);

    if (EnemyManager::Instance().GetEnemy(0)->GetIsRoar())
    {
        renderer_->Draw(bloom_.GetShaderResourceView(), 0, 1, roarPS_.Get());
    }
    else
    {
      renderer_->Draw(bloom_.GetShaderResourceView(), 0, 1, postProcessPS_.Get());
    }

}

// ----- ImGui用 -----
void PostProcess::DrawDebug()
{
    if (ImGui::TreeNode("PostProcess"))
    {
        ImGui::DragFloat("blurPower", &constant_->GetData()->blurPower_, 0.01f);
        
        ImGui::Image(reinterpret_cast<ImTextureID>(postProcess_->shaderResourceViews_[0].Get()), ImVec2(256.0, 256.0));

        bloom_.DrawDebug();

        ImGui::TreePop();
    }
}
