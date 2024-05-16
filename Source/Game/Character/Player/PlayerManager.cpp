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
    if (EnemyManager::Instance().GetEnemyCount() == 0) return;
    auto num = EnemyManager::Instance().GetEnemyCount();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    // プレイヤーの食らい判定処理 ( ダメージを受けたか )
    for (int playerDataIndex = 0; playerDataIndex < GetPlayer()->GetDamageDetectionDataCount(); ++playerDataIndex)
    {
        auto playerData = GetPlayer()->GetDamageDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetAttackDetectionDataCount(); ++enemyDataIndex)
        {
            auto enemyData = enemy->GetAttackDetectionData(enemyDataIndex);

            // 現在アクティブではないので処理をしない
            if (enemyData.GetIsActive() == false) continue;

            if (Collision::IntersectSphereVsSphere(
                enemyData.GetPosition(), enemyData.GetRadius(),
                playerData.GetPosition(), playerData.GetRadius()))
            {
                DirectX::XMFLOAT3 direction = {};
                direction = playerData.GetPosition() - enemy->GetTransform()->GetPosition();
                //direction = playerData.GetPosition() - enemyData.GetPosition();
                GetPlayer()->AddForce(direction, 0.2f);

                GetPlayer()->ChangeState(Player::STATE::Damage);
            }
        }
    }

    // 押し出し判定処理
    for (int playerDataIndex = 0; playerDataIndex < GetPlayer()->GetDamageDetectionDataCount(); ++playerDataIndex)
    {
        auto playerData = GetPlayer()->GetDamageDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetCollisionDetectionDataCount(); ++enemyDataIndex)
        {
            auto enemyData = enemy->GetCollisionDetectionData(enemyDataIndex);

            DirectX::XMFLOAT3 resultPos = {};
            if (Collision::IntersectSphereVsSphereNotConsiderY(
                enemyData.GetPosition(), enemyData.GetRadius(),
                playerData.GetPosition(), playerData.GetRadius(),
                resultPos))
            {
                GetTransform()->SetPosition(resultPos - playerData.GetOffsetPosition());
            }
        }
    }
}
