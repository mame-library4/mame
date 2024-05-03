#include "PlayerManager.h"
#include "../Enemy/EnemyManager.h"
#include "../Game/Collision/Collision.h"
#include "../Other/MathHelper.h"

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
    player_->Update(elapsedTime);

    // プレイヤーと敵の押し出し判定
    CollisionPlayerVsEnemy();
}

// ----- 描画 -----
void PlayerManager::Render()
{
    player_->Render();
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

// ----- プレイヤーと敵との押し出し判定 -----
void PlayerManager::CollisionPlayerVsEnemy()
{
    const int enemyCount = EnemyManager::Instance().GetEnemyCount();
    for (int enemyIndex = 0; enemyIndex < enemyCount; ++enemyIndex)
    {
        auto playerData = GetPlayer()->GetCollisionCylinderData("collide");

        Enemy* enemy = EnemyManager::Instance().GetEnemy(enemyIndex);
        const int dataMax = enemy->GetCollisionCylinderDataCount();

        for (int dataIndex = 0; dataIndex < dataMax; ++dataIndex)
        {
            auto data = enemy->GetCollisionCylinderData(dataIndex);
            DirectX::XMFLOAT3 enemyPos = data.GetPosition();

            DirectX::XMFLOAT3 resultPos = {};
            if (Collision::IntersectCylinderVsCylinder(
                enemyPos, data.GetRadius(), data.GetHeight(),
                GetTransform()->GetPosition(), playerData.GetRadius(), playerData.GetHeight(),
                resultPos))
            {
                GetTransform()->SetPosition(resultPos);
            }
        }
    }
}
