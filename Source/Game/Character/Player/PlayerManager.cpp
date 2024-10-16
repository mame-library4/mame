#include "PlayerManager.h"

// ----- �����ݒ� -----
void PlayerManager::Initialize()
{
    player_->Initialize();
}

// ----- �I���� -----
void PlayerManager::Finalize()
{
    player_->Finalize();
}

// ----- �X�V -----
void PlayerManager::Update(const float& elapsedTime)
{
    // �q�b�g�X�g�b�v
    if (isHitStopActive_)
    {
        ++currentHitStopFrame_;

        if (currentHitStopFrame_ >= hitStopFrame_) isHitStopActive_ = false;

        // �q�b�g�X�g�b�v���Ȃ̂ł����ŏI��
        return;
    }
     
    player_->Update(elapsedTime);
}

// ----- �`�� -----
void PlayerManager::Render(ID3D11PixelShader* psShader)
{
    player_->Render(psShader);
}

void PlayerManager::CastShadow()
{
    player_->CastShadow();
}

void PlayerManager::RenderTrail()
{
    player_->RenderTrail();
}

// ----- ImGui�p -----
void PlayerManager::DrawDebug()
{
    if (ImGui::BeginMenu("Player"))
    {
        if (ImGui::TreeNode("HitStop"))
        {
            ImGui::DragInt("NormalHitStopFrame", &normalHitStopFrame_);
            ImGui::DragInt("CriticalHitStopFrame", &criticalHitStopFrame_);
            
            ImGui::TreePop();
        }

        ImGui::EndMenu();
    }
    player_->DrawDebug();
}

// ----- Debug�p -----
void PlayerManager::DebugRender(DebugRenderer* debugRenderer)
{
    player_->DebugRender(debugRenderer);
}

void PlayerManager::SetHitStop(const HitStopType& type)
{
    hitStopFrame_ = (type == HitStopType::Normal) ? normalHitStopFrame_ : criticalHitStopFrame_;
    currentHitStopFrame_ = 0;
    isHitStopActive_ = true;
}
