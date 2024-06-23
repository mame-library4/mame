#include "PostProcess.h"
#include "Graphics.h"
#include "Application.h"
#include "Character/Enemy/EnemyManager.h"

// ----- �R���X�g���N�^ -----
PostProcess::PostProcess()
{
    renderer_ = std::make_unique<FullscreenQuad>();

    Graphics::Instance().CreatePsFromCso("./Resources/Shader/PostProcessPS.cso", postProcessPS_.GetAddressOf());
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/PostProcessRoarPS.cso", roarPS_.GetAddressOf());

    postProcess_ = std::make_unique<FrameBuffer>(SCREEN_WIDTH, SCREEN_HEIGHT);

    constant_ = std::make_unique<ConstantBuffer<Constants>>();
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

// ----- ImGui�p -----
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
