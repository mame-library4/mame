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
    if (isSkip_)
    {
        if (skipNum_ > 1)
        {
            isSkip_ = false;
        }
        ++skipNum_;
    }
    else
    {
        player_->Update(elapsedTime);
        skipNum_ = 0;
    }
}

// ----- 描画 -----
void PlayerManager::Render(ID3D11PixelShader* psShader)
{
    player_->Render(psShader);
}

void PlayerManager::RenderTrail()
{
    player_->RenderTrail();
}

// ----- ImGui用 -----
void PlayerManager::DrawDebug()
{
    player_->DrawDebug();
}

// ----- Debug用 -----
void PlayerManager::DebugRender(DebugRenderer* debugRenderer)
{
    player_->DebugRender(debugRenderer);
}