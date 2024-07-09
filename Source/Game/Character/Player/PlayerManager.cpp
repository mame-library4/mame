#include "PlayerManager.h"
#include "../Enemy/EnemyManager.h"
#include "../Game/Collision/Collision.h"
#include "../Other/MathHelper.h"

#include "UI/UINumber.h"
#include "UI/UIManager.h"

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
    player_->Update(elapsedTime);

    // �v���C���[�ƓG�̉����o������
    CollisionPlayerVsEnemy();
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

// ----- �v���C���[�ƓG�Ƃ̉����o������ -----
void PlayerManager::CollisionPlayerVsEnemy()
{
    if (EnemyManager::Instance().GetEnemyCount() == 0) return;
    auto num = EnemyManager::Instance().GetEnemyCount();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    // �v���C���[�̐H�炢���菈�� ( �_���[�W���󂯂��� )
    for (int playerDataIndex = 0; playerDataIndex < GetPlayer()->GetDamageDetectionDataCount(); ++playerDataIndex)
    {
        auto playerData = GetPlayer()->GetDamageDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetAttackDetectionDataCount(); ++enemyDataIndex)
        {
            auto enemyData = enemy->GetAttackDetectionData(enemyDataIndex);

            // ���݃A�N�e�B�u�ł͂Ȃ��̂ŏ��������Ȃ�
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

    // �����o�����菈��
    for (int playerDataIndex = 0; playerDataIndex < GetPlayer()->GetCollisionDetectionDataCount(); ++playerDataIndex)
    {
        auto playerData = GetPlayer()->GetCollisionDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetCollisionDetectionDataCount(); ++enemyDataIndex)
        {
            auto enemyData = enemy->GetCollisionDetectionData(enemyDataIndex);

            // ���݃A�N�e�B�u�ł͂Ȃ��̂ŏ��������Ȃ�
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

    // �v���C���[�̍U������
    {
        // �U�����肪�L���ł͂Ȃ��̂ŏI��
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

                        // �U����������
                        GetPlayer()->SetIsAbleAttack(false);

                        UINumber* ui = new UINumber(enemyData.GetDamage(), enemyData.GetPosition());

                        return;
                    }
                }
            }
        }
    }
}
