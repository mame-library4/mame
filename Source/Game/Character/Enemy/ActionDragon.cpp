#include "ActionDragon.h"
#include "PostProcess/PostProcess.h"
#include "Easing.h"
#include "Input.h"
#include "Camera.h"
#include "Common.h"
#include "../Player/PlayerManager.h"

#include "Particle/ParticleManager.h"

#include "Projectile/ProjectileManager.h"
#include "Projectile/Fireball.h"
#include "Projectile/Rock.h"

#include "sprite.h"
#include "Application.h"
#include "System/SystemManager.h"

#include "UI/UIFlashOut.h"

#include "AudioManager.h"

#include "SceneManager.h"
#include "LoadingScene.h"
#include "TitleScene.h"

// ----- GamePadVibration -----
namespace ActionDragon
{
    // ----- 初期化 -----
    void GamePadVibration::Initialize(const float& startFrame, const float& time, const float& power)
    {
        startFrame_ = startFrame;
        time_ = time;
        power_ = power;
        isVibraion_ = false;
    }

    // ----- 更新 -----
    void GamePadVibration::Update(const float& animationFrame)
    {
        // 既に振動させている
        if (isVibraion_) return;

        // 現在のアニメーションのフレームがスタートフレームまで達していない
        if (animationFrame < startFrame_) return;
        
        // コントローラーを振動させる
        Input::Instance().GetGamePad().Vibration(time_, power_);
        isVibraion_ = true;
    }
}

// ----- AddForceData -----
namespace ActionDragon
{
    // ----- 初期化 -----
    void AddForceData::Initialize(const float& addForceFrame, const float& force, const float& decelerationForce)
    {
        addForceFrame_ = addForceFrame;
        force_ = force * 60.0f;
        decelerationForce_ = decelerationForce * 60.0f;
        isAddforce_ = false;
    }

    // ----- 更新 -----
    bool AddForceData::Update(const float& animationFrame)
    {
        // 既にAddForceしている
        if (isAddforce_) return false;
        
        // アニメーションのフレームが指定のフレームに達していない
        if (animationFrame < addForceFrame_) return false;

        isAddforce_ = true;
        return true;
    }
}

#pragma region ---------- 攻撃以外の重要行動 ----------
// ----- DeathAction -----
namespace ActionDragon
{
    const ActionBase::State DeathAction::Run(const float& elapsedTime)
    {
        if (owner_->GetStep() == 0)
        {
            // 攻撃判定を無効化する
            owner_->ResetAllAttackActiveFlag();
            // ジャスト回避判定を無効化する
            owner_->ResetAllJustDodgeActiveFlag();
            
            // ルートモーションを使用しない
            owner_->SetUseRootMotion(false);

            SystemManager::Instance().SetAllSlowSpeed(1.0f);
            SystemManager::Instance().SetPlayerSlowSpeed(1.0f);

            PlayerManager::Instance().GetPlayer()->ChangeState(Player::STATE::Idle);

            // 移動させない
            owner_->AddForce({}, 0.0f, 0.0f);
            // 全オーディオ停止
            AudioManager::Instance().StopAllAudio();

            owner_->SetStep(1);
        }
        if (owner_->GetStep() == 1)
        {
            timer_ -= elapsedTime;
            if (timer_ <= 0.0f)
            {
                AudioManager::Instance().StopAllAudio();
                SceneManager::Instance().ChangeScene(new LoadingScene(new TitleScene));
            }
        }

        return ActionBase::State::Run;
    }

    void DeathAction::DrawDebug()
    {
    }
}

// ----- DownAction -----
namespace ActionDragon
{
    const ActionBase::State DownAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange())
        {
            Finalize();

            return ActionBase::State::Failed;
        }

        switch (owner_->GetStep())
        {
        case 0:// 初期化
            // アニメーション再生
            if (PlayAnimation())
            {
                // 怯み時押し出し判定設定
                owner_->SetDownCollisionActiveFlag();

                // 攻撃判定を無効化する
                owner_->ResetAllAttackActiveFlag();
                // ジャスト回避判定を無効化する
                owner_->ResetAllJustDodgeActiveFlag();

                loopCounter_ = 0;

                owner_->SetStep(1);
            }

            break;
        case 1:

            if (owner_->GetAnimationSeconds() > 1.0f)
            {
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::CriticalLoop, false);
                owner_->SetTransitionTime(0.1f);

                owner_->SetStep(2);
            }

            break;
        case 2:

            if (owner_->IsPlayAnimation() == false)
            {
                if (loopCounter_ >= maxLoopNum_)
                {
                    owner_->PlayAnimation(Enemy::DragonAnimation::CriticalEnd, false);
                    // ステート変更
                    owner_->SetStep(3);
                    
                    return ActionBase::State::Run;
                }
                else
                {
                    owner_->PlayAnimation(Enemy::DragonAnimation::CriticalLoop, false);
                }

                ++loopCounter_;
            }

            break;
        case 3:
            if (owner_->IsPlayAnimation() == false)
            {
                Finalize();
                owner_->SetIsStagger(false);

                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }

    // ----- ImGui用 -----
    void DownAction::DrawDebug()
    {
        if (ImGui::TreeNodeEx("Down", ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- TransitionTime ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragInt("LoopCounter", &loopCounter_);
                ImGui::DragInt("MaxLoopNum", &maxLoopNum_);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- 終了化 -----
    void DownAction::Finalize()
    {
        owner_->SetDownCollisionActiveFlag(false);
    }

    // ----- アニメーション再生 -----
    const bool  DownAction::PlayAnimation()
    {
        const Player::STATE playerState = PlayerManager::Instance().GetPlayer()->GetCurrentState();
        // 現在プレイヤーがラッシュ攻撃中なので待機する
        if (playerState == Player::STATE::RushAttack) return false;

        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());
        float transitionTime = 0.1f;
        float blendAnimationFrame = 0.0f;

        owner_->PlayBlendAnimation(Enemy::DragonAnimation::CriticalStart, false, 1.0f, blendAnimationFrame);
        owner_->SetTransitionTime(transitionTime);

        return true;
    }
}

// ----- KnockDownAction -----
namespace ActionDragon
{
    const ActionBase::State KnockDownAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange())
        {

            return ActionBase::State::Failed;
        }

        switch (owner_->GetStep())
        {
        case 0:// 初期化
            // アニメーション再生
            if (PlayAnimation())
            {
                // 攻撃判定を無効化する
                owner_->ResetAllAttackActiveFlag();
                // ジャスト回避判定を無効化する
                owner_->ResetAllJustDodgeActiveFlag();

                // 次に進む
                owner_->SetStep(1);
            }

            break;
        case 1:

            UpdateAnimationSpeed();

            if (owner_->IsPlayAnimation() == false)
            {
                // フラグをリセット
                owner_->SetIsStagger(false);

                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }

    // ----- ImGui用 -----
    void KnockDownAction::DrawDebug()
    {
        if (ImGui::TreeNodeEx("KnockDown", ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- Slow ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("StartFrame", &slowStartFrame_, 0.01f, 0.0f, 5.0f);
                ImGui::DragFloat("EndFrame", &slowEndFrame_, 0.01f, 0.0f, 5.0f);
                ImGui::DragFloat("SlowSpeed", &slowAnimationSpeed_, 0.01f, 0.0f, 3.0f);
                ImGui::DragFloat("MostSlowSpeed", &mostSlowAnimationSpeed_, 0.01f, 0.0f, 3.0f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- TransitionTime ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("SlamAttack", &transitionSlamAttack_, 0.01f, 0.0f, 0.5f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    const bool KnockDownAction::PlayAnimation()
    {
        const Player::STATE playerState = PlayerManager::Instance().GetPlayer()->GetCurrentState();
        // 現在プレイヤーがラッシュ攻撃中なので待機する
        if (playerState == Player::STATE::RushAttack) return false;
       
        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());
        float translationTime = 0.1f;
        
        if (animationIndex == Enemy::DragonAnimation::AttackSlam0)
        {
            translationTime = transitionSlamAttack_;
        }

        owner_->PlayBlendAnimation(Enemy::DragonAnimation::Damage, false, 1.0f, 0.3f);
        owner_->SetTransitionTime(translationTime);

        return true;
    }

    // ----- アニメーション速度更新 -----
    void KnockDownAction::UpdateAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();
        float animationSpeed = 1.0f;

        if (animationSeconds > slowStartFrame_ && animationSeconds < slowEndFrame_)
        {
            animationSpeed = slowAnimationSpeed_;
        }
        else if (animationSeconds > slowEndFrame_)
        {
            animationSpeed = mostSlowAnimationSpeed_;
        }

        owner_->SetAnimationSpeed(animationSpeed);
    }
}

#pragma endregion ---------- 攻撃以外の重要行動 ----------

#pragma region ---------- 咆哮 ----------
namespace ActionDragon
{
    const ActionBase::State RoarAction::Run(const float& elapsedTime)
    {
        return ActionBase::State::Run;
    }

    void RoarAction::DrawDebug()
    {

    }
}

#pragma endregion ---------- 咆哮 ----------

#pragma region ---------- 攻撃 ----------
// ----- SlamAttackAction -----
namespace ActionDragon
{
    const ActionBase::State SlamAttackAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange())
        {
            Finalize();

            return ActionBase::State::Failed;
        }

        switch (owner_->GetStep())
        {
        case 0:
            // アニメーション設定
            PlayAnimation();

            // 現在の攻撃アクションを設定する
            owner_->SetCurrentAttackAction(Enemy::AttackAction::SlamAttack);

            // カウンター有効範囲を設定する
            PlayerManager::Instance().GetPlayer()->SetCounterActiveRadius(6.0f);

            // パーティクル生成
            slamAttackParticle_ = new SlamAttackParticle();

            // 変数初期化
            isCreateChargeEffect_ = false;
            isPlayExplosionParticle_ = false;
            isActiveJustDodge_ = false;

            isSetTarget_ = false;
            isMovement_ = true;
            moveTimer_ = 0.0f;

            isPlayBreathSE_ = false;    
            isPlayChargeSE_ = false;
            isPlayExplosionSE_ = false;

            owner_->SetStep(1);
            
            break;
        case 1:

            // ジャスト回避判定更新
            UpdateJustDodgeStatus();


            PlayChargeEffect();
            slamAttackParticle_->UpdateHandPosition(owner_->GetJointPosition("Dragon15_l_hand"));

            if (owner_->GetAnimationSeconds() > 1.02f && isPlayExplosionParticle_ == false)
            {
                DirectX::XMFLOAT3 emitterPosition = owner_->GetJointPosition("Dragon15_l_hand");
                emitterPosition.y = 0.0f;
                DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(1.0, 0.42, 0.13);
                slamAttackParticle_->PlayExplosionParticle(emitterPosition, color);

                

                isPlayExplosionParticle_ = true;
            }

            // チャージSEを再生する
            if (owner_->GetAnimationSeconds() > 0.4f && isPlayChargeSE_ == false)
            {
                chargeSENum_ = AudioManager::Instance().PlaySE(SE::Charge0);

                isPlayChargeSE_ = true;
            }
            if (owner_->GetAnimationSeconds() > 0.3f && isPlayBreathSE_ == false)
            {
                AudioManager::Instance().PlaySE(SE::Breath0);

                isPlayBreathSE_ = true;
            }
            if (owner_->GetAnimationSeconds() > 0.9f)
            {
                AudioManager::Instance().StopSE(SE::Charge0, chargeSENum_);
            }
            // 爆発SEを再生する
            if (owner_->GetAnimationSeconds() > 1.03f && isPlayExplosionSE_ == false)
            {
                AudioManager::Instance().PlaySE(SE::Explosion0);
                AudioManager::Instance().PlaySE(SE::Explosion1);

                if (owner_->CalcDistanceToPlayer() < 10.0f) Camera::Instance().ScreenVibrate(0.3f, 0.2f);

                isPlayExplosionSE_ = true;
            }

            if (isCreateChargeEffect_)
            {
                if (owner_->GetUseEffekSeerEffect())
                {
                    EffectManager::Instance().GetEffect("Charge")->SetPosition(powerEffectHandle_, owner_->GetJointPosition("Dragon15_l_hand"));
                }
            }

            // 攻撃判定処理
            if (owner_->GetAnimationSeconds() > 1.04f)
            {
                if (owner_->GetIsAttackActive()) owner_->SetSlamAttackActiveFlag(false);
            }
            else if (owner_->GetAnimationSeconds() > slowStartFrame_)
            {
                if (owner_->GetIsAttackActive() == false) owner_->SetSlamAttackActiveFlag();
            }

            // ターゲット位置設定
            SetTargetPosition();

            // 移動処理
            Move(elapsedTime);

            // 旋回処理
            Turn(elapsedTime);

            // アニメーションの速度を調整する
            UpdateAnimationSpeed();

            if (owner_->IsPlayAnimation() == false)
            {
                Finalize();

                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }
    
    // ----- ImGui用 ----- 
    void SlamAttackAction::DrawDebug()
    {
        if (ImGui::TreeNodeEx("SlamAttack", ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- TransitionTime ----------"))
            {
                ImGui::DragFloat("Walk", &transitionWalk_, 0.01f, 0.0f, 0.5f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- BlendFrame ----------"))
            {
                ImGui::DragFloat("Walk", &blendFrameWalk_, 0.01f, 0.0f, 1.0f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Slow ----------"))
            {
                ImGui::DragFloat("SlowAnimationSpeed", &slowAnimationSpeed_, 0.01f, 0.0f, 3.0f);
                ImGui::DragFloat("SlowStartFrame", &slowStartFrame_, 0.01f, 0.0f, 3.0f);
                ImGui::DragFloat("SlowEndFrame", &slowEndFrame_, 0.01f, 0.0f, 3.0f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Movement ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("StartPosition", &startPosition_.x);
                ImGui::DragFloat("TargetPosition", &targetPosition_.x);
                ImGui::DragFloat("TargetSetFrame", &targetSetFrame_);
                ImGui::Checkbox("IsSetTarget", &isSetTarget_);
                ImGui::Checkbox("IsMovement", &isMovement_);
                ImGui::DragFloat("MoveLength", &moveLength_);
                ImGui::DragFloat("MinMoveLength", &minMoveLength_, 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat("MaxMoveLength", &maxMoveLength_, 0.01f, 0.0f, 20.0f);
                ImGui::DragFloat("MoveTimer", &moveTimer_);
                ImGui::DragFloat("MoveSpeed", &moveSpeed_);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Rotation ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("EndFrame", &rotationEndFrame_, 0.01f, 0.0f, 3.0f);

                ImGui::TreePop();
            }
             

            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void SlamAttackAction::PlayAnimation()
    {
        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());
        float transitionTime = 0.1f;
        float blendAnimationFrame = 0.0f;

        if (animationIndex == Enemy::DragonAnimation::Run)
        {
            transitionTime = transitionWalk_;
            blendAnimationFrame = blendFrameWalk_;
        }

        owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackSlam0, false, 1.0f, blendAnimationFrame);
        owner_->SetTransitionTime(transitionTime);
    }

    // ----- 終了化 -----
    void SlamAttackAction::Finalize()
    {
        if (slamAttackParticle_ != nullptr)
        {
            slamAttackParticle_->Remove();
            slamAttackParticle_ = nullptr;
        }

        if (owner_->GetUseEffekSeerEffect())
        {
            EffectManager::Instance().GetEffect("Charge")->Stop(powerEffectHandle_);
        }
    }

    // ----- アニメーションの速度を調整する -----
    void SlamAttackAction::UpdateAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();
        float animationSpeed = 0.0f;

        if (animationSeconds < 0.25f)
        {
            animationSpeed = 0.6f;
        }
        else if (animationSeconds < 0.45f)
        {
            animationSpeed = 0.8f;
        }
        // 攻撃を出す前の数フレーム感をスローにする
        else if(animationSeconds > slowStartFrame_ && animationSeconds < slowEndFrame_)
        {
            animationSpeed = slowAnimationSpeed_;
        }
        else
        {
            animationSpeed = 1.0f;
        }

        owner_->SetAnimationSpeed(animationSpeed);
    }

    // ----- ジャスト回避判定更新 -----
    void SlamAttackAction::UpdateJustDodgeStatus()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // 判定開始フレームに達していない
        if (animationSeconds < justDodgeStartFrame_) return;
        // 判定終了フレームを越している
        if (animationSeconds > justDodgeEndFrame_)
        {
            if (isActiveJustDodge_ == false) return;

            // ジャスト回避判定を無くす
            owner_->SetJustDodgeActiveFlag(Enemy::AttackAction::SlamAttack, false);
            isActiveJustDodge_ = false;

            return;
        }

        // 既に判定を設定している
        if (isActiveJustDodge_) return;

        owner_->SetJustDodgeActiveFlag(Enemy::AttackAction::SlamAttack, true);
        isActiveJustDodge_ = true;
    }

    // ----- チャージエフェクト再生 -----
    void SlamAttackAction::PlayChargeEffect()
    {
        // もう既に生成している
        if (isCreateChargeEffect_) return;

        if (owner_->GetAnimationSeconds() < 0.4f) return;

        // チャージエフェクトとパーティクルを再生する
        DirectX::XMFLOAT3 emitterPosition = owner_->GetJointPosition("Dragon15_l_hand");
        DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(1.0, 0.42, 0.13);
        slamAttackParticle_->PlayChargeParticle(emitterPosition, color);
        
        if (owner_->GetUseEffekSeerEffect())
        {
            powerEffectHandle_ = EffectManager::Instance().GetEffect("Charge")->Play(emitterPosition, 1.0f, 1.0f);
        }

        isCreateChargeEffect_ = true;
    }

    // ----- 目標地点設定 -----
    void SlamAttackAction::SetTargetPosition()
    {
        // 既に目標地点を設定している
        if (isSetTarget_) return;
        // 目標地点を設定するフレームに達していない
        if (owner_->GetAnimationSeconds() < targetSetFrame_) return;

        // スタート地点を設定
        startPosition_ = owner_->GetTransform()->GetPosition();

        // 距離が近ければ移動処理をしない
        float distance = owner_->CalcDistanceToPlayer();
        if (distance <= minMoveLength_)
        {
            isMovement_ = false;
            isSetTarget_ = true;
            return;
        }

        // 移動量を求める
        moveLength_ = distance - minMoveLength_;
        // 移動量が最大移動量よりも大きかったら丸める
        if (moveLength_ > maxMoveLength_) moveLength_ = maxMoveLength_;

        // 目標地点を設定する
        DirectX::XMFLOAT3 direction = XMFloat3Normalize(owner_->CalcDirectionToPlayer());        
        targetPosition_ = startPosition_ + direction * moveLength_;

        isSetTarget_ = true;
    }

    // ----- 移動処理 -----
    void SlamAttackAction::Move(const float& elapsedTime)
    {
        // 移動処理をしない
        if (isMovement_ == false) return;
        // 移動フレームに達していない
        if (owner_->GetAnimationSeconds() < targetSetFrame_) return;

        moveTimer_ += moveSpeed_ * elapsedTime;
        moveTimer_ = std::min(moveTimer_, 1.0f);

        owner_->GetTransform()->SetPosition(XMFloat3Lerp(startPosition_, targetPosition_, moveTimer_));
    }

    // ----- 旋回処理 -----
    void SlamAttackAction::Turn(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds < targetSetFrame_ || animationSeconds > rotationEndFrame_) return;

        owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());
    }
}

// ----- ComboSlam -----
namespace ActionDragon
{
    const ActionBase::State ComboSlamAttackAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange())
        {
            Finalize();

            return ActionBase::State::Failed;
        }

        switch (owner_->GetStep())
        {
        case 0:// 初期化
            // アニメーション再生
            PlayAnimation();

            // パーティクル生成
            slamAttackParticle_ = new SlamAttackParticle();

            isPlayChargeParticle_ = false;
            isPlayExplosionParticle_ = false;
            isLoop_ = true;

            owner_->SetStep(1);

            break;
        case 1:

            // ----- パーティクル -----
            PlayChargeEffect(particleColor_[0]);
            PlayExplosionEffect(particleColor_[0]);
            slamAttackParticle_->UpdateHandPosition(owner_->GetJointPosition("Dragon15_r_hand")); 
        

            UpdateAnimationSpeed();

            if(owner_->GetAnimationSeconds() > changeAnimationFrame_)
            {
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackComboSlam0, false, 1.0f, animationStartFrame_);
                owner_->SetTransitionTime(transitionSlamAttack_);

                // 初期化
                isPlayChargeParticle_ = false;
                isPlayExplosionParticle_ = false;

                owner_->SetStep(2);
            }

            break;
        case 2:

            PlayChargeEffect(particleColor_[1]);
            PlayExplosionEffect(particleColor_[1]);
            slamAttackParticle_->UpdateHandPosition(owner_->GetJointPosition("Dragon15_r_hand"));
        
            UpdateAnimationSpeed();

            if (isLoop_)
            {
                if (owner_->GetAnimationSeconds() > changeAnimationFrame_)
                {
                    owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackComboSlam0, false, 1.0f, animationStartFrame_);
                    owner_->SetTransitionTime(transitionSlamAttack_);

                    // 初期化
                    isPlayChargeParticle_ = false;
                    isPlayExplosionParticle_ = false;

                    isLoop_ = false;
                    owner_->SetStep(3);
                }
            }

            break;
        case 3:

            PlayChargeEffect(particleColor_[2]);
            PlayExplosionEffect(particleColor_[2]);
            slamAttackParticle_->UpdateHandPosition(owner_->GetJointPosition("Dragon15_r_hand"));

            UpdateAnimationSpeed();

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
            break;
        }

        return ActionBase::State::Run;
    }

    // ----- ImGui用 -----
    void ComboSlamAttackAction::DrawDebug()
    {
        if (ImGui::TreeNodeEx("ComboSlamAttack", ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- Particle ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::ColorEdit3("First", &particleColor_[0].x);
                ImGui::ColorEdit3("Second", &particleColor_[1].x);
                ImGui::ColorEdit3("Third", &particleColor_[2].x);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Animation ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("ChangeAnimationFrame", &changeAnimationFrame_, 0.01f, 0.0f, 2.0f);
                ImGui::DragFloat("AnimationStartFrame", &animationStartFrame_, 0.01f, 0.0f, 2.0f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Slow ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("SlowAnimationSpeed", &slowAnimationSpeed_, 0.01f, 0.0f, 3.0f);
                ImGui::DragFloat("SlowStartFrame", &slowStartFrame_, 0.01f, 0.0f, 2.0f);
                ImGui::DragFloat("SlowEndFrame", &slowEndFrame_, 0.01f, 0.0f, 2.0f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- TransitionTime ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("SlamAttack", &transitionSlamAttack_, 0.01f, 0.0f, 1.0f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- 終了化 -----
    void ComboSlamAttackAction::Finalize()
    {
        if (slamAttackParticle_ != nullptr)
        {
            slamAttackParticle_->Remove();
            slamAttackParticle_ = nullptr;
        }
    }

    // ----- アニメーション再生 -----
    void ComboSlamAttackAction::PlayAnimation()
    {
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackComboSlam0, false);
    }

    // ----- アニメーション速度調整 -----
    void ComboSlamAttackAction::UpdateAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();
        float animationSpeed = 1.0f;

        if (animationSeconds > slowStartFrame_ && animationSeconds < slowEndFrame_)
        {
            animationSpeed = slowAnimationSpeed_;
        }

        owner_->SetAnimationSpeed(animationSpeed);
    }

    // ----- チャージエフェクト再生 -----
    void ComboSlamAttackAction::PlayChargeEffect(const DirectX::XMFLOAT3& color)
    {
        if (isPlayChargeParticle_) return;

        if (owner_->GetAnimationSeconds() < 0.75f) return;

        DirectX::XMFLOAT3 emitterPosition = owner_->GetJointPosition("Dragon15_r_hand");
        slamAttackParticle_->PlayChargeParticle(emitterPosition, color);

        isPlayChargeParticle_ = true;
    }

    // ----- 爆発エフェクト再生 -----
    void ComboSlamAttackAction::PlayExplosionEffect(const DirectX::XMFLOAT3& color)
    {
        if (isPlayExplosionParticle_) return;

        if (owner_->GetAnimationSeconds() < 1.455) return;

        DirectX::XMFLOAT3 emitterPosition = owner_->GetJointPosition("Dragon15_r_hand");
        emitterPosition.y = 0.0f;
        slamAttackParticle_->PlayExplosionParticle(emitterPosition, color);

        isPlayExplosionParticle_ = true;
    }
}

// ----- TurnAttackAction -----
namespace ActionDragon
{
    const ActionBase::State TurnAttackAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange())
        {
            Finalize();
            return ActionBase::State::Failed;
        }

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// 初期設定
            // アニメーション再生
            PlayAnimation();

            // 現在の攻撃アクションを設定する
            owner_->SetCurrentAttackAction(Enemy::AttackAction::TurnAttack);

            // 現時点ではルートモーションを使用しない
            owner_->SetUseRootMotion(false);

            // カウンター有効範囲を設定

            // パーティクル生成
            tailParticle_ = new TailParticle();
            {
                std::vector<DirectX::XMFLOAT3> jointPosition;
                jointPosition.emplace_back(owner_->GetTransform()->GetPosition());
                jointPosition.emplace_back(owner_->GetJointPosition("Dragon15_tail_01"));
                jointPosition.emplace_back(owner_->GetJointPosition("Dragon15_tail_02"));
                jointPosition.emplace_back(owner_->GetJointPosition("Dragon15_tail_03"));
                jointPosition.emplace_back(owner_->GetJointPosition("Dragon15_tail_04"));
                jointPosition.emplace_back(owner_->GetJointPosition("Dragon15_tail_05"));
                tailParticle_->UpdateJointPosition(jointPosition);
            }

            // 変数初期化
            addForceData_.Initialize(1.5f, 0.3f, 0.5f);
            isPlayTailParticle_ = false;
            isPlayTailTrailParticle_ = false;
            isRemoveParticle_ = false;

            isPlayTurnSE_ = false;

            // Attackステートへ
            SetState(STATE::Attack);

            break;
        case STATE::Attack:
            // ルートモーション使用設定
            if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
            {
                owner_->SetUseRootMotion(true);
            }

            if (owner_->GetAnimationSeconds() > slowStartFrame_ && isPlayTailParticle_ == false)
            {
                tailParticle_->PlayTailParticle();
                isPlayTailParticle_ = true;
            }

            if (owner_->GetAnimationSeconds() > 1.55f && isPlayTailTrailParticle_ == false)
            {
                isPlayTailTrailParticle_ = true;
                tailParticle_->PlayTailTrailParticle();
            }

            // 回転攻撃SE再生
            if (owner_->GetAnimationSeconds() > 1.5f && isPlayTurnSE_ == false)
            {
                AudioManager::Instance().PlaySE(SE::Turn0);

                isPlayTurnSE_ = true;
            }

            // 回転攻撃の終わりに、尻尾についているパーティクルを落とす
            if (owner_->GetAnimationSeconds() > removeFrame_ && isRemoveParticle_ == false)
            {
                tailParticle_->Remove();
                isRemoveParticle_ = true;
            }

            // 移動処理
            if (isAbleMove_ && addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            // 攻撃判定 & ジャスト回避判定 更新
            if (owner_->GetAnimationSeconds() > 1.85f)
            {
                owner_->SetJustDodgeActiveFlag(Enemy::AttackAction::TurnAttack, false);

                if (owner_->GetIsAttackActive()) owner_->SetTurnAttackActiveFlag(false);
            }
            else if (owner_->GetAnimationSeconds() > 1.55f)
            {
                owner_->SetJustDodgeActiveFlag(Enemy::AttackAction::TurnAttack, true);

                if (owner_->GetIsAttackActive() == false) owner_->SetTurnAttackActiveFlag();
            }

            // アニメーションの速度を調整する
            UpdateAnimationSpeed();

            // パーティクル更新
            {
                std::vector<DirectX::XMFLOAT3> jointPosition;
                jointPosition.emplace_back(owner_->GetTransform()->GetPosition());
                jointPosition.emplace_back(owner_->GetJointPosition("Dragon15_tail_01"));
                jointPosition.emplace_back(owner_->GetJointPosition("Dragon15_tail_02"));
                jointPosition.emplace_back(owner_->GetJointPosition("Dragon15_tail_03"));
                jointPosition.emplace_back(owner_->GetJointPosition("Dragon15_tail_04"));
                jointPosition.emplace_back(owner_->GetJointPosition("Dragon15_tail_05"));
                tailParticle_->UpdateJointPosition(jointPosition);
            }

            if (owner_->IsPlayAnimation() == false)
            {
                Finalize();

                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }

    // ----- ImGui用 -----
    void TurnAttackAction::DrawDebug()
    {
        if (ImGui::TreeNodeEx("Trun", ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- TransitionTime ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("Walk", &transitionWalk_, 0.01f, 0.0f, 0.5f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- BlendFrame ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("Walk", &blendFrameWalk_, 0.01f, 0.0f, 1.0f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Slow ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("StartFrame", &slowStartFrame_, 0.01f, 0.0f, 3.5f);
                ImGui::DragFloat("EndFrame", &slowEndFrame_, 0.01f, 0.0f, 3.5f);
                ImGui::DragFloat("SlowSpeed", &slowSpeed_, 0.01f, 0.0f, 1.0f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- AttackRecovery ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("RecoveryFrame", &recoveryFrame_, 0.01f, 0.0f, 3.5f);
                ImGui::DragFloat("RecoverySpeed", &recoverySpeed_, 0.1f, 0.0f, 1.0f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Movement ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("AbleMove", &isAbleMove_);
                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void TurnAttackAction::PlayAnimation()
    {
        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());
        float TransitionTime = 0.1f;
        float blendAnimationFrame = 0.0f;

        if (animationIndex == Enemy::DragonAnimation::Run)
        {
            TransitionTime = transitionWalk_;
            blendAnimationFrame = blendFrameWalk_;
        }

        owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackTurn, false, 1.0f, blendAnimationFrame);
        owner_->SetTransitionTime(TransitionTime);
    }

    // ----- 終了化 -----
    void TurnAttackAction::Finalize()
    {
        // ルートモーション使用終了
        owner_->SetUseRootMotion(false);

        if (tailParticle_ != nullptr)
        {
            tailParticle_->Remove();
            tailParticle_ = nullptr;
        }
    }

    // ----- アニメーションの速度を調整する -----
    void TurnAttackAction::UpdateAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();
        float animationSpeed = 0.0f;

        // 予備動作としてスローにする
        if (animationSeconds > slowStartFrame_ && animationSeconds < slowEndFrame_)
        {
            animationSpeed = slowSpeed_;
        }
        // 予備動作よりも前
        else if (animationSeconds < slowStartFrame_)
        {
            animationSpeed = 0.8f;
        }
        else if (animationSeconds > recoveryFrame_)
        {
            animationSpeed = recoverySpeed_;
        }
        else
        {
            animationSpeed = 1.0f;
        }

        owner_->SetAnimationSpeed(animationSpeed);
    }
}

// ----- GuardAction -----
namespace ActionDragon
{
    const ActionBase::State GuardAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange())
        {
            return ActionBase::State::Failed;
        }

        switch (owner_->GetStep())
        {
        case 0:
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackKnockBackStart, false);

            // 現在の攻撃アクションを設定する
            owner_->SetCurrentAttackAction(Enemy::AttackAction::GuardAttack);

            // ルートモーションを使用しない
            owner_->SetUseRootMotion(false);

            loopCounter_ = 0;

            isPlayGuardSE_ = false;
            isPlayGuardAttackSE_ = false;

            owner_->SetStep(1);

            break;
        case 1:

            if (owner_->GetAnimationSeconds() > guardSEPlayFrame_ && isPlayGuardSE_ == false)
            {
                AudioManager::Instance().PlaySE(SE::Guard);

                isPlayGuardSE_ = true;
            }

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackKnockBackLoop, false);

                // 現在の体力を保存する
                oldHealth_ = owner_->GetHealth();

                owner_->SetStep(2);
                return ActionBase::State::Run;
            }

            break;
        case 2:
            // 攻撃をくらったか判定
            if (owner_->GetHealth() != oldHealth_)
            {
                // 攻撃を受けたのでAttackに移行
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackKnockBackEnd0, false);

                owner_->SetStep(4);
                return ActionBase::State::Run;
            }

            // アニメーション再生終了
            if (owner_->IsPlayAnimation() == false)
            {
                // まだループする回数が残っている
                if (loopCounter_ < loopMax_)
                {
                    // ステート変更
                    owner_->SetStep(3);
                    break;
                }
                // ループ終了
                else
                {
                    // 攻撃をくらわなかった
                    owner_->PlayAnimation(Enemy::DragonAnimation::AttackKnockBackEnd1, false);

                    // ステート変更
                    owner_->SetStep(5);
                }
                return ActionBase::State::Run;
            }

            break;
        case 3:
            // アニメーション設定
            owner_->PlayAnimation(Enemy::DragonAnimation::AttackKnockBackLoop, false);

            ++loopCounter_;

            owner_->SetStep(2);
            break;
        case 4:// 攻撃

            // 攻撃SEを再生
            if (owner_->GetAnimationSeconds() > guardAttackSEPlayFrame_ && isPlayGuardAttackSE_ == false)
            {
                AudioManager::Instance().PlaySE(SE::GuardAttack);

                isPlayGuardAttackSE_ = true;
            }

            // 攻撃判定処理
            if (owner_->GetAnimationSeconds() > 1.0f)
            {
                owner_->SetJustDodgeActiveFlag(Enemy::AttackAction::GuardAttack, false);

                if (owner_->GetIsAttackActive())
                    owner_->SetGuardAttackActiveFlag(false);
            }
            else if (owner_->GetAnimationSeconds() > 0.36f)
            {
                owner_->SetJustDodgeActiveFlag(Enemy::AttackAction::GuardAttack, true);

                if (owner_->GetIsAttackActive() == false)
                    owner_->SetGuardAttackActiveFlag();
            }

            if (owner_->GetAnimationSeconds() > 1.75f)
            {
                // ステートリセット
                owner_->SetStep(0);

                return ActionBase::State::Complete;
            }

            break;
        case 5:
            if (owner_->GetAnimationSeconds() > 1.15f)
            {
                // ステートリセット
                owner_->SetStep(0);

                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }

    // ----- ImGui用 -----
    void GuardAction::DrawDebug()
    {
        if (ImGui::TreeNodeEx("Guard", ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- SE ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("GuardSEPlayFrame", &guardSEPlayFrame_, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("GuardAttackSEPlayFrame", &guardSEPlayFrame_, 0.01f, 0.0f, 1.0f);

                ImGui::Checkbox("IsPlayGuardSE", &isPlayGuardSE_);
                ImGui::Checkbox("IsPlayGuardAttackSE", &isPlayGuardAttackSE_);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }
}

// ----- TackleAction -----
namespace ActionDragon
{
    const ActionBase::State TackleAction::Run(const float& elapsedTime)
    {    
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) isCurrentNodeCancelled_ = true;
        if (isCurrentNodeCancelled_)
        {
            if (owner_->GetIsDead())
            {
                Finalize();

                return ActionBase::State::Failed;
            }

            if (PlayerManager::Instance().GetPlayer()->GetCurrentState() != Player::STATE::RushAttack)
            {                
                Finalize();

                return ActionBase::State::Failed;
            }

            // ノード中断待機中なのでここで終了
            return ActionBase::State::Run;
        }

        switch (owner_->GetStep())
        {
        case 0:// 初期化
            // アニメーション再生
            PlayAnimation();

            // 現在の攻撃アクションを設定する
            owner_->SetCurrentAttackAction(Enemy::AttackAction::TackleAttack);

            // 変数初期化
            addForceData_.Initialize(rotationEndFrame_, 0.6f, 0.6f);
            easingTimer_ = 0.0f;
            isCurrentNodeCancelled_ = false;

            for (int i = 0; i < maxFootSteps_; ++i)
            {
                isPlayFootSteps_[i] = false;
            }

            owner_->SetStep(1);
            break;
        case 1:
            // ルートモーションを使用する
            if (owner_->GetUseRootMotionMovement() == false && owner_->GetIsBlendAnimation() == false)
            {
                owner_->SetUseRootMotion(true);
            }

            // 旋回処理
            if(owner_->GetAnimationSeconds() > rotationStartFrame_) Turn(elapsedTime);

            // アニメーション再生終了
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackTackle1, false);

                owner_->SetStep(2);
            }

            break;
        case 2:
            // 足音SE再生
            if (owner_->GetAnimationSeconds() > 0.1f && isPlayFootSteps_[0] == false)
            {
                AudioManager::Instance().PlaySE(SE::FootSteps);

                isPlayFootSteps_[0] = true;
            }
            if (owner_->GetAnimationSeconds() > 0.65f && isPlayFootSteps_[1] == false)
            {
                AudioManager::Instance().PlaySE(SE::FootSteps);

                isPlayFootSteps_[1] = true;
            }

            // 移動処理
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                const DirectX::XMFLOAT3 vec = XMFloat3Normalize(owner_->CalcDirectionToPlayer());

                owner_->AddForce(vec, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            // 旋回処理
            if (owner_->GetAnimationSeconds() < rotationEndFrame_) Turn(elapsedTime);

            // 攻撃判定&ジャスト回避判定 有効化
            if (owner_->GetIsAttackActive() == false)
            {
                if (owner_->GetAnimationSeconds() > 0.2f)
                {
                    owner_->SetTackleAttackActiveFlag();
                    owner_->SetJustDodgeActiveFlag(Enemy::AttackAction::TackleAttack, true);
                }
            }

            // 前足が地面に埋まらないようにする
            AdjustForGroundPenetration(elapsedTime);

            // アニメーション再生終了
            if (owner_->IsPlayAnimation() == false)
            {
                // 地面に埋まらないようにY座標をリセット
                owner_->GetTransform()->SetPositionY(0.0f);

                addForceData_.Initialize(0.03f, 0.5f, 0.6f);

                owner_->PlayAnimation(Enemy::DragonAnimation::AttackTackle3, false);
                // ルートモーションリセット(使用する)
                owner_->SetUseRootMotion(true);

                owner_->SetStep(3);
            }
            break;
        case 3:

            // 足音SE再生
            if (owner_->GetAnimationSeconds() > 0.34f && isPlayFootSteps_[2] == false)
            {
                AudioManager::Instance().PlaySE(SE::FootSteps);

                isPlayFootSteps_[2] = true;
            }

            // 移動処理
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            // 攻撃判定無効化
            if (owner_->GetIsAttackActive())
            {
                if (owner_->GetAnimationSeconds() > 0.6f)
                {
                    owner_->SetTackleAttackActiveFlag(false);
                    owner_->SetJustDodgeActiveFlag(Enemy::AttackAction::TackleAttack, false);
                }
            }

            // アニメーション再生終了
            if (owner_->IsPlayAnimation() == false)
            {
                Finalize();

                owner_->SetStep(0);

                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }

    // ----- ImGui用 -----
    void TackleAction::DrawDebug()
    {
        if (ImGui::TreeNodeEx("Tackle", ImGuiTreeNodeFlags_Framed)) 
        {
            if (ImGui::TreeNodeEx("---------- Rotation ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("StartFrame", &rotationStartFrame_, 0.01f, 0.0f, 2.0f);
                ImGui::Text(u8"別アニメーション");
                ImGui::DragFloat("EndFrame", &rotationEndFrame_, 0.01f, 0.0f, 2.0f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- 終了化 -----
    void TackleAction::Finalize()
    {
        // ルートモーションの使用終了
        owner_->SetUseRootMotion(false);

        // Y座標をリセットする
        owner_->GetTransform()->SetPositionY(0.0f);

        // X軸回転をリセットする
        owner_->GetTransform()->SetRotationX(0.0f);

        isCurrentNodeCancelled_ = false;
    }

    // ----- アニメーション再生 -----
    void TackleAction::PlayAnimation()
    {
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackTackle0, false);
    }

    // ----- 旋回処理 -----
    void TackleAction::Turn(const float& elapsedTime)
    {
        DirectX::XMFLOAT3 targetPosition = PlayerManager::Instance().GetTransform()->GetPosition();
        owner_->Turn(elapsedTime, targetPosition);
    }
    
    // ----- 前足が地面に埋まらないようにする -----
    void TackleAction::AdjustForGroundPenetration(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();
        if (animationSeconds <= 0.45f) return;

        const float maxAngle = -10.0f;
        const float totalFrame = 0.1f;

        if (animationSeconds > 0.65f)
        {
            easingTimer_ -= elapsedTime;
            easingTimer_ = std::max(easingTimer_, 0.0f);
        }
        else if (animationSeconds > 0.45f)
        {
            easingTimer_ += elapsedTime;
            easingTimer_ = std::min(easingTimer_, totalFrame);
        }

        float angle = Easing::InSine(easingTimer_, totalFrame, maxAngle, 0.0f);
        owner_->GetTransform()->SetRotationX(DirectX::XMConvertToRadians(angle));
    }
}


// ----- SuperNovaAction -----
namespace ActionDragon
{
    const ActionBase::State SuperNovaAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange())
        {
            Finalize();

            return ActionBase::State::Failed;
        }

        switch (owner_->GetStep())
        {
        case 0:
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Nova1, false);

            // 現在の攻撃アクションを設定する
            owner_->SetCurrentAttackAction(Enemy::AttackAction::SuperNova);

            // 変数初期化
            superNovaParticle_ = new SuperNovaParticle();
            
            isCreateLavaCrawlerParticle_ = false;
            isCreateCoreBurst_           = false;

            scaleLerpTimer_ = 0.0f;

            radialBlurTimer_ = 0.0f;
            preRadialBlurTimer_ = 0.0f;
            intenseBlurFrameCount_ = 0;

            owner_->SetStep(1);

            break;
        case 1:

            // チャージエフェクト生成
            GenerateChargeEffect(elapsedTime);

            // チャージエフェクト更新
            UpdateChargeEffect(elapsedTime);

            // メインエフェクト生成
            GenarateMainEffect(elapsedTime);

            // ラジアルブラー更新
            UpdateRadialBlur(elapsedTime);


            if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetStep(0);

                Finalize();

                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }

    void SuperNovaAction::DrawDebug()
    {
    }

    // ----- チャージエフェクト生成 -----
    void SuperNovaAction::GenerateChargeEffect(const float& elapsedTime)
    {
        // もう既に生成しているためここで終了
        if (isCreateLavaCrawlerParticle_) return;

        // 生成フレームに達していない
        if (owner_->GetAnimationSeconds() <= 0.65f) return;

        // チャージエフェクトとパーティクルを再生する
        DirectX::XMFLOAT3 emitterPosition = owner_->GetJointPosition("Dragon15_neck_1");
        powerEffectHandle_ = EffectManager::Instance().GetEffect("Power")->Play(emitterPosition, 0.1f, 1.0f);
        
        superNovaParticle_->PlayChargeParticle(emitterPosition);
        
        emitterPosition.y = 0.3f;
        superNovaParticle_->PlayLavaCrawlerParticle(elapsedTime, emitterPosition);

        isCreateLavaCrawlerParticle_ = true;
    }

    // ----- メインエフェクト生成 -----
    void SuperNovaAction::GenarateMainEffect(const float& elapsedTime)
    {
        // もう既にメインエフェクトが生成されている
        if (isCreateCoreBurst_) return;
        // 生成フレームに達していない
        if (owner_->GetAnimationSeconds() <= 3.9f) return;
        
        // チャージエフェクトを停止する
        EffectManager::Instance().GetEffect("Power")->Stop(powerEffectHandle_);

        const DirectX::XMFLOAT3 emitterPosition = owner_->GetJointPosition("Dragon15_neck_1");

        // 爆発パーティクル再生
        superNovaParticle_->PlayCoreBurstParticle(elapsedTime, emitterPosition);
        // 爆発エフェクト再生
        Effect* superNovaEffect = EffectManager::Instance().GetEffect("SuperNova");
        superNovaEffect->Play(emitterPosition, 1.3f, 1.0f);

        // 白飛び画像を生成
        UIFlashOut* uiFlashOut = new UIFlashOut();

        // カメラシェイク
        Camera::Instance().ScreenVibrate(0.3f, 1.5f);

        // 地面を這うパーティクルの速度を上げる
        superNovaParticle_->SetLavaCrawlerParticleSpeed(30.0f);

        isCreateCoreBurst_ = true;
    }

    // ----- チャージエフェクト更新 -----
    void SuperNovaAction::UpdateChargeEffect(const float& elapsedTime)
    {
        // まだチャージエフェクトが生成されていない
        if (isCreateLavaCrawlerParticle_ == false) return;
        // メインの爆発エフェクトが生成されているので処理しない
        if (isCreateCoreBurst_) return;

        PostProcess::Instance().SetUseRadialBlur();
        preRadialBlurTimer_ += elapsedTime;
        preRadialBlurTimer_ = std::min(preRadialBlurTimer_, 1.0f);

        // offsetを使って振動を表現する
        const float strength = XMFloatLerp(0.0f, 0.1f, preRadialBlurTimer_);
        const float offset = rand() % 10 * 0.01f;

        PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = strength + offset;
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = maxSampleCount_;

        // ブラーの開始中心点を決める
        const DirectX::XMFLOAT3 dragonNeckPosition = owner_->GetJointPosition("Dragon15_neck_1");
        DirectX::XMFLOAT2 centerPosition = Sprite::ConvertToScreenPos(dragonNeckPosition);
        centerPosition.x /= SCREEN_WIDTH;
        centerPosition.y /= SCREEN_HEIGHT;
        // 中心点を 0.0 ~ 1.0 の間に収める
        centerPosition.x = std::clamp(centerPosition.x, 0.0f, 1.0f);
        centerPosition.y = std::clamp(centerPosition.y, 0.0f, 1.0f);
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->uvOffset_ = centerPosition;

        // エフェクトのサイズを徐々に大きくする
        const float speed = 0.4f;
        scaleLerpTimer_ += speed * elapsedTime;
        scaleLerpTimer_ = std::min(scaleLerpTimer_, 1.0f);

        const float scale = XMFloatLerp(0.1f, 7.0f, scaleLerpTimer_);
        EffectManager::Instance().GetEffect("Power")->SetScale(powerEffectHandle_, scale);

        const float radius = XMFloatLerp(0.0f, 5.0f, scaleLerpTimer_);
        superNovaParticle_->SetChargeParticleRadius(radius);
    }

    // ----- ラジアルブラー更新 -----
    void SuperNovaAction::UpdateRadialBlur(const float& elapsedTime)
    {
        // メインの爆発パーティクルが生成されていないので更新しない
        if (isCreateCoreBurst_ == false) return;

        // ラジアルブラーを使用する サンプリング回数は５回
        PostProcess::Instance().SetUseRadialBlur();
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = maxSampleCount_;

        // ブラーの開始中心点を決める
        const DirectX::XMFLOAT3 dragonNeckPosition = owner_->GetJointPosition("Dragon15_neck_1");
        DirectX::XMFLOAT2 centerPosition = Sprite::ConvertToScreenPos(dragonNeckPosition);
        centerPosition.x /= SCREEN_WIDTH;
        centerPosition.y /= SCREEN_HEIGHT;
        // 中心点を 0.0 ~ 1.0 の間に収める
        centerPosition.x = std::clamp(centerPosition.x, 0.0f, 1.0f);
        centerPosition.y = std::clamp(centerPosition.y, 0.0f, 1.0f);

        PostProcess::Instance().GetRadialBlurConstants()->GetData()->uvOffset_ = centerPosition;

        // ブラー開始の数フレームを最大強度のブラーをかける
        if (intenseBlurFrameCount_ < intenseBlurFrame_)
        {
            PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = 1.5f;
            ++intenseBlurFrameCount_;
            return;
        }

        // ラジアルブラーの強度を徐々に下げる
        if (radialBlurTimer_ < 2.0f)
        {
            radialBlurTimer_ += elapsedTime;
            radialBlurTimer_ = std::min(radialBlurTimer_, 1.0f);

            const float strength = XMFloatLerp(1.0f, 0.0f, radialBlurTimer_);

            PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = strength;

            return;
        }

        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 1;
    }

    // ----- 終了処理 -----
    void SuperNovaAction::Finalize()
    {
        if (superNovaParticle_ != nullptr)
        {
            superNovaParticle_ = nullptr;
        }
        
        EffectManager::Instance().GetEffect("Power")->Stop(powerEffectHandle_);
        
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 1;
    }
}

// ----- WalkAction -----
namespace ActionDragon
{
    const ActionBase::State WalkAction::Run(const float& elapsedTime)
    {
        switch (owner_->GetStep())
        {
        case 0:
            {
                // プレイヤーとの距離と方向を算出
                direction_ = owner_->CalcDirectionToPlayerNoConsiderationY();

                // プレイヤーとの距離を算出
                targetLength_ = XMFloat3Length(direction_);

                // プレイヤーからのオフセット距離を引いた距離が移動最低距離より小さかったら処理しない
                moveLength_ = targetLength_ - offsetLength_;
                if(moveLength_ < minMoveLength_)
                {
                    return ActionBase::State::Failed;
                }

                // 移動距離が最大移動距離より大きかったら丸める
                if (moveLength_ > maxMoveLength_) moveLength_ = maxMoveLength_;                                
                
                initPosition_ = owner_->GetTransform()->GetPosition();
                targetPosition_ = initPosition_ + XMFloat3Normalize(direction_) * moveLength_;

                // 移動距離に応じて移動速度を調整
                lerpSpeed_ = XMFloatInverseLerp(minMoveLength_, maxMoveLength_, moveLength_);
                lerpSpeed_ = XMFloatLerp(maxLerpSpeed_, minLerpSpeed_, lerpSpeed_);
            }

            // アニメーション再生
            PlayAnimation();

            lerpTimer_ = 0.0f;

            owner_->SetStep(1);

            break;
        case 1:
            lerpTimer_ += lerpSpeed_ * elapsedTime;
            lerpTimer_ = std::min(lerpTimer_, 1.0f);

            owner_->GetTransform()->SetPosition(XMFloat3Lerp(initPosition_, targetPosition_, lerpTimer_));

            owner_->Turn(elapsedTime, targetPosition_);

            if (lerpTimer_ == 1.0f)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        case 2:
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
            break;
        }

        return ActionBase::State::Run;
    }


    void WalkAction::DrawDebug()
    {
        if (ImGui::TreeNodeEx("Walk", ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- Movement ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat(u8"プレイヤーまでの距離", &targetLength_);
                ImGui::DragFloat(u8"プレイヤーからオフセット距離", &offsetLength_);
                ImGui::DragFloat(u8"最大移動距離", &maxMoveLength_);
                ImGui::DragFloat(u8"最小移動距離", &minMoveLength_);
                ImGui::DragFloat(u8"移動距離", &moveLength_);
                
                ImGui::DragFloat("MaxLerpSpeed", &maxLerpSpeed_);
                ImGui::DragFloat("MinLerpSpeed", &minLerpSpeed_);
                ImGui::DragFloat("LerpSpeed", &lerpSpeed_);
                ImGui::DragFloat("LerpTimer", &lerpTimer_);

                ImGui::DragFloat3("TargetPosition", &targetPosition_.x);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- TransitionTime ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("SlamAttack", &transitionSlamAttack_, 0.01f, 0.0f, 0.5f);
                ImGui::DragFloat("TurnAttack", &transitionTurnAttack_, 0.01f, 0.0f, 0.5f);
                
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void WalkAction::PlayAnimation()
    {
        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());
        float transitionTime = 0.1f;

        if (animationIndex == Enemy::DragonAnimation::AttackSlam0)
        {
            transitionTime = transitionSlamAttack_;
        }
        else if (animationIndex == Enemy::DragonAnimation::AttackTurn)
        {
            transitionTime = transitionTurnAttack_;
        }

        owner_->PlayBlendAnimation(Enemy::DragonAnimation::Run, true);
        owner_->SetTransitionTime(transitionTime);
    }
}

#pragma endregion ---------- 攻撃 ----------

#pragma region ---------- 未完成 ----------
// ----- Meteor -----
namespace ActionDragon
{
    const ActionBase::State MeteorAction::Run(const float& elapsedTime)
    {
        switch (owner_->GetStep())
        {
        case 0:
            // アニメーション再生
            PlayAnimation();

            meteorParticle_ = new MeteorParticle();

            owner_->SetStep(1);

            break;
        case 1:

            // アニメーションの速度を調整
            UpdateAnimationSpeed();

            if (owner_->GetAnimationSeconds() > 1.4f && meteorParticle_->GetIsPlayMeteorGlowEffect() == false)
            {
                DirectX::XMFLOAT3 targetPosition = PlayerManager::Instance().GetTransform()->GetPosition();
                meteorParticle_->PlayMeteorGlowEffect(targetPosition);
                
                targetPosition.y = 0.0f;
                DirectX::XMFLOAT3 createPosition = owner_->GetTransform()->GetPosition();
                createPosition.y = 30.0f;
                Rock* rock = new Rock(createPosition, targetPosition);
            }

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }

    // ----- ImGui用 -----
    void MeteorAction::DrawDebug()
    {
        if (ImGui::TreeNodeEx("Meteor", ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- Slow ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("SlowSpeed", &slowAnimationSpeed_, 0.01f, 0.0f, 1.0f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void MeteorAction::PlayAnimation()
    {
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::Meteor, false);
    }

    // ----- アニメーションの速度を調整 -----
    void MeteorAction::UpdateAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();
        float animationSpeed = 1.0f;

        if (animationSeconds > 1.5f)
        {
            animationSpeed = slowAnimationSpeed_;
        }

        owner_->SetAnimationSpeed(animationSpeed);
    }

    // ----- 終了化 -----
    void MeteorAction::Finalize()
    {
        if (meteorParticle_ != nullptr)
        {
            ParticleManager::Instance().Remove(meteorParticle_);
            meteorParticle_ = nullptr;
        }
    }
}

// ----- FireBreathAction -----
namespace ActionDragon
{
    const ActionBase::State FireBreathAction::Run(const float& elapsedTime)
    {
        return ActionBase::State::Run;
    }

    void FireBreathAction::DrawDebug()
    {
    }
}
#pragma endregion ---------- 未完成 ----------