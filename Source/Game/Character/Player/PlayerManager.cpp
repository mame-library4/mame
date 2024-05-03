#include "PlayerManager.h"
#include "../Enemy/EnemyManager.h"
#include "../Game/Collision/Collision.h"
#include "../Other/MathHelper.h"

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
void PlayerManager::Render()
{
    player_->Render();
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
