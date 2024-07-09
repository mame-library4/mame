#include "PlayerManager.h"
#include "../Enemy/EnemyManager.h"
#include "../Game/Collision/Collision.h"
#include "../Other/MathHelper.h"

#include "UI/UINumber.h"
#include "UI/UIManager.h"

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
    for (int playerDataIndex = 0; playerDataIndex < GetPlayer()->GetCollisionDetectionDataCount(); ++playerDataIndex)
    {
        auto playerData = GetPlayer()->GetCollisionDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetCollisionDetectionDataCount(); ++enemyDataIndex)
        {
            auto enemyData = enemy->GetCollisionDetectionData(enemyDataIndex);

            // 現在アクティブではないので処理をしない
            if (enemyData.GetIsActive() == false) continue;

            DirectX::XMFLOAT3 resultPos = {};
            if (Collision::IntersectSphereVsSphereNotConsiderY(
                enemyData.GetPosition(), enemyData.GetRadius(),
                playerData.GetPosition(), playerData.GetRadius(),
                resultPos))
            {
                GetTransform()->SetPosition(GetTransform()->GetPosition() - resultPos);// -playerData.GetOffsetPosition());
                //GetTransform()->SetPosition(resultPos - playerData.GetOffsetPosition());
            }
        }
    }

    // プレイヤーの攻撃判定
    {
        // 攻撃判定が有効ではないので終了
        if (GetPlayer()->GetIsAbleAttack() == false) return;

        for (int playerDataIndex = 0; playerDataIndex < GetPlayer()->GetAttackDetectionDataCount(); ++playerDataIndex)
        {
            auto playerData = GetPlayer()->GetAttackDetectionData(playerDataIndex);

            for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetDamageDetectionDataCount(); ++enemyDataIndex)
            {
                auto enemyData = enemy->GetDamageDetectionData(enemyDataIndex);

                if (Collision::IntersectSphereVsSphere(
                    enemyData.GetPosition(), enemyData.GetRadius(),
                    playerData.GetPosition(), playerData.GetRadius()))
                {
                    if (enemyData.GetIsHit() == false)
                    {
                        enemy->GetDamageDetectionData(enemyDataIndex).SetIsHit(true);
                        enemy->GetDamageDetectionData(enemyDataIndex).SetHitTimer(0.01f);
                        enemy->AddDamage(enemyData.GetDamage());

                        // 攻撃当たった
                        GetPlayer()->SetIsAbleAttack(false);

                        UINumber* ui = new UINumber(enemyData.GetDamage(), enemyData.GetPosition());

                        return;
                    }
                }
            }
        }
    }
}
