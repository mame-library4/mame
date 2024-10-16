#include "PlayerManager.h"

// ----- 初期設定 -----
void PlayerManager::Initialize()
{
    player_->Initialize();
}

// ----- 終了化 -----
void PlayerManager::Finalize()
{
    player_->Finalize();
}

// ----- 更新 -----
void PlayerManager::Update(const float& elapsedTime)
{
    // ヒットストップ
    if (isHitStopActive_)
    {
        ++currentHitStopFrame_;

        if (currentHitStopFrame_ >= hitStopFrame_) isHitStopActive_ = false;

        // ヒットストップ中なのでここで終了
        return;
    }
     
    player_->Update(elapsedTime);
}

// ----- 描画 -----
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

// ----- ImGui用 -----
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

// ----- Debug用 -----
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
