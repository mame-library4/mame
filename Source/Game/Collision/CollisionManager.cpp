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

// ----- 更新 -----
void CollisionManager::Update(const float& elapsedTime)
{
    // Player と Enemy の判定
    UpdatePlayerVsEnemy();

    // Player と Projectile の判定
    UpdatePlayerVsProjectile();
}

// ----- PlayerとEnemyの判定 -----
void CollisionManager::UpdatePlayerVsEnemy()
{
    // Enemyが存在しない場合はここで終了
    if (EnemyManager::Instance().GetEnemyCount() == 0) return;

    // -------------------------------------------------------
    //   ここから下の処理はプレイヤーから見た処理になっています
    // -------------------------------------------------------
    // 攻撃判定
    UpdatePlayerAttackVsEnemyDamage();

    // カウンター判定

    // くらい判定
    UpdatePlayerDamageVsEnemyAttack();

    // 押し出し判定
    UpdatePlayerCollisionVsEnemyCollision();
}

// ----- Playerの攻撃判定と Enemyのくらい判定をチェック -----
void CollisionManager::UpdatePlayerAttackVsEnemyDamage()
{
    // 攻撃判定が無効なのでここで終了
    if (PlayerManager::Instance().GetPlayer()->GetIsAbleAttack() == false) return;

    // 攻撃可能フレーム外なのでここで終了
    if (PlayerManager::Instance().GetPlayer()->GetIsAttackValid() == false) return;

    Player* player = PlayerManager::Instance().GetPlayer().get();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    for (int playerDataIndex = 0; playerDataIndex < player->GetAttackDetectionDataCount(); ++playerDataIndex)
    {
        const AttackDetectionData playerData = player->GetAttackDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetDamageDetectionDataCount(); ++enemyDataIndex)
        {
            DamageDetectionData enemyData = enemy->GetDamageDetectionData(enemyDataIndex);

            // もう既にダメージを食らっているデータ
            if (enemyData.GetIsHit()) continue;

            // 当たったかチェック
            if (IntersectSphereVsSphere(
                playerData.GetPosition(), playerData.GetRadius(),
                enemyData.GetPosition(), enemyData.GetRadius()))
            {
                // Hitフラグを立てる, このデータの無敵時間設定
                enemyData.SetIsHit(true);
                enemyData.SetHitTimer(0.01f);
                
                // TODO:ダメージ処理
                // ダメージ処理
                enemy->AddDamage(enemyData.GetDamage());
                
                // Playerの攻撃判定を無くす
                player->SetIsAbleAttack(false);

                // 敵が生きていたらDamageUIを生成する
                if (PlayerManager::Instance().GetUseCollisionDetection())
                {
                    UINumber* ui = new UINumber(enemyData.GetDamage(), enemyData.GetPosition());
                }

                // TODO:ヒットストップ処理
                // ヒットストップ

                // カウンター攻撃時ならコントローラーを振動させる
                if (player->GetCurrentState() == Player::STATE::CounterCombo)
                {
                    Input::Instance().GetGamePad().Vibration(0.3f, 1.0f);
                }

                // 当たったので判定をここで終了する
                return;
            }
        }
    }
}

// ----- Playerのくらい判定と Enemyの攻撃判定をチェック -----
void CollisionManager::UpdatePlayerDamageVsEnemyAttack()
{
    // 攻撃判定が無効なのでここで終了
    if (EnemyManager::Instance().GetEnemy(0)->GetIsAttackActive() == false) return;

    Player* player = PlayerManager::Instance().GetPlayer().get();

    // 現在無敵状態なので判定を行わない
    if (player->GetIsInvincible()) return;

    // カウンターが成功したのでダメージをくらわない
    if (player->GetIsAbleCounterAttack()) return;

    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    // くらい判定 (吹っ飛ばされる怯み)
    for (int playerDataIndex = 0; playerDataIndex < player->GetDamageDetectionDataCount(); ++playerDataIndex)
    {
        const DamageDetectionData playerData = player->GetDamageDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetAttackDetectionDataCount(); ++enemyDataIndex)
        {
            const AttackDetectionData enemyData = enemy->GetAttackDetectionData(enemyDataIndex);

            // このデータの攻撃判定が有効ではない
            if (enemyData.GetIsActive() == false) continue;

            // 当たったかチェック
            if (IntersectSphereVsSphere(
                playerData.GetPosition(), playerData.GetRadius(),
                enemyData.GetPosition(), enemyData.GetRadius()))
            {
                // カウンター状態ならカウンター成功
                if (player->GetIsCounter())
                {
                    player->SetIsAbleCounterAttack(true);
                    return;
                }

                // DamageStateに遷移
                player->ChangeState(Player::STATE::Damage);
                return;
            }
        }
    }
}

// ----- Playerの押し出し判定と Enemyの押し出し判定をチェック -----
void CollisionManager::UpdatePlayerCollisionVsEnemyCollision()
{
    // 押し出し判定が無効
    if (PlayerManager::Instance().GetUseCollisionDetection() == false) return;

    Player* player = PlayerManager::Instance().GetPlayer().get();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    for (int playerDataIndex = 0; playerDataIndex < player->GetCollisionDetectionDataCount(); ++playerDataIndex)
    {
        const CollisionDetectionData playerData = player->GetCollisionDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetCollisionDetectionDataCount(); ++enemyDataIndex)
        {
            const CollisionDetectionData enemyData = enemy->GetCollisionDetectionData(enemyDataIndex);

            // このデータの判定が無効
            if (enemyData.GetIsActive() == false) continue;

            DirectX::XMFLOAT3 resultPosition = {};

            // Yの値が0.0fのデータとの判定
            if (enemyData.GetFixedY())
            {
                // 当たったかチェック
                if (IntersectSphereVsSphere(
                    enemyData.GetPosition(), enemyData.GetRadius(),
                    player->GetTransform()->GetPosition(), playerData.GetRadius(),
                    resultPosition))
                {
                    resultPosition.y = 0.0f;

                    player->GetTransform()->SetPosition(resultPosition);
                }
            }
            // その他のデータとの判定
            else
            {
                // CollisionDataの位置を更新する
                player->UpdateCollisionDetectionData();

                // 当たったかチェック
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

// ----- PlayerとProjectileの判定 -----
void CollisionManager::UpdatePlayerVsProjectile()
{
    // -------------------------------------------------------
    //   ここから下の処理はプレイヤーから見た処理になっています
    // -------------------------------------------------------
    // カウンター判定

    // くらい判定
    UpdatePlayerDamageVsProjectileAttack();
}

// ----- Playerのくらい判定と Projectileの攻撃判定をチェック -----
void CollisionManager::UpdatePlayerDamageVsProjectileAttack()
{
    Player* player = PlayerManager::Instance().GetPlayer().get();

    // 現在無敵状態なので判定を行わない
    if (player->GetIsInvincible()) return;

    // カウンターが成功したのでダメージをくらわない
    if (player->GetIsAbleCounterAttack()) return;

    for (int playerDataIndex = 0; playerDataIndex < player->GetDamageDetectionDataCount(); ++playerDataIndex)
    {
        const DamageDetectionData playerData = player->GetDamageDetectionData(playerDataIndex);

        std::vector<Projectile*> projectiles = ProjectileManager::Instance().GetProjectiles();
        
        for(int projectileIndex = 0; projectileIndex < projectiles.size(); ++projectileIndex)
        {
            Projectile* projectile = projectiles.at(projectileIndex);

            // 当たったかチェック
            if (IntersectSphereVsSphere(
                playerData.GetPosition(), playerData.GetRadius(),
                projectile->GetTransform()->GetPosition(), projectile->GetRadius()))
            {
                // カウンター状態ならカウンター成功
                if (player->GetIsCounter())
                {
                    player->SetIsAbleCounterAttack(true);
                    return;
                }

                // DamageStateに遷移
                player->ChangeState(Player::STATE::Damage);
                return;
            }
        }

    }
}

// ---------- 球と球の交差判定 ----------
const bool CollisionManager::IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA, const float radiusA, const DirectX::XMFLOAT3& positionB, const float radiusB)
{
    const DirectX::XMFLOAT3 vec = positionB - positionA;
    const float lengthSq = XMFloat3LengthSq(vec);
    const float range = radiusA + radiusB;

    // 当たっていない
    if (lengthSq > range * range) return false;

    return true;
}

// ---------- 球と球の交差判定 押し出し有 ----------
const bool CollisionManager::IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA, const float radiusA, const DirectX::XMFLOAT3& positionB, const float radiusB, DirectX::XMFLOAT3& outPositionB)
{
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    const float range = radiusA + radiusB;
    const float distXZ = sqrtf(vx * vx + vz * vz);

    // 当たっていない
    if (distXZ > range) return false;

    // 正規化
    vx /= distXZ;
    vz /= distXZ;

    outPositionB.x = positionA.x + (vx * range);
    outPositionB.y = positionB.y;
    outPositionB.z = positionA.z + (vz * range);

    return true;
}

// ---------- 球と球の交差判定 Y座標の押し出しを考慮しない ----------
const bool CollisionManager::IntersectSphereVsSphereNotConsiderY(const DirectX::XMFLOAT3& positionA, const float radiusA, const DirectX::XMFLOAT3& positionB, const float radiusB, DirectX::XMFLOAT3& outPositionB)
{
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionA, PositionB);
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    const float range = radiusA + radiusB;    
    // 当たっていない
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

// ---------- 円柱と円柱の交差判定 ----------
const bool CollisionManager::IntersectCylinderVsCylinder(const DirectX::XMFLOAT3& positionA, const float& radiusA, const float& heightA, const DirectX::XMFLOAT3& positionB, const float& radiusB, const float& heightB, DirectX::XMFLOAT3& outPosition)
{
    // Aの足元が Bの頭より上なら当たっていない
    if (positionA.y > positionB.y + heightB) return false;

    // Aの頭が Bの足元より下なら当たっていない
    if (positionA.y + heightA < positionB.y) return false;

    // XZ平面での当たり判定
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx * vx + vz * vz);
    vx /= distXZ;
    vz /= distXZ;

    // 当たっていない
    if (distXZ > range) return false;

    // Aが Bを押し出す
    outPosition.x = positionA.x + (vx * range);
    outPosition.y = positionB.y;
    outPosition.z = positionA.z + (vz * range);

    return true;
}

// ---------- 球と直線の交差判定 ----------
const bool CollisionManager::IntersectSphereVsRay(const DirectX::XMFLOAT3& spherePos, const float radius, const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, const float length)
{
    DirectX::XMFLOAT3 vec = spherePos - rayStart;

    // 光線の方向と逆方向に球があった場合は処理しない
    if (XMFloat3Dot(XMFloat3Normalize(vec), rayDirection) < 0)return false;

    // 光線の長さよりも球までの距離が長かった場合処理しない
    if (XMFloat3Length(vec) > length)return false;

    // 光線から球までの最短距離
    float projectionLength = XMFloat3Dot(vec, rayDirection);

    const DirectX::XMFLOAT3 p1 = rayStart + rayDirection * projectionLength;
    float nearestLength = XMFloat3Length(p1 - spherePos);

    // この距離が球の半径よりも短いなら交差している
    if (nearestLength <= radius)
    {
        return true;
    }

    return false;
}

// ---------- 球と直線の交差判定 ----------
const bool CollisionManager::IntersectSphereVsCapsule(const DirectX::XMFLOAT3& spherePos, const float sphereRadius, const DirectX::XMFLOAT3& capsuleStart, const DirectX::XMFLOAT3& capsuleDirection, const float length, const float capsuleRadius)
{
    DirectX::XMFLOAT3 vec = spherePos - capsuleStart;

    // 光線の方向と逆方向に球があった場合は処理しない
    if (XMFloat3Dot(XMFloat3Normalize(vec), capsuleDirection) < 0)return false;

    // 光線の長さよりも球までの距離が長かった場合処理しない
    if (XMFloat3Length(vec) > length + capsuleRadius)return false;

    // 光線から球までの最短距離
    float projectionLength = XMFloat3Dot(vec, capsuleDirection);

    const DirectX::XMFLOAT3 p1 = capsuleStart + capsuleDirection * projectionLength;
    float nearestLength = XMFloat3Length(p1 - spherePos);

    // この距離が球の半径よりも短いなら交差している
    if (nearestLength <= sphereRadius + capsuleRadius)
    {
        return true;
    }

    return false;
}
