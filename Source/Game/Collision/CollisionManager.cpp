#include "CollisionManager.h"
#include "MathHelper.h"
#include "Character/Player/PlayerManager.h"
#include "Character/Enemy/EnemyManager.h"
#include "Character/Enemy/EnemyDragon.h"
#include "Projectile/ProjectileManager.h"
#include "UI/UINumber.h"
#include "AudioManager.h"
#include "Item/ItemManager.h"
#include "Camera.h"

void CollisionManager::Initialize()
{
}

void CollisionManager::Finalize()
{
}

// ----- 更新 -----
void CollisionManager::Update(const float& elapsedTime)
{
    // Player VS 〇〇
    UpdatePlayerVs();

    // Enemy VS 〇〇
    UpdateEnemyVs();

    // Projectile VS 〇〇
    UpdateProjectileVs();


    // アイテム と 〇〇 の判定
    UpdateItemVs();

    if (PlayerManager::Instance().GetPlayer()->GetCurrentState() == Player::STATE::RushAttack) return;
    for (int i = 0; i < maxEffectHandle_; ++i)
    {
        const std::string effectName = effectHandle_[i].name_;

        if (effectName == "") continue;

        EffectManager::Instance().GetEffect(effectName)->SetSpeed(effectHandle_[i].effectHandle_, 1.0f);
    }
}

void CollisionManager::DrawDebug()
{
    if (ImGui::TreeNode("CollisionManager"))
    {
        if (ImGui::TreeNodeEx("DamageUI", ImGuiTreeNodeFlags_Framed))
        {
            ImGui::ColorEdit4("DamageColor", &damageUIColor_.x);
            ImGui::ColorEdit4("WeakPointColor", &weakPointDamageUIColor_.x);

            ImGui::TreePop();
        }

        ImGui::DragFloat("Volume", &vibrationVolume_, 0.01f);
        ImGui::DragFloat("Time", &vibrationTime_, 0.01f);

        ImGui::TreePop();
    }
}

#pragma region ==================== Player VS 〇〇 ====================
// ----- Player VS 〇〇 -----
void CollisionManager::UpdatePlayerVs()
{
    // ジャスト回避判定
    UpdatePlayerJustDodge();

    // 攻撃判定
    UpdatePlayerAttack();

    // カウンター判定
    UpdatePlayerCounter();

    // くらい判定
    UpdatePlayerDamage();

    // 押し出し判定
    UpdatePlayerCollision();
}

#pragma region ---------- ジャスト回避判定 ----------
// ----- ジャスト回避判定 -----
void CollisionManager::UpdatePlayerJustDodge()
{
    // Player VS Enemy
    PlayerJustDodgeVsEnemyAttack();

    // Player VS Projectile
    PlayerJustDodgeVsProjectileAttack();
}

// ----- Player VS Enemy -----
void CollisionManager::PlayerJustDodgeVsEnemyAttack()
{
    // Enemyが存在しない
    if (EnemyManager::Instance().GetEnemyCount() <= 0) return;
    // ジャスト回避判定をしない
    if (PlayerManager::Instance().GetPlayer()->GetIsJustDodgeCheckEnabled() == false) return;

    Player* player = PlayerManager::Instance().GetPlayer().get();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    const DirectX::XMFLOAT3 playerPosition = player->GetTransform()->GetPosition();
    const int maxEnemyData = enemy->GetJustDodgeDetectionDataCount();

    for (int enemyDataIndex = 0; enemyDataIndex < maxEnemyData; ++enemyDataIndex)
    {
        const JustDodgeDetectionData enemyData = enemy->GetJustDodgeDetectionData(enemyDataIndex);

        // 現在有効ではない
        if (enemyData.GetIsActive() == false) continue;

        // 当たったかチェック
        if (IntersectSphereVsSphere(
            playerPosition, player->GetJustDodgeRadius(),
            enemyData.GetPosition(), enemyData.GetRadius()))
        {
            // ジャスト回避成功
            player->SetIsJustDodgeSuccessful(true);

            return;
        }
    }
}

// ----- Player VS Projectile -----
void CollisionManager::PlayerJustDodgeVsProjectileAttack()
{
}

#pragma endregion ---------- ジャスト回避判定 ----------

#pragma region ---------- 攻撃判定 ----------
// ----- 攻撃判定 -----
void CollisionManager::UpdatePlayerAttack()
{
    // Player VS Enemy
    PlayerAttackVsEnemyDamage();
}

// ----- Player VS Enemy -----
void CollisionManager::PlayerAttackVsEnemyDamage()
{
    // Enemyが存在しない
    if (EnemyManager::Instance().GetEnemyCount() <= 0) return;
    // 既に攻撃が当たっている
    if (PlayerManager::Instance().GetPlayer()->GetIsAttackHit()) return;
    // 攻撃可能フレーム外
    if (PlayerManager::Instance().GetPlayer()->GetIsAttackValid() == false) return;

    Player* player = PlayerManager::Instance().GetPlayer().get();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    const int maxPlayerData = player->GetAttackDetectionDataCount();
    const int maxEnemyData = enemy->GetDamageDetectionDataCount();

    for (int playerDataIndex = 0; playerDataIndex < maxPlayerData; ++playerDataIndex)
    {
        const AttackDetectionData playerData = player->GetAttackDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < maxEnemyData; ++enemyDataIndex)
        {
            DamageDetectionData enemyData = enemy->GetDamageDetectionData(enemyDataIndex);

            // このデータは、既にダメージをくらっている
            if (enemyData.GetIsHit()) continue;

            // 当たったかチェック
            if (IntersectSphereVsSphere(
                playerData.GetPosition(), playerData.GetRadius(),
                enemyData.GetPosition(), enemyData.GetRadius()))
            {
                const Player::STATE playerState = player->GetCurrentState();

                // ============================================================
                //  Hitフラグを立てる ( このデータの無敵時間設定 )
                // ============================================================
                enemyData.SetIsHit(true);
                enemyData.SetHitTimer(0.01f);

                // ============================================================
                //  当たった部位が弱点部位かの判定をする
                // ============================================================
                bool isWeakPoint = false;
                const EnemyDragon::DamageData damageDataIndex = static_cast<EnemyDragon::DamageData>(enemyDataIndex);
                if (damageDataIndex == EnemyDragon::DamageData::Head ||
                    (damageDataIndex >= EnemyDragon::DamageData::Tail && damageDataIndex <= EnemyDragon::DamageData::TailEnd))
                {
                    isWeakPoint = true;
                }

                // ============================================================
                // ヒットエフェクトを再生 ( 弱点部位は違うエフェクトを再生する )
                // ============================================================
                std::string hitEffectName = hitEffectType_ ? "Hit0" : "Hit1"; // エフェクトに幅を持たせる
                hitEffectType_ = (hitEffectType_ == 0) ? 1 : 0;

                // 弱点部位ならエフェクト変更
                if (isWeakPoint) hitEffectName = "Attack1";

                // 生成位置を求める
                const DirectX::XMFLOAT3 generatePosition = playerData.GetPosition() + XMFloat3Normalize(enemyData.GetPosition() - playerData.GetPosition()) * playerData.GetRadius();
                // 再生速度を決定
                const float effectSpeed = (playerState == Player::STATE::RushAttack) ? hitEffectSlowSpeed_ : hitEffectDefaultSpeed_;
                // エフェクトサイズを決定
                const float hitEffectSize = isWeakPoint ? hitEffectWeakPointSize_ : hitEffectDefaultSize_;

                Effect* hitEffect = EffectManager::Instance().GetEffect(hitEffectName.c_str());

                // 生成したエフェクトの情報を登録する
                effectHandle_[handleCounter_].name_ = hitEffectName;
                effectHandle_[handleCounter_].effectHandle_ = hitEffect->Play(generatePosition, hitEffectSize, effectSpeed);

                // エフェクトハンドル情報を更新
                ++handleCounter_;
                if (handleCounter_ >= maxEffectHandle_) handleCounter_ = 0;

                // GPU Particle Effectを再生する
                hitEffectName = isWeakPoint ? "HitEffect1" : "HitEffect";
                computeParticleEmitter_.SetEmitPosition(generatePosition);
                computeParticleEmitter_.EmitParticle(hitEffectName);

                // ============================================================
                // 効果音を鳴らす
                // ============================================================
                if (playerState != Player::STATE::RushAttack)
                {
                    AudioManager::Instance().PlaySE(SE::Attack0);
                }
                else
                {
                    AudioManager::Instance().PlaySE(SE::Attack0);
                }

                // ============================================================
                // 敵が死んでいなかったらダメージ処理をする
                // ============================================================
                if (enemy->GetIsDead() == false)
                {
                    const float attackPower = player->GetAttackPower();
                    const float damage = attackPower * enemyData.GetDamage();

                    enemy->AddDamage(damage, enemyDataIndex);

                    const DirectX::XMFLOAT4 color = isWeakPoint ? weakPointDamageUIColor_ : damageUIColor_;

                    // ダメージUIを生成する
                    UINumber* ui = new UINumber(damage, generatePosition, color);
                }

                // ============================================================
                // Playerの攻撃判定を無くす
                // ============================================================
                player->SetIsAttackHit(true);

                // ============================================================
                // ヒットストップ ( 弱点部位はヒットストップを長くする )
                // ============================================================
                if (playerState != Player::STATE::RushAttack)
                {
                    PlayerManager::Instance().SetHitStop(isWeakPoint ? PlayerManager::HitStopType::Critical : PlayerManager::HitStopType::Normal);
                }

                // ============================================================
                // カウンター攻撃時ならコントローラーを振動させる
                // ============================================================
                if (player->GetCurrentState() == Player::STATE::CounterCombo)
                {
                    Input::Instance().GetGamePad().Vibration(0.3f, 1.0f);
                }
                else
                {
                    // プレイヤーが現在ラッシュ攻撃中ならコントローラー振動を強くする
                    if (player->GetCurrentState() == Player::STATE::RushAttack)
                    {
                        const Player::Animation playerAnimationIndex = static_cast<Player::Animation>(player->GetAnimationIndex());
                        if (playerAnimationIndex == Player::Animation::AttackRush0 ||
                            playerAnimationIndex == Player::Animation::AttackRush2)
                        {
                            Input::Instance().GetGamePad().Vibration(0.2f, 0.7f, 1.0f);
                        }
                        else
                        {
                            Input::Instance().GetGamePad().Vibration(0.2f, 1.0f, 0.7f);
                        }
                    }
                    else
                    {
                        if (isWeakPoint) Input::Instance().GetGamePad().Vibration(0.2f, 0.4f);
                        else Input::Instance().GetGamePad().Vibration(0.1f, 0.3f);
                    }
                }

                // ============================================================
                // プレイヤーのルートの移動値を無くす
                // ============================================================
                if (playerState != Player::STATE::Helmbreaker && playerState != Player::STATE::CounterCombo)
                {
                    PlayerManager::Instance().GetPlayer()->SetRootMotionValue(0.0f);
                }

                // ============================================================
                // カメラシェイクを入れる
                // ============================================================
                if (isWeakPoint)
                {
                    Camera::Instance().ScreenVibrate(0.02f, 0.3f);
                }

                // ============================================================
                // 大回転斬りの場合、兜割り可能にする
                // ============================================================
                if (player->GetCurrentState() == Player::STATE::CounterCombo)
                {
                    PlayerManager::Instance().GetPlayer()->SetSwordColor({ 1,0,0 });
                    PlayerManager::Instance().GetPlayer()->SetSwordSpirit(1.0f);
                }

                return;
            }
        }
    }
}

#pragma endregion ---------- 攻撃判定 ----------

#pragma region ---------- カウンター判定 ----------
// ----- カウンター判定 -----
void CollisionManager::UpdatePlayerCounter()
{
    // Player VS Enemy
    PlayerCounterVsEnemy();
    PlayerGuardCounterVsEnemy();

    // Player VS Projectile
    PlayerCounterVsProjectile();
    PlayerGuardCounterVsProjectile();
}

// ----- Player VS Enemy -----
void CollisionManager::PlayerCounterVsEnemy()
{
    // Enemyが存在しない
    if (EnemyManager::Instance().GetEnemyCount() <= 0) return;

    Player* player = PlayerManager::Instance().GetPlayer().get();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    // Playerのステートがカウンターではない
    if (player->GetCurrentState() != Player::STATE::Counter) return;
    // Playerがカウンターを受け付けていない
    if (player->GetIsCounter() == false) return;
    // 既にカウンターが成功している
    if (player->GetIsAbleCounterAttack()) return;
    // Enemyの攻撃判定が無効状態
    if (enemy->GetIsAttackActive() == false) return;
    // 咆哮はカウンターできない
    if (enemy->GetCurrentAttackAction() == Enemy::AttackAction::Roar) return;

    const float distance = enemy->CalcDistanceToPlayer();
    const float counterActiveRadius = player->GetCounterActiveRadius();

    // カウンターが成功した
    if (distance < counterActiveRadius)
    {
        player->SetIsAbleCounterAttack(true);
    }
}

// ----- Player VS Enemy -----
void CollisionManager::PlayerGuardCounterVsEnemy()
{
    // Enemyが存在しない
    if (EnemyManager::Instance().GetEnemyCount() <= 0) return;
    // Playerがガードしていない
    if (PlayerManager::Instance().GetPlayer()->GetIsGuardCounterStance() == false) return;    

    Player* player = PlayerManager::Instance().GetPlayer().get();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    const DirectX::XMFLOAT3 pelvisPosition = player->GetJointPosition("pelvis");
    const int maxEnemyData = enemy->GetAttackDetectionDataCount();

    for (int enemyDataIndex = 0; enemyDataIndex < maxEnemyData; ++enemyDataIndex)
    {
        const AttackDetectionData enemyData = enemy->GetAttackDetectionData(enemyDataIndex);

        // このデータの攻撃判定が有効ではない
        if (enemyData.GetIsActive() == false) continue;

        const DirectX::XMFLOAT3 enemyDataPosition = enemyData.GetPosition();

        // 当たったかチェック
        if (IntersectSphereVsSphere(
            pelvisPosition, player->GetGuardCounterRadius(),
            enemyDataPosition, enemyData.GetRadius()))
        {
            // ガードカウンターが成功した
            player->SetIsGuardCounterSuccessful(true);

            // ============================================================
            //  ノックバックを入れる
            // ============================================================
            const DirectX::XMFLOAT3 playerPosition = player->GetTransform()->GetPosition();
            const DirectX::XMFLOAT3 enemyPosition = enemy->GetTransform()->GetPosition();
            // 敵のアニメーションによってノックバックの計算を変える
            DirectX::XMFLOAT3 knockBackDirection = {};
            Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(enemy->GetAnimationIndex());
            if (animationIndex == Enemy::DragonAnimation::AttackSlam0)
            {
                knockBackDirection = enemyPosition - playerPosition;
            }
            else
            {
                knockBackDirection = enemyDataPosition - playerPosition;
            }
            knockBackDirection.y = 0.0f;
            player->SetKnockBackDirection(XMFloat3Normalize(knockBackDirection));

            // ============================================================
            //  エフェクト再生
            // ============================================================
            DirectX::XMFLOAT3 effectPosition = pelvisPosition + XMFloat3Normalize(enemyDataPosition - pelvisPosition) * player->GetGuardCounterRadius();
            EffectManager::Instance().GetEffect("Counter")->Play(effectPosition, 0.1f, 4.0f);

            
            // ============================================================
            //  敵を数フレーム停止させる
            // ============================================================
            enemy->SetHitStop();

            return;
        }
    }
}

// ----- Player VS Projectile -----
void CollisionManager::PlayerCounterVsProjectile()
{
    Player* player = PlayerManager::Instance().GetPlayer().get();

    // Playerがカウンターステートではない
    const Player::STATE playerState = player->GetCurrentState();
    if (playerState != Player::STATE::Counter && playerState != Player::STATE::GuardCounter) return;
    // Playerがカウンターを受け付けていない
    if (player->GetIsCounter() == false && player->GetIsGuardCounterStance() == false) return;
    // 既にカウンター成功している
    if (player->GetIsAbleCounterAttack() && player->GetIsGuardCounterSuccessful()) return;

    std::vector<Projectile*> projectiles = ProjectileManager::Instance().GetProjectiles();
    const int maxProjectileData = projectiles.size();

    const DirectX::XMFLOAT3 playerPosition = player->GetTransform()->GetPosition();

    for (int projectileIndex = 0; projectileIndex < maxProjectileData; ++projectileIndex)
    {
        Projectile* projectile = projectiles.at(projectileIndex);
        const DirectX::XMFLOAT3 projectilePosition = projectile->GetTransform()->GetPosition();

        // 範囲内にいるか判定
        if (IntersectSphereVsSphere(
            playerPosition, 0.0f,
            projectilePosition, projectile->GetCounterRadius()))
        {
            // ----- 見切カウンター -----
            if (playerState == Player::STATE::Counter)
            {
                // カウンター成功
                player->SetIsAbleCounterAttack(true);
            }
            // ----- ガードカウンター -----
            else
            {
                // ガードカウンターが成功した
                player->SetIsGuardCounterSuccessful(true);

                // ノックバックの方向算出
                DirectX::XMFLOAT3 playerPosition = player->GetTransform()->GetPosition();
                DirectX::XMFLOAT3 knockBackDirection = projectile->GetTransform()->GetPosition() - playerPosition;
                knockBackDirection.y = 0.0f;
                knockBackDirection = XMFloat3Normalize(knockBackDirection);
                player->SetKnockBackDirection(knockBackDirection);

                // エフェクト再生
                const DirectX::XMFLOAT3 pelvisPosition = player->GetJointPosition("pelvis");
                DirectX::XMFLOAT3 vec = projectile->GetTransform()->GetPosition() - pelvisPosition;
                vec = pelvisPosition + XMFloat3Normalize(vec) * player->GetGuardCounterRadius();

                EffectManager::Instance().GetEffect("Counter")->Play(vec, 0.1f, 4.0f);
            }

            return;
        }
    }
}

// ----- Player VS Projectile -----
void CollisionManager::PlayerGuardCounterVsProjectile()
{
}

#pragma endregion ---------- カウンター判定 ----------

#pragma region ---------- くらい判定 ----------
// ----- くらい判定 -----
void CollisionManager::UpdatePlayerDamage()
{
    // Player VS Enemy
    PlayerFlinchVsEnemyAttack(); // 怯み判定
    PlayerDamageVsEnemyAttack(); // くらい判定

    // Player VS Projectile
    PlayerDamageVsProjectileAttack();
}

// ----- Player VS Enemy -----
void CollisionManager::PlayerFlinchVsEnemyAttack()
{
    // Enemyが存在しない
    if (EnemyManager::Instance().GetEnemyCount() <= 0) return;
    // Enemyの攻撃判定が無効
    if (EnemyManager::Instance().GetEnemy(0)->GetIsAttackActive() == false) return;
    // Enemyが咆哮していない
    if (EnemyManager::Instance().GetEnemy(0)->GetCurrentAttackAction() != Enemy::AttackAction::Roar) return;
    // 既にPlayerが怯んでいる
    if (PlayerManager::Instance().GetPlayer()->GetCurrentState() == Player::STATE::Flinch) return;

    Player* player = PlayerManager::Instance().GetPlayer().get();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    const int maxPlayerData = player->GetDamageDetectionDataCount();
    const int maxEnemyData = enemy->GetAttackDetectionDataCount();

    for (int playerDataIndex = 0; playerDataIndex < maxPlayerData; ++playerDataIndex)
    {
        const DamageDetectionData playerData = player->GetDamageDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < maxEnemyData; ++enemyDataIndex)
        {
            const AttackDetectionData enemyData = enemy->GetAttackDetectionData(enemyDataIndex);
            // このデータは攻撃判定が有効ではない
            if (enemyData.GetIsActive() == false) continue;

            // 当たったか判定
            if (IntersectSphereVsSphere(
                playerData.GetPosition(), playerData.GetRadius(),
                enemyData.GetPosition(), enemyData.GetRadius()))
            {
                // 怯みステートに遷移
                player->ChangeState(Player::STATE::Flinch);

                // 当たったので終了
                return;
            }
        }
    }
}

// ----- Player VS Enemy -----
void CollisionManager::PlayerDamageVsEnemyAttack()
{
    // Enemyが存在しない
    if (EnemyManager::Instance().GetEnemyCount() <= 0) return;
    // Enemyの攻撃判定が無効
    if (EnemyManager::Instance().GetEnemy(0)->GetIsAttackActive() == false) return;
    // Playerが無敵状態
    if (PlayerManager::Instance().GetPlayer()->GetIsInvincible()) return;
    // カウンターが成功している
    if (PlayerManager::Instance().GetPlayer()->GetIsAbleCounterAttack()) return;

    Player* player = PlayerManager::Instance().GetPlayer().get();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    // 咆哮の場合ダメージは受けない
    if (enemy->GetCurrentAttackAction() == Enemy::AttackAction::Roar) return;

    // SuperNovaのチャージに対するくらい判定
    if (enemy->GetCurrentAttackAction() == Enemy::AttackAction::SuperNova)
    {
        const DirectX::XMFLOAT2 playerPosition = { player->GetTransform()->GetPositionX(), player->GetTransform()->GetPositionZ() };
        const DirectX::XMFLOAT2 dragonPosition = { enemy->GetTransform()->GetPositionX(), enemy->GetTransform()->GetPositionZ() };
        const float length = XMFloat2Length(playerPosition - dragonPosition);
        if (length < enemy->GetSuperNovaRadius())
        {
            player->AddDamage(enemy->GetSuperNovaDamage());
        }
    }

    const int maxPlayerData = player->GetDamageDetectionDataCount();
    const int maxEnemyData = enemy->GetAttackDetectionDataCount();

    for (int playerDataIndex = 0; playerDataIndex < maxPlayerData; ++playerDataIndex)
    {
        const DamageDetectionData playerData = player->GetDamageDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < maxEnemyData; ++enemyDataIndex)
        {
            const AttackDetectionData enemyData = enemy->GetAttackDetectionData(enemyDataIndex);

            // このデータの攻撃判定が有効ではない
            if (enemyData.GetIsActive() == false) continue;

            // 当たったかチェック
            if (IntersectSphereVsSphere(
                playerData.GetPosition(), playerData.GetRadius(),
                enemyData.GetPosition(), enemyData.GetRadius()))
            {
                // ==================================================
                // カウンター状態ならカウンター成功
                // ==================================================
                if (player->GetIsCounter())
                {
                    player->SetIsAbleCounterAttack(true);
                    return;
                }

                // ==================================================
                // ダメージを与える
                // ==================================================
                int damage = enemy->GetAttackPower();
                player->AddDamage(damage);

                // ==================================================
                // コントローラー振動 (ダメージ受けたリアクションとして)
                // ==================================================
                if (enemy->GetCurrentAttackAction() != Enemy::AttackAction::SuperNova)
                {
                    Input::Instance().GetGamePad().Vibration(0.2f, 1.0f);
                }

                // ==================================================
                // ダメージSE再生
                // ==================================================
                AudioManager::Instance().PlaySE(SE::Damage);

                // HPがまだあるのでDamageStateに遷移
                if (player->GetHealth() > 0.0f)
                {
                    player->ChangeDamageState();
                }
                // HPがもうないのでDeathStateに遷移
                else
                {
                    player->ChangeDeathState();
                }

                return;
            }
        }
    }
}

// ----- Player VS Projectile -----
void CollisionManager::PlayerDamageVsProjectileAttack()
{
    // Projectileが存在していない
    if (ProjectileManager::Instance().GetProjectileCount() <= 0) return;
    // Playerが無敵状態
    if (PlayerManager::Instance().GetPlayer()->GetIsInvincible()) return;
    // カウンターが成功している
    if (PlayerManager::Instance().GetPlayer()->GetIsAbleCounterAttack()) return;

    Player* player = PlayerManager::Instance().GetPlayer().get();
    std::vector<Projectile*> projectiles = ProjectileManager::Instance().GetProjectiles();

    const int maxPlayerData = player->GetDamageDetectionDataCount();
    const int maxProjectileData = projectiles.size();

    for (int playerDataIndex = 0; playerDataIndex < maxPlayerData; ++playerDataIndex)
    {
        const DamageDetectionData playerData = player->GetDamageDetectionData(playerDataIndex);
        
        for (int projectileIndex = 0; projectileIndex < maxProjectileData; ++projectileIndex)
        {
            Projectile* projectile = projectiles.at(projectileIndex);

            // Playerとの当たり判定を行わない
            if (projectile->GetAttackType() == static_cast<int>(ProjectileManager::AttackType::Enemy) ||
                projectile->GetAttackType() == static_cast<int>(ProjectileManager::AttackType::None))
            {
                continue;
            }

            // 当たったかチェック
            if (IntersectSphereVsSphere(
                playerData.GetPosition(), playerData.GetRadius(),
                projectile->GetTransform()->GetPosition(), projectile->GetRadius()))
            {
                projectile->OnHit({});

                // カウンター状態ならカウンター成功
                if (player->GetIsCounter())
                {
                    player->SetIsAbleCounterAttack(true);
                    return;
                }

                // ダメージを与える
                player->AddDamage(projectile->GetDamage());

                // HPがまだあるのでDamageStateに遷移
                if (player->GetHealth() > 0.0f)
                {
                    player->ChangeDamageState();
                }
                // HPがもうないのでDeathStateに遷移
                else
                {
                    player->ChangeDeathState();
                }
                return;
            }
        }
    }
}

#pragma endregion ---------- くらい判定 ----------

#pragma region ---------- 押し出し判定 ----------
// ----- 押し出し判定 -----
void CollisionManager::UpdatePlayerCollision()
{
    // Player VS Enemy
    PlayerVsEnemy();
}

// ----- Player VS Enemy -----
void CollisionManager::PlayerVsEnemy()
{
    // Enemyが存在しない
    if (EnemyManager::Instance().GetEnemyCount() <= 0) return;
    // 押し出し判定が無効
    if (PlayerManager::Instance().GetUseCollisionDetection() == false) return;
    // プレイヤーのステートが兜割り
    if (PlayerManager::Instance().GetPlayer()->GetCurrentState() == Player::STATE::Helmbreaker) return;

    Player* player = PlayerManager::Instance().GetPlayer().get();
    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);

    const int maxPlayerData = player->GetCollisionDetectionDataCount();
    const int maxEnemyData = enemy->GetCollisionDetectionDataCount();

    for (int playerDataIndex = 0; playerDataIndex < maxPlayerData; ++playerDataIndex)
    {
        const CollisionDetectionData playerData = player->GetCollisionDetectionData(playerDataIndex);

        for (int enemyDataIndex = 0; enemyDataIndex < maxEnemyData; ++enemyDataIndex)
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

#pragma endregion ---------- 押し出し判定 ----------

#pragma endregion ==================== Player VS 〇〇 ====================

#pragma region ==================== Enemy VS 〇〇 ====================
// ----- Enemy VS 〇〇 -----
void CollisionManager::UpdateEnemyVs()
{
    // くらい判定
    UpdateEnemyDamage();
}

#pragma region ---------- くらい判定 ----------
// ----- くらい判定 -----
void CollisionManager::UpdateEnemyDamage()
{
    // Enemy VS Projectile
    EnemyDamageVsProjectileAttack();
}

// ----- Enemy VS Projectile -----
void CollisionManager::EnemyDamageVsProjectileAttack()
{
    // Enemyが存在しない
    if (EnemyManager::Instance().GetEnemyCount() <= 0) return;
    // Projectileが存在しない
    if (ProjectileManager::Instance().GetProjectileCount() <= 0) return;

    Enemy* enemy = EnemyManager::Instance().GetEnemy(0);
    std::vector<Projectile*> projectiles = ProjectileManager::Instance().GetProjectiles();

    const int maxEnemyData = enemy->GetDamageDetectionDataCount();
    const int maxProjectileData = projectiles.size();

    for (int enemyDataIndex = 0; enemyDataIndex < maxEnemyData; ++enemyDataIndex)
    {
        DamageDetectionData enemyData = enemy->GetDamageDetectionData(enemyDataIndex);

        // このデータは、既にダメージをくらっている
        if (enemyData.GetIsHit()) continue;

        for (int projectileIndex = 0; projectileIndex < maxProjectileData; ++projectileIndex)
        {
            Projectile* projectile = projectiles.at(projectileIndex);

            // Enemyとの当たり判定を行わない
            if (projectile->GetAttackType() == static_cast<int>(ProjectileManager::AttackType::Player) ||
                projectile->GetAttackType() == static_cast<int>(ProjectileManager::AttackType::None))
            {
                continue;
            }

            // 当たったかチェック
            if (IntersectSphereVsSphere(
                projectile->GetTransform()->GetPosition(), projectile->GetRadius(),
                enemyData.GetPosition(), enemyData.GetRadius()))
            {
                // 当たった位置を求める
                const DirectX::XMFLOAT3 hitPosition = projectile->GetTransform()->GetPosition() + XMFloat3Normalize(enemyData.GetPosition() - projectile->GetTransform()->GetPosition()) * projectile->GetRadius();

                // ============================================================
                //  Hitフラグ & 関数呼び出し
                // ============================================================
                enemyData.SetIsHit(true);
                enemyData.SetHitTimer(0.01f);
                projectile->OnHit(hitPosition);

                // ============================================================
                //  当たった部位が弱点部位かの判定をする
                // ============================================================
                bool isWeakPoint = false;
                const EnemyDragon::DamageData damageDataIndex = static_cast<EnemyDragon::DamageData>(enemyDataIndex);
                if (damageDataIndex == EnemyDragon::DamageData::Head ||
                    (damageDataIndex >= EnemyDragon::DamageData::Tail && damageDataIndex <= EnemyDragon::DamageData::TailEnd))
                {
                    isWeakPoint = true;
                }

                // ============================================================
                // 敵が死んでいなかったらダメージ処理をする
                // ============================================================
                if (enemy->GetIsDead() == false)
                {
                    const float attackPower = projectile->GetDamage();
                    const float damage = attackPower * enemyData.GetDamage();

                    enemy->AddDamage(damage, enemyDataIndex);

                    const DirectX::XMFLOAT4 color = isWeakPoint ? weakPointDamageUIColor_ : damageUIColor_;

                    // ダメージUIを生成する
                    UINumber* ui = new UINumber(damage, hitPosition, color);
                }

                return;
            }
        }
    }
}

#pragma endregion ---------- くらい判定 ----------

#pragma endregion ==================== Enemy VS 〇〇 ====================


#pragma region ==================== Projectile VS 〇〇 ====================
void CollisionManager::UpdateProjectileVs()
{
}

#pragma endregion ==================== Projectile VS 〇〇 ====================


#pragma region ---------- Item VS 〇〇 ----------
void CollisionManager::UpdateItemVs()
{
    // アイテムと攻撃判定との判定
    UpdateItemVsAttack();

    // アイテムとダメージ判定
    UpdateItemVsDamage();

    // アイテム同士の判定
    UpdateItemVsItem();
}

// ----- アイテムと攻撃判定との判定 -----
void CollisionManager::UpdateItemVsAttack()
{
    // プレイヤーの攻撃との判定をする
    // 攻撃可能フレームなら処理する
    if (PlayerManager::Instance().GetPlayer()->GetIsAttackValid())
    {
        for (int itemIndex = 0; itemIndex < ItemManager::Instance().GetItemCount(); ++itemIndex)
        {
            Item* item = ItemManager::Instance().GetItems().at(itemIndex);
            if (item->GetIsDrawModel() == false) continue;
            
            bool isHit = false;
            Player* player = PlayerManager::Instance().GetPlayer().get();            
            for (int playerDataIndex = 0; playerDataIndex < player->GetAttackDetectionDataCount(); ++playerDataIndex)
            {
                // 既に当たっていたら飛ばす
                if (isHit) continue;

                const AttackDetectionData playerData = player->GetAttackDetectionData(playerDataIndex);

                const DirectX::XMFLOAT3 itemPosition = item->GetTransform()->GetPosition() + item->GetOffsetPosition();

                // 当たったかチェック
                if (IntersectSphereVsSphere(
                    playerData.GetPosition(), playerData.GetRadius(),
                    itemPosition, item->GetDamageRadius()))
                {
                    item->OnHit();
                    isHit = true;
                }
            }
        }
    }

    // ドラゴンの攻撃との判定をする
    // 攻撃判定が有効 & まだ当たってないなら 処理する
    if (EnemyManager::Instance().GetEnemy(0)->GetIsAttackActive())
    {
        for (int itemIndex = 0; itemIndex < ItemManager::Instance().GetItemCount(); ++itemIndex)
        {
            Item* item = ItemManager::Instance().GetItems().at(itemIndex);
            if (item->GetIsDrawModel() == false) continue;
            
            bool isHit = false;
            Enemy* enemy = EnemyManager::Instance().GetEnemy(0);
            // 咆哮では樽は壊れない
            if (enemy->GetCurrentAttackAction() == Enemy::AttackAction::Roar) continue;

            for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetAttackDetectionDataCount(); ++enemyDataIndex)
            {
                const AttackDetectionData enemyData = enemy->GetAttackDetectionData(enemyDataIndex);
                if (enemyData.GetIsActive() == false) continue;

                const DirectX::XMFLOAT3 itemPosition = item->GetTransform()->GetPosition() + item->GetOffsetPosition();

                // 当たったかチェック
                if (IntersectSphereVsSphere(
                    enemyData.GetPosition(), enemyData.GetRadius(),
                    itemPosition, item->GetDamageRadius()))
                {
                    item->OnHit();
                    isHit = true;
                }
            }
        }
    }
}

// ----- アイテムとダメージ判定 -----
void CollisionManager::UpdateItemVsDamage()
{
    // プレイヤーのくらい判定との判定をする
    for (int itemIndex = 0; itemIndex < ItemManager::Instance().GetItemCount(); ++itemIndex)
    {
        Item* item = ItemManager::Instance().GetItems().at(itemIndex);
        // 攻撃判定有効ではない
        if (item->GetIsAttackActive() == false) continue;
        
        Player* player = PlayerManager::Instance().GetPlayer().get();
        
        // ダメージ食らっているので処理しない
        if (player->GetCurrentState() == Player::STATE::Damage ||
            player->GetCurrentState() == Player::STATE::Death)
        {
            continue;
        }

        bool isHit = false;
        for (int playerDataIndex = 0; playerDataIndex < player->GetDamageDetectionDataCount(); ++playerDataIndex)
        {
            // 既に当たっていたらスキップする
            if (isHit) continue;

            const DamageDetectionData playerData = player->GetDamageDetectionData(playerDataIndex);

            const DirectX::XMFLOAT3 itemPosition = item->GetTransform()->GetPosition() + item->GetOffsetPosition();
            
            // 当たったか判定
            if (IntersectSphereVsSphere(
                playerData.GetPosition(), playerData.GetRadius(),
                itemPosition, item->GetAttackRadius()))
            {
                isHit = true;

                // カウンター状態ならカウンター成功
                if (player->GetIsCounter())
                {
                    player->SetIsAbleCounterAttack(true);
                    return;
                }

                // ダメージを与える
                int damage = item->GetAttackPower() * 0.2f;
                player->AddDamage(damage);

                // コントローラー振動 (ダメージ受けたリアクションとして)
                if (EnemyManager::Instance().GetEnemy(0)->GetCurrentAttackAction() != Enemy::AttackAction::SuperNova)
                {
                    Input::Instance().GetGamePad().Vibration(0.2f, 1.0f);
                }

                // ダメージSE再生
                AudioManager::Instance().PlaySE(SE::Damage);

                // HPがまだあるのでDamageStateに遷移
                if (player->GetHealth() > 0.0f)
                {
                    player->ChangeDamageState();
                }
                // HPがもうないのでDeathStateに遷移
                else
                {
                    player->ChangeDeathState();
                }
            }
        }
    }

    // ドラゴンのくらい判定との判定をする
    for (int itemIndex = 0; itemIndex < ItemManager::Instance().GetItemCount(); ++itemIndex)
    {
        Item* item = ItemManager::Instance().GetItems().at(itemIndex);
        // 攻撃判定有効ではない
        if (item->GetIsAttackActive() == false) continue;

        Enemy* enemy = EnemyManager::Instance().GetEnemy(0);
        bool isHit = false;
        for (int enemyDataIndex = 0; enemyDataIndex < enemy->GetDamageDetectionDataCount(); ++enemyDataIndex)
        {
            // 既に当たっていたらスキップする
            if (isHit) continue;

            DamageDetectionData enemyData = enemy->GetDamageDetectionData(enemyDataIndex);

            // もう既にダメージを食らっているデータ
            if (enemyData.GetIsHit()) continue;

            const DirectX::XMFLOAT3 itemPosition = item->GetTransform()->GetPosition() + item->GetOffsetPosition();

            // 当たったかチェック
            if (IntersectSphereVsSphere(
                enemyData.GetPosition(), enemyData.GetRadius(),
                itemPosition, item->GetAttackRadius()))
            {
                // Hitフラグを立てる, このデータの無敵時間設定
                isHit = true;
                enemyData.SetIsHit(true);
                enemyData.SetHitTimer(0.01f);

                // 弱点部位か判断する
                bool isWeakPoint = false;
                const EnemyDragon::DamageData damageDataIndex = static_cast<EnemyDragon::DamageData>(enemyDataIndex);
                if (damageDataIndex == EnemyDragon::DamageData::Head ||
                    (damageDataIndex >= EnemyDragon::DamageData::Tail && damageDataIndex <= EnemyDragon::DamageData::TailEnd))
                {
                    isWeakPoint = true;
                }

                // TODO:ヒットエフェクトを再生 ( 弱点部位は違うエフェクトを再生する )

                // TODO:効果音を鳴らす


                // 敵が死んでいなかったらダメージ処理をする
                if (enemy->GetIsDead() == false)
                {
                    const float attackPower = item->GetAttackPower();
                    const float damage = attackPower * enemyData.GetDamage();

                    enemy->AddDamage(damage, enemyDataIndex);

                    const DirectX::XMFLOAT4 defaultColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
                    const DirectX::XMFLOAT4 weakPointColor = DirectX::XMFLOAT4(1.0f, 0.55f, 0.0f, 1.0f);
                    DirectX::XMFLOAT4 color = isWeakPoint ? weakPointColor : defaultColor;

                    UINumber* ui = new UINumber(damage, enemyData.GetPosition(), color);
                }
            }
        }
    }
}

// ----- アイテム同士の判定 -----
void CollisionManager::UpdateItemVsItem()
{
    for (int attackItemIndex = 0; attackItemIndex < ItemManager::Instance().GetItemCount(); ++attackItemIndex)
    {
        Item* attackItem = ItemManager::Instance().GetItems().at(attackItemIndex);
        // 攻撃判定有効ではない
        if (attackItem->GetIsAttackActive() == false) continue;

        for (int damageItemIndex = 0; damageItemIndex < ItemManager::Instance().GetItemCount(); ++damageItemIndex)
        {
            // 同じアイテムなので処理しない
            if (damageItemIndex == attackItemIndex) continue;

            Item* damageItem = ItemManager::Instance().GetItems().at(damageItemIndex);

            if (damageItem->GetIsDrawModel() == false) continue;

            const DirectX::XMFLOAT3 attackItemPosition = attackItem->GetTransform()->GetPosition() + attackItem->GetOffsetPosition();
            const DirectX::XMFLOAT3 damageItemPosition = damageItem->GetTransform()->GetPosition() + damageItem->GetOffsetPosition();

            // 当たったかチェック
            if (IntersectSphereVsSphere(
                attackItemPosition, attackItem->GetAttackRadius(),
                damageItemPosition, damageItem->GetDamageRadius()))
            {
                damageItem->OnHit();
            }
        }
    }

}

#pragma endregion ---------- Item VS 〇〇 ----------

#pragma region ---------- Intersect ----------
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

#pragma endregion ---------- Intersect ----------