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

// ----- �`�� -----
void PlayerManager::Render(ID3D11PixelShader* psShader)
{
    player_->Render(psShader);
}

void PlayerManager::RenderTrail()
{
    player_->RenderTrail();
}

// ----- ImGui�p -----
void PlayerManager::DrawDebug()
{
    player_->DrawDebug();
}

// ----- Debug�p -----
void PlayerManager::DebugRender(DebugRenderer* debugRenderer)
{
    player_->DebugRender(debugRenderer);
}