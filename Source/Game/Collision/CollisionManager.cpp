#include "CollisionManager.h"
#include "MathHelper.h"
#include "Character/Player/PlayerManager.h"
#include "Character/Enemy/EnemyManager.h"
#include "Projectile/ProjectileManager.h"
#include "UI/UINumber.h"

void CollisionManager::Initialize()
{
}

void CollisionManager::Finalize()
{
}

// ----- �X�V -----
void CollisionManager::Update(const float& elapsedTime)
{
    // Player �� Enemy �̔���
    UpdatePlayerVsEnemy();

    // Player �� Projectile �̔���
    UpdatePlayerVsProjectile();
}

// ----- Player��Enemy�̔��� -----
void CollisionManager::UpdatePlayerVsEnemy()
{
    // Enemy�����݂��Ȃ��ꍇ�͂����ŏI��
    if (EnemyManager::Instance().GetEnemyCount() == 0) return;

    // -------------------------------------------------------
    //   �������牺�̏����̓v���C���[���猩�������ɂȂ��Ă��܂�
    // -------------------------------------------------------
    // �U������
    UpdatePlayerAttackVsEnemyDamage();

    // �J�E���^�[����

    // ���炢����
    UpdatePlayerDamageVsEnemyAttack();

    // �����o������
    UpdatePlayerCollisionVsEnemyCollision();
}

// ----- Player�̍U������� Enemy�̂��炢������`�F�b�N -----
void CollisionManager::UpdatePlayerAttackVsEnemyDamage()
{
    // �U�����肪�����Ȃ̂ł����ŏI��
    if (PlayerManager::Instance().GetPlayer()->GetIsAbleAttack() == false) return;

    // �U���\�t���[���O�Ȃ̂ł����ŏI��
    if (PlayerManager::Instance().GetPlayer()->GetIsAttackValid() == false) return;

    Player* player = PlayerManager::Instance().GetPlayer().get();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    for (int playerDataIndex = 0; playerDataIndex < player->GetAttackDetectionDataCount(); ++playerDataIndex)
    {
        const AttackDetectionData playerData = player->GetAttackDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetDamageDetectionDataCount(); ++enemyDataIndex)
        {
            DamageDetectionData enemyData = enemy->GetDamageDetectionData(enemyDataIndex);

            // �������Ƀ_���[�W��H����Ă���f�[�^
            if (enemyData.GetIsHit()) continue;

            // �����������`�F�b�N
            if (IntersectSphereVsSphere(
                playerData.GetPosition(), playerData.GetRadius(),
                enemyData.GetPosition(), enemyData.GetRadius()))
            {
                // Hit�t���O�𗧂Ă�, ���̃f�[�^�̖��G���Ԑݒ�
                enemyData.SetIsHit(true);
                enemyData.SetHitTimer(0.01f);
                
                // TODO:�_���[�W����
                // �_���[�W����
                enemy->AddDamage(enemyData.GetDamage());
                
                // Player�̍U������𖳂���
                player->SetIsAbleAttack(false);

                // �G�������Ă�����DamageUI�𐶐�����
                if (PlayerManager::Instance().GetUseCollisionDetection())
                {
                    UINumber* ui = new UINumber(enemyData.GetDamage(), enemyData.GetPosition());
                }

                // TODO:�q�b�g�X�g�b�v����
                // �q�b�g�X�g�b�v

                // �J�E���^�[�U�����Ȃ�R���g���[���[��U��������
                if (player->GetCurrentState() == Player::STATE::CounterCombo)
                {
                    Input::Instance().GetGamePad().Vibration(0.3f, 1.0f);
                }

                // ���������̂Ŕ���������ŏI������
                return;
            }
        }
    }
}

// ----- Player�̂��炢����� Enemy�̍U��������`�F�b�N -----
void CollisionManager::UpdatePlayerDamageVsEnemyAttack()
{
    // �U�����肪�����Ȃ̂ł����ŏI��
    if (EnemyManager::Instance().GetEnemy(0)->GetIsAttackActive() == false) return;

    Player* player = PlayerManager::Instance().GetPlayer().get();

    // ���ݖ��G��ԂȂ̂Ŕ�����s��Ȃ�
    if (player->GetIsInvincible()) return;

    // �J�E���^�[�����������̂Ń_���[�W�������Ȃ�
    if (player->GetIsAbleCounterAttack()) return;

    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    // ���炢���� (������΂���鋯��)
    for (int playerDataIndex = 0; playerDataIndex < player->GetDamageDetectionDataCount(); ++playerDataIndex)
    {
        const DamageDetectionData playerData = player->GetDamageDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetAttackDetectionDataCount(); ++enemyDataIndex)
        {
            const AttackDetectionData enemyData = enemy->GetAttackDetectionData(enemyDataIndex);

            // ���̃f�[�^�̍U�����肪�L���ł͂Ȃ�
            if (enemyData.GetIsActive() == false) continue;

            // �����������`�F�b�N
            if (IntersectSphereVsSphere(
                playerData.GetPosition(), playerData.GetRadius(),
                enemyData.GetPosition(), enemyData.GetRadius()))
            {
                // �J�E���^�[��ԂȂ�J�E���^�[����
                if (player->GetIsCounter())
                {
                    player->SetIsAbleCounterAttack(true);
                    return;
                }

                // DamageState�ɑJ��
                player->ChangeState(Player::STATE::Damage);
                return;
            }
        }
    }
}

// ----- Player�̉����o������� Enemy�̉����o��������`�F�b�N -----
void CollisionManager::UpdatePlayerCollisionVsEnemyCollision()
{
    // �����o�����肪����
    if (PlayerManager::Instance().GetUseCollisionDetection() == false) return;

    Player* player = PlayerManager::Instance().GetPlayer().get();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    for (int playerDataIndex = 0; playerDataIndex < player->GetCollisionDetectionDataCount(); ++playerDataIndex)
    {
        const CollisionDetectionData playerData = player->GetCollisionDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetCollisionDetectionDataCount(); ++enemyDataIndex)
        {
            const CollisionDetectionData enemyData = enemy->GetCollisionDetectionData(enemyDataIndex);

            // ���̃f�[�^�̔��肪����
            if (enemyData.GetIsActive() == false) continue;

            DirectX::XMFLOAT3 resultPosition = {};

            // Y�̒l��0.0f�̃f�[�^�Ƃ̔���
            if (enemyData.GetFixedY())
            {
                // �����������`�F�b�N
                if (IntersectSphereVsSphere(
                    enemyData.GetPosition(), enemyData.GetRadius(),
                    player->GetTransform()->GetPosition(), playerData.GetRadius(),
                    resultPosition))
                {
                    resultPosition.y = 0.0f;

                    player->GetTransform()->SetPosition(resultPosition);
                }
            }
            // ���̑��̃f�[�^�Ƃ̔���
            else
            {
                // CollisionData�̈ʒu���X�V����
                player->UpdateCollisionDetectionData();

                // �����������`�F�b�N
                if (IntersectSphereVsSphereNotConsiderY(
                    enemyData.GetPosition(), enemyData.GetRadius(),
                    playerData.GetPosition(), playerData.GetRadius(),
                    resultPosition))
                {
                    resultPosition = player->GetTransform()->GetPosition() - resultPosition;
                    player->GetTransform()->SetPosition(resultPosition);
                }
            }
        }
    }
}

// ----- Player��Projectile�̔��� -----
void CollisionManager::UpdatePlayerVsProjectile()
{
    // -------------------------------------------------------
    //   �������牺�̏����̓v���C���[���猩�������ɂȂ��Ă��܂�
    // -------------------------------------------------------
    // �J�E���^�[����

    // ���炢����
    UpdatePlayerDamageVsProjectileAttack();
}

// ----- Player�̂��炢����� Projectile�̍U��������`�F�b�N -----
void CollisionManager::UpdatePlayerDamageVsProjectileAttack()
{
    Player* player = PlayerManager::Instance().GetPlayer().get();

    // ���ݖ��G��ԂȂ̂Ŕ�����s��Ȃ�
    if (player->GetIsInvincible()) return;

    // �J�E���^�[�����������̂Ń_���[�W�������Ȃ�
    if (player->GetIsAbleCounterAttack()) return;

    for (int playerDataIndex = 0; playerDataIndex < player->GetDamageDetectionDataCount(); ++playerDataIndex)
    {
        const DamageDetectionData playerData = player->GetDamageDetectionData(playerDataIndex);

        std::vector<Projectile*> projectiles = ProjectileManager::Instance().GetProjectiles();
        
        for(int projectileIndex = 0; projectileIndex < projectiles.size(); ++projectileIndex)
        {
            Projectile* projectile = projectiles.at(projectileIndex);

            // �����������`�F�b�N
            if (IntersectSphereVsSphere(
                playerData.GetPosition(), playerData.GetRadius(),
                projectile->GetTransform()->GetPosition(), projectile->GetRadius()))
            {
                // �J�E���^�[��ԂȂ�J�E���^�[����
                if (player->GetIsCounter())
                {
                    player->SetIsAbleCounterAttack(true);
                    return;
                }

                // DamageState�ɑJ��
                player->ChangeState(Player::STATE::Damage);
                return;
            }
        }

    }
}

// ---------- ���Ƌ��̌������� ----------
const bool CollisionManager::IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA, const float radiusA, const DirectX::XMFLOAT3& positionB, const float radiusB)
{
    const DirectX::XMFLOAT3 vec = positionB - positionA;
    const float lengthSq = XMFloat3LengthSq(vec);
    const float range = radiusA + radiusB;

    // �������Ă��Ȃ�
    if (lengthSq > range * range) return false;

    return true;
}

// ---------- ���Ƌ��̌������� �����o���L ----------
const bool CollisionManager::IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA, const float radiusA, const DirectX::XMFLOAT3& positionB, const float radiusB, DirectX::XMFLOAT3& outPositionB)
{
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    const float range = radiusA + radiusB;
    const float distXZ = sqrtf(vx * vx + vz * vz);

    // �������Ă��Ȃ�
    if (distXZ > range) return false;

    // ���K��
    vx /= distXZ;
    vz /= distXZ;

    outPositionB.x = positionA.x + (vx * range);
    outPositionB.y = positionB.y;
    outPositionB.z = positionA.z + (vz * range);

    return true;
}

// ---------- ���Ƌ��̌������� Y���W�̉����o�����l�����Ȃ� ----------
const bool CollisionManager::IntersectSphereVsSphereNotConsiderY(const DirectX::XMFLOAT3& positionA, const float radiusA, const DirectX::XMFLOAT3& positionB, const float radiusB, DirectX::XMFLOAT3& outPositionB)
{
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionA, PositionB);
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    const float range = radiusA + radiusB;    
    // �������Ă��Ȃ�
    if (lengthSq > range * range) return false;

    DirectX::XMVECTOR HorizonVec = DirectX::XMVectorSetY(Vec, 0.0f);
    
    const float lengthHorizon = DirectX::XMVectorGetX(DirectX::XMVector3Length(HorizonVec));
    const float subY = DirectX::XMVectorGetY(Vec);
    const float newHorizonLength = sqrtf(range * range - subY * subY);
    const DirectX::XMVECTOR normalVec = DirectX::XMVector3Normalize(HorizonVec);
    const float penetration = newHorizonLength - lengthHorizon;
    
    DirectX::XMStoreFloat3(&outPositionB, DirectX::XMVectorScale(normalVec, penetration));

    return true;
}

// ---------- �~���Ɖ~���̌������� ----------
const bool CollisionManager::IntersectCylinderVsCylinder(const DirectX::XMFLOAT3& positionA, const float& radiusA, const float& heightA, const DirectX::XMFLOAT3& positionB, const float& radiusB, const float& heightB, DirectX::XMFLOAT3& outPosition)
{
    // A�̑����� B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (positionA.y > positionB.y + heightB) return false;

    // A�̓��� B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (positionA.y + heightA < positionB.y) return false;

    // XZ���ʂł̓����蔻��
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx * vx + vz * vz);
    vx /= distXZ;
    vz /= distXZ;

    // �������Ă��Ȃ�
    if (distXZ > range) return false;

    // A�� B�������o��
    outPosition.x = positionA.x + (vx * range);
    outPosition.y = positionB.y;
    outPosition.z = positionA.z + (vz * range);

    return true;
}

// ---------- ���ƒ����̌������� ----------
const bool CollisionManager::IntersectSphereVsRay(const DirectX::XMFLOAT3& spherePos, const float radius, const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, const float length)
{
    DirectX::XMFLOAT3 vec = spherePos - rayStart;

    // �����̕����Ƌt�����ɋ����������ꍇ�͏������Ȃ�
    if (XMFloat3Dot(XMFloat3Normalize(vec), rayDirection) < 0)return false;

    // �����̒����������܂ł̋��������������ꍇ�������Ȃ�
    if (XMFloat3Length(vec) > length)return false;

    // �������狅�܂ł̍ŒZ����
    float projectionLength = XMFloat3Dot(vec, rayDirection);

    const DirectX::XMFLOAT3 p1 = rayStart + rayDirection * projectionLength;
    float nearestLength = XMFloat3Length(p1 - spherePos);

    // ���̋��������̔��a�����Z���Ȃ�������Ă���
    if (nearestLength <= radius)
    {
        return true;
    }

    return false;
}

// ---------- ���ƒ����̌������� ----------
const bool CollisionManager::IntersectSphereVsCapsule(const DirectX::XMFLOAT3& spherePos, const float sphereRadius, const DirectX::XMFLOAT3& capsuleStart, const DirectX::XMFLOAT3& capsuleDirection, const float length, const float capsuleRadius)
{
    DirectX::XMFLOAT3 vec = spherePos - capsuleStart;

    // �����̕����Ƌt�����ɋ����������ꍇ�͏������Ȃ�
    if (XMFloat3Dot(XMFloat3Normalize(vec), capsuleDirection) < 0)return false;

    // �����̒����������܂ł̋��������������ꍇ�������Ȃ�
    if (XMFloat3Length(vec) > length + capsuleRadius)return false;

    // �������狅�܂ł̍ŒZ����
    float projectionLength = XMFloat3Dot(vec, capsuleDirection);

    const DirectX::XMFLOAT3 p1 = capsuleStart + capsuleDirection * projectionLength;
    float nearestLength = XMFloat3Length(p1 - spherePos);

    // ���̋��������̔��a�����Z���Ȃ�������Ă���
    if (nearestLength <= sphereRadius + capsuleRadius)
    {
        return true;
    }

    return false;
}
