#include "PlayerState.h"
#include <cmath>
#include "Input.h"
#include "Camera.h"
#include "Easing.h"
#include "MathHelper.h"
#include "../Enemy/EnemyManager.h"
#include "Effect/EffectManager.h"

#include "UI/UIFader.h"
#include "System/SystemManager.h"
#include "PostProcess/PostProcess.h"

#include "Application.h"

#include "AudioManager.h"
#include "Item/Barrel.h"
#include "Item/MagicCircle.h"

#include "Projectile/HailBolt.h"
#include "Projectile/AquaSeeker.h"
#include "Projectile/RainClouds.h"

// ----- AddForceData -----
namespace PlayerState
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

// ----- AttackData -----
namespace PlayerState
{
    // ----- 初期化 -----
    void AttackData::Initialize(const float& startFrame, const float& endFrame)
    {
        attackStartFrame_ = startFrame;
        attackEndFrame_ = endFrame;
    }

    // ----- 更新 -----
    bool AttackData::Update(const float& animationFrame, const bool& flag)
    {
        // 既に攻撃が当たっている
        if (flag) return false;

        // 攻撃スタートフレームに達していないので終了
        if (animationFrame < attackStartFrame_) return false;

        // 攻撃エンドフレームを越しているのでここで終了
        if (animationFrame > attackEndFrame_) return false;

        return true;
    }
}

// ----- GamePadVibration -----
namespace PlayerState
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
    const bool GamePadVibration::Update(const float& animationFrame)
    {
        // 既に振動させている
        if (isVibraion_) return false;

        // 現在のアニメーションのフレームがスタートフレームまで達していない
        if (animationFrame < startFrame_) return false;

        // コントローラーを振動させる
        Input::Instance().GetGamePad().Vibration(time_, power_);
        isVibraion_ = true;

        return true;
    }
}

#pragma region ---------- SwordMan ----------
// ----- 待機 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void IdleState::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        // アニメーション設定
        SetAnimation();

        // 先行入力設定
        owner_->SetNextInputStartFrame();
        owner_->SetNextInputEndFrame();
        owner_->SetNextInputTransitionFrame();
    }

    // ----- 更新 -----
    void IdleState::Update(const float& elapsedTime)
    {
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y)
        {
            owner_->ChangeState(Player::STATE::ChargeAttack);
            return;
        }

        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y)
        {
            if (owner_->GetSwordSpirit() != 0.0f)
            {
                owner_->ChangeState(Player::STATE::Helmbreaker);
                return;
            }
        }

        // 先行入力判定
        if (CheckNextInput()) return;
    }

    // ----- 終了化 -----
    void IdleState::Finalize()
    {
    }

    // ----- ImGui用 -----
    void IdleState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {

            ImGui::TreePop();
        }
    }

    // ----- アニメーション設定 -----
    void IdleState::SetAnimation()
    {
        // 最初の一回だけこの処理が通る
        if (owner_->GetAnimationIndex() < 0)
        {
            owner_->PlayAnimation(Player::Animation::Idle, true);
            return;
        }

        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        if (animationIndex == Player::Animation::Counter)
        {
            owner_->SetTransitionTime(0.3f);
        }
        else if (animationIndex == Player::Animation::RollFront ||
            animationIndex == Player::Animation::RollBack ||
            animationIndex == Player::Animation::RollRight ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->SetTransitionTime(0.15f);
        }
        else
        {
            owner_->SetTransitionTime(0.15f);
        }
        owner_->PlayBlendAnimation(Player::Animation::Idle, true);
    }

    // ----- 先行入力判定 -----
    const bool IdleState::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

#pragma region ----- 先行入力受付 -----
        // 攻撃先行入力受付
        if (animationSeconds >= owner_->GetAttackInputStartFrame() &&
            animationSeconds <= owner_->GetAttackInputEndFrame())
        {
            if (owner_->IsComboAttack0KeyDown()) owner_->SetNextInput(Player::NextInput::ComboAttack0);
        }

        // カウンター先行入力受付
        if (animationSeconds >= owner_->GetCounterInputStartFrame() &&
            animationSeconds <= owner_->GetCounterInputEndFrame())
        {
            if (owner_->IsCounterStanceKey()) owner_->SetNextInput(Player::NextInput::Counter);
        }

        // 回避先行入力受付
        if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
            animationSeconds <= owner_->GetDodgeInputEndFrame())
        {
            if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
        }

#pragma endregion ----- 先行入力受付 -----

#pragma region ----- 遷移チェック -----
        // 回避遷移チェック
        if (owner_->GetNextInput() == Player::NextInput::Dodge)
        {
            if (animationSeconds >= owner_->GetDodgeTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Dodge);
                return true;
            }
        }
        // 攻撃遷移チェック
        else if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
        {
            if (animationSeconds >= owner_->GetAttackTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_0);
                return true;
            }
        }
        // カウンター遷移チェック
        else if (owner_->GetNextInput() == Player::NextInput::Counter)
        {
            if (animationSeconds >= owner_->GetCounterTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Counter);
                return true;
            }
        }

#pragma endregion ----- 遷移チェック -----

        // ガード入力判定
        if (owner_->IsGuardCounterButtonDown() && owner_->GetIsGuardGaugeDepleted() == false)
        {
            owner_->ChangeState(Player::STATE::GuardCounter);
            return true;
        }

        // アイテム使用入力判定
        if (owner_->IsItemKeyDown())
        {
            owner_->ChangeState(Player::STATE::PlacingBarre);
            return true;
        }

        // 移動入力判定
        if (animationSeconds >= owner_->GetMoveInputStartFrame())
        {
            // スティック入力があるか
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (aLx == 0.0f && aLy == 0.0f) return false;

            owner_->ChangeState(Player::STATE::Run);
            return true;
        }

        return false;
    }
}

// ----- 走り -----
namespace PlayerState
{
    // ----- 初期化 -----
    void RunState::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        // アニメーション設定
        SetAnimation();

        // 最大速度を設定
        owner_->SetMaxSpeed(5.0f);

        // 先行入力設定
        owner_->SetNextInputStartFrame();
        owner_->SetNextInputEndFrame();
        owner_->SetNextInputTransitionFrame();

        // 足音の効果音再生
        dashSENum_ = AudioManager::Instance().PlaySE(SE::Dash);

        // 変数初期化
        changeStateTimer_ = 0.0f;
    }

    // ----- 更新 -----
    void RunState::Update(const float& elapsedTime)
    {
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y)
        {
            if (owner_->GetSwordSpirit() != 0.0f)
            {
                owner_->ChangeState(Player::STATE::Helmbreaker);
                return;
            }
        }

        // アイテム使用入力判定
        if (owner_->IsItemKeyDown())
        {
            owner_->ChangeState(Player::STATE::PlacingBarre);
            return;
        }

        // 先行入力判定
        if (CheckNextInput()) return;

        // 旋回
        owner_->Turn(elapsedTime);

        // タイマー加算
        changeStateTimer_ += elapsedTime;

        // ダッシュ処理
        UpdateDash(elapsedTime);
    }
    
    // ----- 終了化 -----
    void RunState::Finalize()
    {
        owner_->SetMoveDirection({});
        owner_->SetVelocity({});

        owner_->SetIsDash(false);

        // 効果音停止
        AudioManager::Instance().StopSE(SE::Dash, dashSENum_);
    }

    // ----- ImGui用 -----
    void RunState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {

            ImGui::TreePop();
        }
    }

    // ----- アニメーション設定 -----
    void RunState::SetAnimation()
    {
        // 移動キャンセルの場合素早くアニメーションを切り替える
        if (owner_->GetIsMoveAttackCancel())
        {
            owner_->SetTransitionTime(0.15f);
            owner_->PlayBlendAnimation(Player::Animation::Run, true);
            owner_->SetIsMoveAttackCancel(false);
            return;
        }

        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        if (animationIndex == Player::Animation::RollFront ||
            animationIndex == Player::Animation::RollBack    ||
            animationIndex == Player::Animation::RollRight   ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->SetTransitionTime(0.2f);
        }
        else if (animationIndex == Player::Animation::Attack0_0 ||
            animationIndex == Player::Animation::Attack0_1 ||
            animationIndex == Player::Animation::RunAttack1)
        {
            owner_->SetTransitionTime(0.3f);
        }
        else if (animationIndex == Player::Animation::Attack0_2)
        {
            owner_->SetTransitionTime(0.3f);
        }
        else
        {
            owner_->SetTransitionTime(0.15f);
        }

        owner_->PlayBlendAnimation(Player::Animation::Run, true);
    }

    // ----- 先行入力判定 -----
    const bool RunState::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

#pragma region ----- 先行入力受付 -----
        // 攻撃先行入力受付
        if (animationSeconds >= owner_->GetAttackInputStartFrame() &&
            animationSeconds <= owner_->GetAttackInputEndFrame())
        {
            if (owner_->IsComboAttack0KeyDown()) owner_->SetNextInput(Player::NextInput::ComboAttack0);
        }

        // カウンター先行入力受付
        if (animationSeconds >= owner_->GetCounterInputStartFrame() &&
            animationSeconds <= owner_->GetCounterInputEndFrame())
        {
            if (owner_->IsCounterStanceKey()) owner_->SetNextInput(Player::NextInput::Counter);
        }

        // 回避先行入力受付
        if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
            animationSeconds <= owner_->GetDodgeInputEndFrame())
        {
            if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
        }

#pragma endregion ----- 先行入力受付 -----

#pragma region ----- 遷移チェック -----
        // 回避遷移チェック
        if (owner_->GetNextInput() == Player::NextInput::Dodge)
        {
            if (animationSeconds >= owner_->GetDodgeTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Dodge);
                return true;
            }
        }
        // 攻撃遷移チェック
        else if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
        {
            if (animationSeconds >= owner_->GetAttackTransitionFrame())
            {
                // ダッシュしているときは走り攻撃に遷移する
                if (owner_->GetIsDash()) owner_->ChangeState(Player::STATE::RunAttack);
                // 通常の走りの場合はコンボ攻撃０に遷移する
                else owner_->ChangeState(Player::STATE::ComboAttack0_0);
                return true;
            }
        }
        // カウンター遷移チェック
        else if (owner_->GetNextInput() == Player::NextInput::Counter)
        {
            if (animationSeconds >= owner_->GetCounterTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Counter);
                return true;
            }
        }

#pragma endregion ----- 遷移チェック -----

        // ガード入力判定
        if (owner_->IsGuardCounterButtonDown() && owner_->GetIsGuardGaugeDepleted() == false)
        {
            owner_->ChangeState(Player::STATE::GuardCounter);
            return true;
        }

        // 移動入力判定
        if (animationSeconds >= owner_->GetMoveInputStartFrame())
        {
            // 走り->待機 の遷移制御
            if (changeStateTimer_ <= 0.2f) return false;

            // スティック入力があるか
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (aLx == 0.0f && aLy == 0.0f)
            {
                owner_->ChangeState(Player::STATE::Idle);
                return true;
            }
        }

        return false;
    }

    // ----- ダッシュの処理 -----
    void RunState::UpdateDash(const float& elapsedTime)
    {
        const bool isDashKey = owner_->IsDashKey();

        // スタミナが底をついた場合速度を遅くする
        if(owner_->GetIsStaminaDepleted())
        {
            owner_->SetAnimationSpeed(0.8f);

            // 最大速度を設定
            owner_->SetMaxSpeed(3.0f);

            owner_->UseDashStamina(elapsedTime);

            // ダッシュしているか設定
            owner_->SetIsDash(isDashKey);

            return;
        }

        // ダッシュの処理
        if (isDashKey)
        {
            owner_->SetAnimationSpeed(owner_->GetDashAnimationSpeed());

            // 最大速度を設定
            owner_->SetMaxSpeed(owner_->GetDashSpeed());

            owner_->SetIsDash(true);

            owner_->UseDashStamina(elapsedTime);
        }
        else
        {
            owner_->SetAnimationSpeed(1.0f);

            // 最大速度を設定
            owner_->SetMaxSpeed(5.0f);

            owner_->SetIsDash(false);
        }

    }
}

// ----- ガードカウンター -----
namespace PlayerState
{
    // ----- 初期化 ----
    void GuardCounterState::Initialize()
    {
        // アニメーション設定
        owner_->PlayUpperLowerBodyAnimation(static_cast<int>(Player::Animation::BlockLoop), true);

        DirectX::XMFLOAT3 position = owner_->GetJointPosition("pelvis");
        guardEffect_ = EffectManager::Instance().GetEffect("Guard")->Play(position, guardEffectStartSize_, 1.0f);

        owner_->SetIsGuardCounterStance(true);
        owner_->SetIsGuardCounterSuccessful(false); // リセットする

        // 最大速度を設定
        owner_->SetMaxSpeed(5.0f);

        // 変数初期化
        gamePadVibration_.Initialize(0.0f, 0.2f, 0.5f);
        guardEffectLerpTimer_       = 0.0f;
    }

    // ----- 更新 -----
    void GuardCounterState::Update(const float& elapsedTime)
    {
        // カウンターの入力をチェックする
        if (owner_->IsCounterStanceKey())
        {
            owner_->PlayAnimation(Player::Animation::Counter, false, 1.0f, 0.15f);
            owner_->ChangeState(Player::STATE::Counter);
            return;
        }

        // ガードの入力がなくなってるかチェックする
        if (owner_->GetIsBlendUpperLowerBodyAnimation() == false && owner_->IsGuardCounterButton() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

        // ガードゲージがなくなったら待機に遷移する
        if (owner_->GetGuardGauge() <= 0.0f)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

        // ガードゲージを消費
        owner_->UseGuardGauge(elapsedTime);

        // 移動入力処理
        const float aLx = Input::Instance().GetGamePad().GetAxisLX();
        const float aLy = Input::Instance().GetGamePad().GetAxisLY();
        if (fabsf(aLx) != 0.0f || fabsf(aLy) != 0.0f)
        {
            owner_->ChangeLowerBodyAnimation(static_cast<int>(Player::Animation::Run));

            // 旋回
            owner_->Turn(elapsedTime);
        }
        else
        {
            owner_->ChangeLowerBodyAnimation(static_cast<int>(Player::Animation::Idle));

            // 移動&回転処理リセット
            owner_->SetMoveDirection({});
            owner_->SetVelocity({});
        }

        // エフェクト更新
        UpdateEffect(elapsedTime);

        // ガードが成功した
        if (owner_->GetIsGuardCounterSuccessful())
        {
            // 自動的にカウンターに遷移する
            if (owner_->GetIsAutoCounterModeEnabled())
            {
                // TODO:仮で作ってる
                gamePadVibration_.Update(owner_->GetAnimationSeconds());

                owner_->ChangeState(Player::STATE::GuardCounterAttack);
                return;
            }
            else
            {
                // ガードした方向に向かせる
                const DirectX::XMFLOAT3 knockBackDirection_float3 = owner_->GetKnockBackDirection();
                const DirectX::XMFLOAT3 playerFront_float3 = owner_->GetTransform()->CalcForward();
                const DirectX::XMFLOAT2 knockBackDirection = XMFloat2Normalize({ knockBackDirection_float3.x, knockBackDirection_float3.z });
                const DirectX::XMFLOAT2 playerFront = XMFloat2Normalize({ playerFront_float3.x, playerFront_float3.z });
                
                float angle = acosf(std::clamp(XMFloat2Dot(knockBackDirection, playerFront), -1.0f, 1.0f));

                float cross = XMFloat2Cross(knockBackDirection, playerFront);
                if (cross > 0)  owner_->GetTransform()->AddRotationY(-angle);
                else            owner_->GetTransform()->AddRotationY(angle);

                // ガードゲージを消費 (ガードゲージがまだあればtrue)
                if (owner_->UseGuardGaugeOnBlock())
                {// ガードできた                   

                    // 無敵状態にする
                    owner_->SetIsInvincible(true);

                    // ガードブロックしたステートへ遷移
                    owner_->ChangeState(Player::STATE::GuardBlock);
                    return;
                }
                else
                {// ガードが破壊された
                    // ガードゲージがなくなったフラグを立てる
                    owner_->SetIsGuardGaugeDepleted(true);

                    // 無敵状態にする
                    owner_->SetIsInvincible(true);

                    owner_->SetIsGuardCounterSuccessful(false);
                    
                    // ガード破壊されたステートへ遷移
                    owner_->ChangeState(Player::STATE::GuardBroken);
                    return;
                }
            }
        }


    }

    // ----- 終了化 -----
    void GuardCounterState::Finalize()
    {
        // エフェクトを停止させる
        EffectManager::Instance().GetEffect("Guard")->Stop(guardEffect_);

        // 移動&回転処理リセット
        owner_->SetMoveDirection({});
        owner_->SetVelocity({});

        owner_->SetIsGuardCounterStance(false);

        owner_->SetIsGuardCounterSuccessful(false);
    }

    // ----- ImGui用 -----
    void GuardCounterState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragFloat("GuardEffectStartSize", &guardEffectStartSize_, 0.1f, 1.0f, 6.0f);
            ImGui::DragFloat("GuardEffectEndSize", &guardEffectEndSize_, 0.1f, 1.0f, 6.0f);
            ImGui::DragFloat("LerpTimer", &guardEffectLerpTimer_);
            ImGui::DragFloat("LerpSpeed", &guardEffectLerpSpeed_);

            ImGui::TreePop();
        }
    }

    // ----- エフェクト更新 -----
    void GuardCounterState::UpdateEffect(const float& elapsedTime)
    {
        // ガードが終わっていたら更新しない
        //if (isGuardLoopAnimationEnd_) return;

        Effect* guardEffect = EffectManager::Instance().GetEffect("Guard");
        DirectX::XMFLOAT3 pelvisPosition = owner_->GetJointPosition("pelvis");

        guardEffect->SetPosition(guardEffect_, pelvisPosition);

        // -----------------------------------
        //  　エフェクトのサイズを更新する
        // 　ついでに判定用の変数も更新しておく
        // -----------------------------------
        guardEffectLerpTimer_ += guardEffectLerpSpeed_ * elapsedTime;
        guardEffectLerpTimer_ = std::min(guardEffectLerpTimer_, 1.0f);
        const float guardEffectSize = XMFloatLerp(guardEffectStartSize_, guardEffectEndSize_, guardEffectLerpTimer_);
        guardEffect->SetScale(guardEffect_, guardEffectSize);

        float guardCounterRadius = XMFloatLerp(guardCounterStartRadius_, guardCounterEndRadius_, guardEffectLerpTimer_);
        owner_->SetGuardCounterRadius(guardCounterRadius);
    }

    // ----- ガードが成功したかチェック -----
    const bool GuardCounterState::CheckGuardCounterSuccessful()
    {
        return false;
    }

}

// ----- ガードカウンター攻撃 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void GuardCounterAttackState::Initialize()
    {
        // アニメーション設定    
        owner_->PlayBlendAnimation(Player::Animation::CounterAttack0, false, 1.0f, 0.5f);
        owner_->SetTransitionTime(0.1f);

        owner_->SetIsDrawSwordTrail(true);
    }

    // ----- 更新 -----
    void GuardCounterAttackState::Update(const float& elapsedTime)
    {
        // RootMotionの設定
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotionを使用する
            owner_->SetUseRootMotion(true);
        }

        // 剣の軌跡の設定
        if (owner_->GetAnimationSeconds() > 1.0f && owner_->GetIsDrawSwordTrail())
        {
            owner_->SetIsDrawSwordTrail(false);
        }

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void GuardCounterAttackState::Finalize()
    {
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui用 -----
    void GuardCounterAttackState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {

            ImGui::TreePop();
        }
    }
}

// ----- ガードブロック -----
namespace PlayerState
{
    // ----- 初期化 -----
    void GuardBlockState::Initialize()
    {
        // アニメーション再生
        owner_->PlayBlendAnimation(Player::Animation::BlockHit, false);

        // ルートモーションを使用
        owner_->SetUseRootMotion(true);

        // ルートの移動値を増やす
        owner_->SetRootMotionValue(3.0f);

        // コントローラ振動
        Input::Instance().GetGamePad().Vibration(0.2f, 0.5f);
    }

    // ----- 更新 -----
    void GuardBlockState::Update(const float& elapsedTime)
    {
        // カウンターに派生する入力
        if(owner_->IsGuardCounterButtonDown())
        {
            owner_->ChangeState(Player::STATE::GuardCounterAttack);
            return;
        }
        // 回避入力
        if (owner_->IsDodgeKeyDown())
        {
            owner_->ChangeState(Player::STATE::Dodge);
            return;
        }
        // 移動入力判定
        if (owner_->GetAnimationSeconds() > moveInputFrame_)
        {
            const float aLX = Input::Instance().GetGamePad().GetAxisLX();
            const float aLY = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLX) >= moveInputThreshold_ || fabsf(aLY) >= moveInputThreshold_)
            {
                owner_->ChangeState(Player::STATE::Run);
                return;
            }
        }

        if (owner_->IsPlayAnimation() == false)
        {
            // 無敵状態解除
            owner_->SetIsInvincible(false);

            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void GuardBlockState::Finalize()
    {
        // 無敵状態を解除する
        owner_->SetIsInvincible(false);

        // ルートモーションを使用しない
        owner_->SetUseRootMotion(false);

        // ルートの移動値をリセット
        owner_->SetRootMotionValue(1.0f);
    }

    // ----- ImGui用 -----
    void GuardBlockState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragFloat("MoveInputFrame", &moveInputFrame_, 0.01f);
            ImGui::DragFloat("MoveInputThresold", &moveInputThreshold_, 0.01f, 0.0f, 1.0f);

            ImGui::TreePop();
        }
    }
}

// ----- ガード破壊された -----
namespace PlayerState
{
    // ----- 初期化 -----
    void GuardBrokenState::Initialize()
    {
        // アニメーション再生
        owner_->PlayBlendAnimation(Player::Animation::BlockBreak, false);        

        // ルートモーションを使用
        owner_->SetUseRootMotion(true);

        // ルートの移動値を増やす
        owner_->SetRootMotionValue(3.0f);
    }

    // ----- 更新 -----
    void GuardBrokenState::Update(const float& elapsedTime)
    {
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void GuardBrokenState::Finalize()
    {
        // 無敵状態を解除する
        owner_->SetIsInvincible(false);

        // ルートモーションを使用しない
        owner_->SetUseRootMotion(false);

        // ルートの移動値をリセット
        owner_->SetRootMotionValue(1.0f);
    }

    // ----- ImGui用 -----
    void GuardBrokenState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {

            ImGui::TreePop();
        }
    }
}

// ----- 弱怯み -----
namespace PlayerState
{
    // ----- 初期化 -----
    void LightFlinchState::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        owner_->PlayBlendAnimation(Player::Animation::DownStart, false);
    }

    // ----- 更新 -----
    void LightFlinchState::Update(const float& elapsedTime)
    {
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void LightFlinchState::Finalize()
    {
    }
    void LightFlinchState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {

            ImGui::TreePop();
        }
    }
}

// ----- 怯み -----
namespace PlayerState
{
    // ----- 初期化 -----
    void FlinchState::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::DownStart, false, 2.0f);

        state_ = 0;
    }

    // ----- 更新 -----
    void FlinchState::Update(const float& elapsedTime)
    {
        switch (state_)
        {
        case 0:
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Player::Animation::DownLoop, false);
                state_ = 1;
            }

            break;
        case 1:
            

            if(owner_->IsPlayAnimation() == false)
            //if (EnemyManager::Instance().GetEnemy(0)->GetActiveNodeName() != "Roar")
            {
                owner_->PlayBlendAnimation(Player::Animation::DownEnd, false);
                state_ = 2;
            }

            break;
        case 2:
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->ChangeState(Player::STATE::Idle);
            }

            break;
        }

    }

    // ----- 終了化 -----
    void FlinchState::Finalize()
    {
    }
    void FlinchState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {

            ImGui::TreePop();
        }
    }
}

// ----- ダメージ -----
namespace PlayerState
{
    // ----- 初期化 -----
    void DamageState::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        // アニメーション再生 
        owner_->PlayAnimation(Player::Animation::Damage, false, 1.2f);

        // 無敵状態にする
        owner_->SetIsInvincible(true);

        // 変数初期化
        addForceData_.Initialize(0.1f, 0.3f, 0.5f);
        isFirstAnimation_ = true;

        DirectX::XMFLOAT3 ownerPos = owner_->GetTransform()->GetPosition();
        DirectX::XMFLOAT3 dragonPos = EnemyManager::Instance().GetEnemy(0)->GetTransform()->GetPosition();
        ownerPos.y = 0;
        dragonPos.y = 0;
        addForceDirection_ = XMFloat3Normalize(ownerPos - dragonPos);

        // 回転
        Turn();

        // ビネット設定
        SetVignette();

        isCameraShakeActive_ = false;
    }

    // ----- 更新 -----
    void DamageState::Update(const float& elapsedTime)
    {
        // アニメーションの速度設定
        SetAnimationSpeed();

        // カメラシェイクを少し入れる
        if (isCameraShakeActive_ == false)
        {
            if (owner_->GetAnimationSeconds() > 0.15f)
            {
                Camera::Instance().ScreenVibrate(0.1f, 0.3f);
                isCameraShakeActive_ = true;
            }
        }

        // ビネット更新
        vignetteTimer_ += vignetteFadeOutSpeed_ * elapsedTime;
        vignetteTimer_ = std::min(vignetteTimer_, 1.0f);
        const float maxIntensity = isHighDamage_ ? highDamageMaxIntensity_ : normalDamageMaxIntensity_;
        const float intensity = XMFloatLerp(maxIntensity, 0.0f, vignetteTimer_);
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteIntensity_ = intensity;

        // 移動値
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(addForceDirection_, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // 吹き飛ばされアニメーション処理
        if (owner_->GetAnimationSeconds() > 1.2f && isFirstAnimation_ == true)
        {
            // 入力があれば倒れてる状態を終了する
            if (owner_->IsGetUpKeyDown())
            {
                owner_->PlayBlendAnimation(Player::Animation::GetUp, false);
                owner_->SetTransitionTime(0.1f);
                isFirstAnimation_ = false;
            }
        }

        // アニメーション終了
        if (owner_->IsPlayAnimation() == false && isFirstAnimation_ == true)
        {
            owner_->PlayBlendAnimation(Player::Animation::GetUp, false);
            owner_->SetTransitionTime(0.1f);
            isFirstAnimation_ = false;
        }
        //else if(owner_->IsPlayAnimation() == false)
        else if (owner_->GetAnimationSeconds() > 1.8f && isFirstAnimation_ == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

        if (isFirstAnimation_ == false)
        {
            if (owner_->GetAnimationSeconds() > 1.25f)
            {
                if (owner_->IsDodgeKeyDown())
                {
                    owner_->ChangeState(Player::STATE::Dodge);
                    return;
                }
            }
        }

    }

    // ----- 終了化 -----
    void DamageState::Finalize()
    {
        // 無敵状態を解除する
        owner_->SetIsInvincible(false);

        // ビネット使用フラグを下げる
        PostProcess::Instance().SetUseVignette(false);
    }

    // ----- ImGui用 -----
    void DamageState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- Vignette ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("Timer", &vignetteTimer_, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("FadeOutSpeed", &vignetteFadeOutSpeed_, 0.01f, 0.0f, 5.0f);

                ImGui::ColorEdit4("NormalDmageColor", &normalDamageColor_.x);
                ImGui::ColorEdit4("HighDmageColor", &highDamageColor_.x);

                ImGui::DragFloat("NormalIntensity", &normalDamageMaxIntensity_, 0.01f, 0.0f, 2.0f);
                ImGui::DragFloat("HighIntensity", &highDamageMaxIntensity_, 0.01f, 0.0f, 2.0f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- ビネット設定 -----
    void DamageState::SetVignette()
    {
        // HighDamageかの判定
        isHighDamage_ = (EnemyManager::Instance().GetEnemy(0)->GetAnimationIndex() == static_cast<int>(Enemy::DragonAnimation::Nova1));

        PostProcess::Instance().SetUseVignette();
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteCenter_ = { 0.5f, 0.5f };
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteSmoothness_ = 2.2f;
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteIntensity_ = isHighDamage_ ? highDamageMaxIntensity_ : normalDamageMaxIntensity_;
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteColor_ = isHighDamage_ ? highDamageColor_ : normalDamageColor_;

        vignetteTimer_ = 0.0f;
    }

    // ----- アニメーションの速度設定 -----
    void DamageState::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // 一つ目のアニメーション ( 吹き飛ばされ )
        if (isFirstAnimation_)
        {
            if (animationSeconds > 1.2f)
            {// 起き上がるまでの時間を延ばす
                owner_->SetAnimationSpeed(0.4f);
                //owner_->SetAnimationSpeed(0.1f);
            }
            else if (animationSeconds > 1.0f)
            {
                owner_->SetAnimationSpeed(1.0f);
            }
        }
        // 二つ目のアニメーション ( 起き上がり )
        else
        {
            if (animationSeconds > 1.0f)
            {
                owner_->SetAnimationSpeed(1.5f);
            }
        }
    }

    // ----- 回転処理 -----
    void DamageState::Turn()
    {
        DirectX::XMFLOAT2 ownerFront = XMFloat2Normalize({ owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z });
        DirectX::XMFLOAT2 addForceDirection = XMFloat2Normalize(DirectX::XMFLOAT2(addForceDirection_.x, addForceDirection_.z) * -1.0f);

        float cross = XMFloat2Cross(addForceDirection, ownerFront);
        float dot = std::clamp(XMFloat2Dot(addForceDirection, ownerFront), -1.0f, 1.0f);
        float angle = acosf(dot);

        if (cross > 0)
        {
            owner_->GetTransform()->SetRotationY(-angle);
        }
        else
        {
            owner_->GetTransform()->SetRotationY(angle);
        }
    }
}

// ----- 死亡 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void DeathState::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        owner_->PlayBlendAnimation(Player::Animation::Death, false, 1.0f, 0.2f);
        owner_->SetTransitionTime(0.3f);

        // 死亡したので無敵状態にする
        owner_->SetIsInvincible(true);

        // 死亡カメラを使用する
        Camera::Instance().SetUsePlayerDeathCmaera();

        // 変数初期化
        deathTimer_ = 0.0f;
        isCreateFadeUi_ = false;
    }

    // ----- 更新 -----
    void DeathState::Update(const float& elapsedTime)
    {
        //return;
        deathTimer_ += elapsedTime;

        if (deathTimer_ > 2.2f)
        {
            if (isCreateFadeUi_ == false)
            {
                UIFader* uiFader = new UIFader();

                isCreateFadeUi_ = true;
            }
        }

        if (deathTimer_ > 3.0f)
        {
            Camera::Instance().SetUsePlayerDeathCmaera(false);
            Camera::Instance().Initialize();
            owner_->SetHealth(owner_->GetMaxHealth());
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void DeathState::Finalize()
    {
        owner_->SetIsInvincible(false);
    }

    void DeathState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {

            ImGui::TreePop();
        }
    }
}

// ----- 回避 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void DodgeState::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        // アニメーション設定
        SetAnimation();

        // 無敵状態にする
        owner_->SetIsInvincible(true);

        // スタミナ消費
        owner_->UseDodgeStamina();

        // ジャスト回避判定有効化
        owner_->SetIsJustDodgeCheckEnabled(true);

        // 変数初期化
        isRotating_ = false;
        isFirstTime_ = false;
    }

    // ----- 更新 -----
    void DodgeState::Update(const float& elapsedTime)
    {
        currentAnimationFrame_ = owner_->GetAnimationSeconds();

        // RootMotionの設定
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotionを使用する
            owner_->SetUseRootMotion(true);
            owner_->SetRootMotionValue(rootMotionMoveValue_);
        }

        // ジャスト回避判定
        if (currentAnimationFrame_ < justDodgeFrame_)
        {
            // ジャスト回避が成功したら遷移する
            if (owner_->GetIsJustDodgeSuccessful())
            {
                // フラグリセット
                owner_->SetIsJustDodgeCheckEnabled(false);
                owner_->SetIsJustDodgeSuccessful(false);

                owner_->ChangeState(Player::STATE::JustDodge);
                return;
            }
        }
        else
        {
            if (owner_->GetIsJustDodgeCheckEnabled())
                owner_->SetIsJustDodgeCheckEnabled(false);
        }

        // 先行入力処理
        if (CheckNextInput()) return;

        Turn(elapsedTime);

        // アニメーションの速度設定
        SetAnimationSpeed();

        // 無敵判定更新
        if (owner_->GetIsInvincible() && currentAnimationFrame_ > invincibleFrame_)
        {
            owner_->SetIsInvincible(false);
        }

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void DodgeState::Finalize()
    {
        // ルートモーションフラグリセット
        owner_->SetUseRootMotion(false);

        // 変数をリセットしておく
        isFirstTime_ = true;
    }

    void DodgeState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragFloat("AnimationFrame", &currentAnimationFrame_, 0.01f, 0.0f, 1.0f);
            if (ImGui::TreeNodeEx("---------- JustDodge ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("JustDodgeFrame", &justDodgeFrame_, 0.01f, 0.0f, 3.0f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Invincible ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("InvincibleFrame", &invincibleFrame_, 0.01f, 0.0f, 3.0f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Movement ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("RootMotionMoveValue", &rootMotionMoveValue_, 0.1f, 0.0f, 10.0f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- 回転処理 -----
    void DodgeState::Turn(const float& elapsedTime)
    {
        // 回転量がないためここで終了
        if (isRotating_ == false) return;
        // 入力値がないので回転する必要がない。ここで終了
        if (isInputStick_ == false) return;

        DirectX::XMFLOAT2 playerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
        playerForward = XMFloat2Normalize(playerForward);

        float forwardCross = XMFloat2Cross(inputDirection_, playerForward);

        float dot = std::clamp(XMFloat2Dot(inputDirection_, playerForward), -1.0f, 1.0f);
        float angle = acosf(dot);

        if (angle < DirectX::XMConvertToRadians(1))
        {
            isRotating_ = false;
            return;
        }

        const float speed = owner_->GetRotateSpeed() * elapsedTime;
        float rotateY = angle * speed;

        if (forwardCross > 0)
        {
            owner_->GetTransform()->AddRotationY(-rotateY);
        }
        else
        {
            owner_->GetTransform()->AddRotationY(rotateY);
        }
    }

    // ----- このステートをリセット(初期化)する -----
    void DodgeState::ResetState()
    {
        // ------------------------------
        //  回避を連続して出している場合
        // ------------------------------
        if (isFirstTime_ == false)
        {
            // 回転処理を行う
            isRotating_ = true;
        }

        // アニメーション設定
        SetAnimation();

        // フラグをリセットする
        owner_->ResetFlags();

        // スタミナ消費
        owner_->UseDodgeStamina();

        // ルートモーションリセット
        owner_->SetUseRootMotion(false);
    }

    // ----- 先行入力処理 -----
    const bool DodgeState::CheckNextInput()
    {
#if 1
        const float nextInputStartFrame = 0.5f; // 先行入力開始フレーム

        if (currentAnimationFrame_ > nextInputStartFrame)
        {
            if (owner_->IsComboAttack0KeyDown())
            {
                owner_->SetNextInput(Player::NextInput::ComboAttack0);
            }

            // 回避
            if (owner_->IsDodgeKeyDown() &&
                owner_->GetAnimationIndex() != static_cast<int>(Player::Animation::RollBack))
            {
                owner_->SetNextInput(Player::NextInput::Dodge);

                GamePad& gamePad = Input::Instance().GetGamePad();
                const float aLx = gamePad.GetAxisLX();
                const float aLy = gamePad.GetAxisLY();
                if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
                {
                    const DirectX::XMFLOAT3 cameraFront = Camera::Instance().CalcForward();
                    const DirectX::XMFLOAT3 cameraRight = Camera::Instance().CalcRight();
                    inputDirection_ =
                    {
                        aLy * cameraFront.x + aLx * cameraRight.x,
                        aLy * cameraFront.z + aLx * cameraRight.z,
                    };
                    inputDirection_ = XMFloat2Normalize(inputDirection_);

                    isInputStick_ = true;
                }
                else
                {
                    isInputStick_ = false;
                }
            }
        }

#else 


        // 先行入力受付
#pragma region 先行入力受付
        switch (static_cast<Player::Animation>(owner_->GetAnimationIndex()))
        {
        case Player::Animation::RollForward:// 前
        {
            const float nextInputStartFrame = 0.5f; // 先行入力開始フレーム
            if (animationSeconds > nextInputStartFrame)
            {
                // コンボ攻撃
                if (owner_->IsComboAttack0KeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::ComboAttack0);
                }
                // 回避
                if (owner_->IsAvoidanceKeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::Avoidance);
                }
            }
        }
        break;
        case Player::Animation::RollBack:// 後ろ
        {
            const float nextInputStartFrame = 0.5f;
            if (animationSeconds > nextInputStartFrame)
            {
                // コンボ攻撃
                if (owner_->IsComboAttack0KeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::ComboAttack0);
                }
            }
        }
        break;
        case Player::Animation::RollRight:// 右
        {
            const float nextInputStartFrame = 0.5f;
            if (animationSeconds > nextInputStartFrame)
            {
                // コンボ攻撃
                if (owner_->IsComboAttack0KeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::ComboAttack0);
                }
                // 回避
                if (owner_->IsAvoidanceKeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::Avoidance);
                }
            }
        }
        break;
        case Player::Animation::RollLeft:
        {
            const float nextInputStartFrame = 0.5f;
            if (animationSeconds > nextInputStartFrame)
            {
                // コンボ攻撃
                if (owner_->IsComboAttack0KeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::ComboAttack0);
                }
                // 回避
                if (owner_->IsAvoidanceKeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::Avoidance);
                }
            }
        }
        break;
        }
#pragma endregion 先行入力受付
#endif

        // 先行入力によるステート変更処理
#pragma region 先行入力によるステート変更処理
        switch (static_cast<Player::Animation>(owner_->GetAnimationIndex()))
        {
        case Player::Animation::RollFront:// 前
        {
            // 回避の先行入力がある場合
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                const float avoidanceFrame = 0.92f; // 回避に遷移できるフレーム
                if (currentAnimationFrame_ > avoidanceFrame)
                {
                    //回避は現在と同じステートなので、初期化を呼ぶ
                    ResetState();
                    return true;
                }
            }
            // コンボ攻撃0の場合
            else if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                const float comboAttack0Frame = 0.9f; // コンボ攻撃0に遷移できるフレーム
                if (currentAnimationFrame_ > comboAttack0Frame)
                {
                    owner_->ChangeState(Player::STATE::ComboAttack0_0);
                    return true;
                }
            }
            // それ以外
            else
            {
                const float moveFrame = 0.8f;// 移動に遷移できるフレーム
                if (currentAnimationFrame_ > moveFrame)
                {
                    // 移動値があるか判定
                    const float aLx = Input::Instance().GetGamePad().GetAxisLX();
                    const float aLy = Input::Instance().GetGamePad().GetAxisLY();
                    if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
                    {
                        owner_->ChangeState(Player::STATE::Run);
                        return true;
                    }
                }
            }
        }
        break;
        case Player::Animation::RollBack:// 後ろ
        {
            // コンボ攻撃0の場合
            if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                const float comboAttack0Frame = 0.85f; // コンボ攻撃0に遷移できるフレーム
                if (currentAnimationFrame_ > comboAttack0Frame)
                {
                    owner_->ChangeState(Player::STATE::ComboAttack0_0);
                    return true;
                }
            }
            // それ以外
            else
            {
                const float moveFrame = 1.0f;// 移動に遷移できるフレーム
                if (currentAnimationFrame_ > moveFrame)
                {
                    // 移動値があるか判定
                    const float aLx = Input::Instance().GetGamePad().GetAxisLX();
                    const float aLy = Input::Instance().GetGamePad().GetAxisLY();
                    if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
                    {
                        owner_->ChangeState(Player::STATE::Run);
                        return true;
                    }
                }
            }
        }
        break;
        case Player::Animation::RollRight:
        {
            // 回避の先行入力がある場合
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                const float avoidanceFrame = 0.92f; // 回避に遷移できるフレーム
                if (currentAnimationFrame_ > avoidanceFrame)
                {
                    //回避は現在と同じステートなので、初期化を呼ぶ
                    ResetState();
                    return true;
                }
            }
            // コンボ攻撃0の場合
            else if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                const float comboAttack0Frame = 0.9f; // コンボ攻撃0に遷移できるフレーム
                if (currentAnimationFrame_ > comboAttack0Frame)
                {
                    owner_->ChangeState(Player::STATE::ComboAttack0_0);
                    return true;
                }
            }
            // それ以外
            else
            {
                const float moveFrame = 0.8f;// 移動に遷移できるフレーム
                if (currentAnimationFrame_ > moveFrame)
                {
                    // 移動値があるか判定
                    const float aLx = Input::Instance().GetGamePad().GetAxisLX();
                    const float aLy = Input::Instance().GetGamePad().GetAxisLY();
                    if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
                    {
                        owner_->ChangeState(Player::STATE::Run);
                        return true;
                    }
                }
            }
        }
        break;
        case Player::Animation::RollLeft:
        {
            // 回避の先行入力がある場合
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                const float avoidanceFrame = 0.92f; // 回避に遷移できるフレーム
                if (currentAnimationFrame_ > avoidanceFrame)
                {
                    //回避は現在と同じステートなので、初期化を呼ぶ
                    ResetState();
                    return true;
                }
            }
            // コンボ攻撃0の場合
            else if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                const float comboAttack0Frame = 0.9f; // コンボ攻撃0に遷移できるフレーム
                if (currentAnimationFrame_ > comboAttack0Frame)
                {
                    owner_->ChangeState(Player::STATE::ComboAttack0_0);
                    return true;
                }
            }
            // それ以外
            else
            {
                const float moveFrame = 0.8f;// 移動に遷移できるフレーム
                if (currentAnimationFrame_ > moveFrame)
                {
                    // 移動値があるか判定
                    const float aLx = Input::Instance().GetGamePad().GetAxisLX();
                    const float aLy = Input::Instance().GetGamePad().GetAxisLY();
                    if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
                    {
                        owner_->ChangeState(Player::STATE::Run);
                        return true;
                    }
                }
            }
        }
        break;
        }
#pragma endregion 先行入力によるステート変更処理

        return false;
    }

    // ----- アニメーションの速度設定 -----
    void DodgeState::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        switch (static_cast<Player::Animation>(owner_->GetAnimationIndex()))
        {
        case Player::Animation::RollFront:// 前
            if (animationSeconds < 0.6f)
            {
                owner_->SetAnimationSpeed(1.4f);
            }
            else
            {
                owner_->SetAnimationSpeed(1.0f);
            }

            break;
        case Player::Animation::RollBack:// 後ろ
            if (animationSeconds < 0.6f)
            {
                owner_->SetAnimationSpeed(1.4f);
            }
            else
            {
                owner_->SetAnimationSpeed(1.0f);
            }

            break;
        case Player::Animation::RollRight:// 右
            if (animationSeconds < 0.6f)
            {
                owner_->SetAnimationSpeed(1.4f);
            }
            else
            {
                owner_->SetAnimationSpeed(1.0f);
            }

            break;
        case Player::Animation::RollLeft:// 左
            if (animationSeconds < 0.6f)
            {
                owner_->SetAnimationSpeed(1.4f);
            }
            else
            {
                owner_->SetAnimationSpeed(1.0f);
            }

            break;
        }
    }

    // ----- アニメーション設定 -----
    void DodgeState::SetAnimation()
    {
        // --------------------------------------------------
        //  回避を連続して出している場合
        // --------------------------------------------------
        if (isFirstTime_ == false)
        {
            // 前方向のアニメーションを設定する
            owner_->PlayBlendAnimation(Player::Animation::RollFront, false, 1.0f, animationStartFrame_);
            owner_->SetTransitionTime(0.05f);
            return;
        }

        float transitionTime = 0.05f;
        // 元のアニメーションに応じてブレンドの時間を設定する
        const Player::Animation currentAnimationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());
        if (currentAnimationIndex == Player::Animation::Attack0_0)          transitionTime = 0.1f;
        else if (currentAnimationIndex == Player::Animation::RunAttack1)    transitionTime = 0.1f;
        else if (currentAnimationIndex == Player::Animation::GetUp)         transitionTime = 0.2f;
        owner_->SetTransitionTime(transitionTime);

        // ------------------------------------------------------------
        // プレイヤーの姿勢に合わせてアニメーションの方向を設定する
        // ------------------------------------------------------------
        const float animationSpeed = 1.0f;
        const float aLx = Input::Instance().GetGamePad().GetAxisLX();
        const float aLy = Input::Instance().GetGamePad().GetAxisLY();
        Player::Animation animationindex = Player::Animation::DodgeFront;
        // 入力値がある場合
        if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
        {
            // カメラから見たスティックの入力値を算出する
            DirectX::XMFLOAT2 cameraInput = Camera::Instance().ConvertTo2DVectorFromCamera(DirectX::XMFLOAT2(aLx, aLy));
            DirectX::XMFLOAT2 ownerFront = XMFloat2Normalize({ owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z });

            // 内積で角度を算出
            float dot = std::clamp(XMFloat2Dot(cameraInput, ownerFront), -1.0f, 1.0f);
            float angle = acosf(dot);

            // 左右判定
            float cross = XMFloat2Cross(cameraInput, ownerFront);

            // 回転角が９０度よりも小さければ 前,右,左 の三択
            if (angle < DirectX::XM_PIDIV2)
            {
                // 回転角が４５度よりも小さければ 前方向
                if (angle < DirectX::XM_PIDIV4)
                {
                    animationindex = Player::Animation::RollFront;
                }
                else
                {
                    // 右方向
                    if (cross < 0)  animationindex = Player::Animation::RollRight;
                    // 左方向
                    else            animationindex = Player::Animation::RollLeft;
                }
            }
            // 回転角が９０度よりも大きければ 後,右,左 の三択
            else
            {
                // 回転角が１３５度よりも大きければ 後方向
                if (angle > DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4)
                {
                    animationindex = Player::Animation::RollBack;
                }
                else
                {
                    // 右方向
                    if (cross < 0)  animationindex = Player::Animation::RollRight;
                    // 左方向
                    else            animationindex = Player::Animation::RollLeft;
                }
            }
        }
        // 入力値がない場合前方向のアニメーションを設定する
        else
        {
            animationindex = Player::Animation::RollFront;
        }
        owner_->PlayBlendAnimation(animationindex, false, animationSpeed, animationStartFrame_);
    }
}

// ----- ジャスト回避 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void JustDodgeState::Initialize()
    {
        // 敵の攻撃判定を無くす
        EnemyManager::Instance().GetEnemy(0)->SetIsAttackActivationAllowed(false);
        EnemyManager::Instance().GetEnemy(0)->SetIsAttackActive(false);
        EnemyManager::Instance().GetEnemy(0)->ResetAllAttackActiveFlag();

        // ラジアルブラー設定
        PostProcess::Instance().SetUseRadialBlur();
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 5;
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->uvOffset_ = { 0.5f, 0.5f };
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = 1.5f;

        // ビネット設定
        PostProcess::Instance().SetUseVignette();
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteColor_ = { 0.65f, 0.65f, 0.65f, 1.0f };
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteCenter_ = { 0.5f, 0.5f };
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteSmoothness_ = 2.2f;
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteIntensity_ = 0.0f;
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteRounded_ = 1.0f;
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteRoundness_ = 1.0f;

        // コントローラー振動を入れる
        Input::Instance().GetGamePad().Vibration(vibrationTime_, 1.0f);

        // 現在鳴っているSEを全て停止させる
        AudioManager::Instance().StopAllSE();
        // ジャスト回避のSEを鳴らす
        AudioManager::Instance().PlaySE(SE::JustDodge);
        slowSENum_ = AudioManager::Instance().PlaySE(SE::Slow);

        // ラッシュ攻撃UIを生成
        owner_->GenerateUIRush();

        slowTimer_ = 0.0f;
        lerpTimer_ = 0.0f;
    }

    // ----- 更新 -----
    void JustDodgeState::Update(const float& elapsedTime)
    {
        // ----------------------------------------
        //  ラジアルブラーとビネット更新
        // ----------------------------------------
        // 中心点をプレイヤーの腰に合わせる
        DirectX::XMFLOAT3 jointPosition_float3 = owner_->GetJointPosition("pelvis");
        DirectX::XMFLOAT2 jointPosition_float2 = Sprite::ConvertToScreenPos(jointPosition_float3);
        jointPosition_float2.x /= SCREEN_WIDTH;
        jointPosition_float2.y /= SCREEN_HEIGHT;
        // 中心点を 0.0 ~ 1.0 の間に収める
        jointPosition_float2.x = std::clamp(jointPosition_float2.x, 0.0f, 1.0f);
        jointPosition_float2.y = std::clamp(jointPosition_float2.y, 0.0f, 1.0f);

        PostProcess::Instance().GetRadialBlurConstants()->GetData()->uvOffset_ = jointPosition_float2;
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteCenter_ = jointPosition_float2;

        lerpTimer_ += lerpSpeed_ * elapsedTime;
        lerpTimer_ = std::min(lerpTimer_, 1.0f);
        const float strength = XMFloatLerp(1.5f, maxLerpStrength_, lerpTimer_);
        
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = strength;

        const float intensity = XMFloatLerp(1.5f, 1.0f, lerpTimer_);
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteIntensity_ = intensity;
        const float color = XMFloatLerp(0.3f, 0.65f, lerpTimer_);
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteColor_ = { color, color, color, 1.0f };

        // スローの設定
        if (owner_->GetAnimationSeconds() > 0.78f)
        {
            SystemManager::Instance().SetAllSlowSpeed(1.0f);
            SystemManager::Instance().SetPlayerSlowSpeed(1.0f);
        }
        else if (owner_->GetAnimationSeconds() > 0.33f)
        {
            slowTimer_ += elapsedTime;
            if (slowTimer_ > slowStartFrame_)
            {
                SystemManager::Instance().SetPlayerSlowSpeed(0.1f);
            }
            else
            {
                SystemManager::Instance().SetPlayerSlowSpeed(playerSlowSpeed_);
                SystemManager::Instance().SetAllSlowSpeed(allSlowSpeed_);
            }
        }

        // ラッシュ攻撃入力
        if (owner_->GetAnimationSeconds() <= 0.78f)
        {
            if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_B)
            {
                owner_->ChangeState(Player::STATE::RushAttack);
                return;
            }
        }

        // RootMotionの設定
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotionを使用する
            owner_->SetUseRootMotion(true);
            owner_->SetRootMotionValue(rootMotionMoveValue_);
        }

        // 回避のアニメーションのままなら、待機に遷移
        if (owner_->IsPlayAnimation() == false)
        {
            Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

            if (animationIndex == Player::Animation::RollFront || animationIndex == Player::Animation::RollBack ||
                animationIndex == Player::Animation::RollRight || animationIndex == Player::Animation::RollLeft)
            {
                // ラッシュ攻撃UIを削除する
                owner_->RemoveUIRush();

                owner_->ChangeState(Player::STATE::Idle);
                return;
            }
        }

        // キャンセルボタンを押された
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_X)
        {
            // ラッシュ攻撃UIを削除する
            owner_->RemoveUIRush();
            
            owner_->ChangeState(Player::STATE::JustDodgeCancel);
            return;
        }
    }

    // ----- 終了化 -----
    void JustDodgeState::Finalize()
    {
        // ラジアルブラーリセット
        PostProcess::Instance().SetUseRadialBlur(false);
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 1;

        // ビネットリセット
        PostProcess::Instance().SetUseVignette(false);

        SystemManager::Instance().SetAllSlowSpeed(1.0f);
        SystemManager::Instance().SetPlayerSlowSpeed(1.0f);

        owner_->SetUseRootMotion(false);

        // 無敵状態無効化
        owner_->SetIsInvincible(false);

        // SEを止める
        AudioManager::Instance().StopSE(SE::Slow, slowSENum_);
    }

    // ----- ImGui用 -----
    void JustDodgeState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- GamePad ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("VibrationTime", &vibrationTime_, 0.01f, 0.0f, 1.0f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Slow ----------"))
            {
                ImGui::DragFloat("AllSlowSpeed", &allSlowSpeed_, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("PlayerSlowSpeed", &playerSlowSpeed_, 0.01f, 0.0f, 1.0f);

                ImGui::DragFloat("SlowTimer", &slowTimer_);
                ImGui::DragFloat("SlowStartFrame", &slowStartFrame_, 0.1f, 0.0f, 1.0f);

                ImGui::TreePop();
            }


            ImGui::DragFloat("LerpTimer", &lerpTimer_);
            ImGui::DragFloat("LerpSpeed", &lerpSpeed_);
            ImGui::DragFloat("MaxLerpStrength", &maxLerpStrength_, 0.01f, 0.0f, 1.0f);

            ImGui::TreePop();
        }
    }
}

// ----- ジャスト回避キャンセル -----
namespace PlayerState
{
    // ----- 初期化 -----
    void JustDodgeCancelState::Initialize()
    {
        owner_->SetUseRootMotion(true);
    }

    // ----- 更新 -----
    void JustDodgeCancelState::Update(const float& elapsedTime)
    {
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void JustDodgeCancelState::Finalize()
    {
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui用 -----
    void JustDodgeCancelState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {

            ImGui::TreePop();
        }
    }
}

// ----- ラッシュ攻撃 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void RushAttackState::Initialize()
    {
        owner_->SetAnimationSpeed(2.0f);

        SystemManager::Instance().SetAllSlowSpeed(0.01f);
        SystemManager::Instance().SetPlayerSlowSpeed(1.0f);

        // ラジアルブラー設定
        PostProcess::Instance().SetUseRadialBlur();
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 5;
        // ビネット設定
        PostProcess::Instance().SetUseVignette();

        // 無敵状態にしておく
        owner_->SetIsInvincible(true);

        // ラッシュ攻撃(移動)SEを再生
        AudioManager::Instance().PlaySE(SE::RushAttackMove0);
        AudioManager::Instance().PlaySE(SE::RushAttackMove1);

        // 変数初期化
        currentAttackNum_ = 0;
        radialBlurLerpTimer_ = 0.0f;
        startRadialBlurStrength_ = PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_;
    }

    // ----- 更新 -----
    void RushAttackState::Update(const float& elapsedTime)
    {
        DirectX::XMFLOAT3 jointPosition_float3 = owner_->GetJointPosition("pelvis");
        DirectX::XMFLOAT2 jointPosition_float2 = Sprite::ConvertToScreenPos(jointPosition_float3);
        jointPosition_float2.x /= SCREEN_WIDTH;
        jointPosition_float2.y /= SCREEN_HEIGHT;
        // 中心点を 0.0 ~ 1.0 の間に収める
        jointPosition_float2.x = std::clamp(jointPosition_float2.x, 0.0f, 1.0f);
        jointPosition_float2.y = std::clamp(jointPosition_float2.y, 0.0f, 1.0f);
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->uvOffset_ = jointPosition_float2;
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteCenter_ = jointPosition_float2;

        Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        // ラジアルブラー更新
        radialBlurLerpTimer_ += radialBlurLerpSpeed_ * elapsedTime;
        radialBlurLerpTimer_ = std::min(radialBlurLerpTimer_, 1.0f);
        const float strength = XMFloatLerp(startRadialBlurStrength_, 0.1f, radialBlurLerpTimer_);
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = strength;

        // -----------------------------------------------------------------
        //  回避モーションの場合脚が地面につくまで再生する。
        //  再生し終わったら次のアニメーションを再生する
        // -----------------------------------------------------------------
        if (animationIndex == Player::Animation::RollFront || animationIndex == Player::Animation::RollBack ||
            animationIndex == Player::Animation::RollRight || animationIndex == Player::Animation::RollLeft)
        {
            if(owner_->GetAnimationSeconds() > 0.85f/*脚が地面に着くフレーム*/)
            {
                owner_->PlayBlendAnimation(Player::Animation::DodgeFront, false, 1.0f, 0.3f);
                owner_->SetTransitionTime(0.05f);

                // 敵のアニメーションに合わせてターゲットを設定する
                SetTargetPosition();
            }
        }

        // -----------------------------------------------------------------
        //  敵との距離を一気に詰める
        // -----------------------------------------------------------------
        if (animationIndex == Player::Animation::DodgeFront)
        {
            // ルートモーションを使用する
            if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
            {
                owner_->SetUseRootMotion(true);

                owner_->SetRootMotionValue(dashRootMotionValue_);
            }

            // 移動判定
            DirectX::XMFLOAT3 ownerPosition_float3 = owner_->GetTransform()->GetPosition();
            DirectX::XMFLOAT2 ownerPosition = { ownerPosition_float3.x, ownerPosition_float3.z };
            DirectX::XMFLOAT2 targetposition = { targetPosition_.x, targetPosition_.z };
            const float length = XMFloat2Length(ownerPosition - targetposition);
            if (length < radius_)
            {
                owner_->SetRootMotionValue(0.0f);
            }

            // 回転処理
            Turn(elapsedTime);

            // 移動のアニメーションが終わったら一撃目を繰り出す
            if (owner_->GetAnimationSeconds() > 0.45f/*移動のアニメーション終了フレーム*/)
            {
                owner_->PlayBlendAnimation(Player::Animation::AttackRush0, false, 1.0f, 0.18f);
                owner_->SetUseRootMotion(false);

                attackData_.Initialize(0.18f, 0.28f);
                owner_->ResetFlags();

                AudioManager::Instance().PlaySE(SE::SowrdSlash2);

                ++currentAttackNum_;
            }
        }

        // ラッシュ攻撃処理
        if (animationIndex == Player::Animation::AttackRush0)
        {
            // 先行入力取得
            if (owner_->IsRushAttackKeyDown()) isNextInput_ = true;

            // 攻撃判定処理
            const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
            owner_->SetIsAttackValid(attackFlag);

            // 攻撃終了フレーム
            if (owner_->GetAnimationSeconds() > 0.28f)
            {
                // アニメーション再生速度を落とす
                owner_->SetAnimationSpeed(0.2f);

                // 先行入力があれば次の攻撃
                if (isNextInput_)
                {
                    owner_->PlayBlendAnimation(Player::Animation::AttackRush1, false, 1.0f, 0.15f);
                    isNextInput_ = false;

                    AudioManager::Instance().PlaySE(SE::SowrdSlash2);

                    attackData_.Initialize(0.15f, 0.3f);
                    owner_->ResetFlags();

                    ++currentAttackNum_;
                }
            }
        }
        if (animationIndex == Player::Animation::AttackRush1)
        {
            // 先行入力取得
            if (owner_->IsRushAttackKeyDown()) isNextInput_ = true;

            // 攻撃判定処理
            const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
            owner_->SetIsAttackValid(attackFlag);

            // 攻撃終了フレーム
            if (owner_->GetAnimationSeconds() > 0.3f)
            {
                // アニメーション再生速度を落とす
                owner_->SetAnimationSpeed(0.2f);

                // 先行入力があれば次の攻撃
                if (isNextInput_)
                {
                    owner_->PlayBlendAnimation(Player::Animation::AttackRush2, false, 1.0f, 0.15f);
                    isNextInput_ = false;

                    AudioManager::Instance().PlaySE(SE::SowrdSlash2);

                    attackData_.Initialize(0.15f, 0.25f);
                    owner_->ResetFlags();

                    ++currentAttackNum_;
                }
            }
        }
        if (animationIndex == Player::Animation::AttackRush2)
        {
            // 先行入力取得
            if (owner_->IsRushAttackKeyDown()) isNextInput_ = true;

            // 攻撃判定処理
            const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
            owner_->SetIsAttackValid(attackFlag);

            // 攻撃終了フレーム
            if (owner_->GetAnimationSeconds() > 0.25f)
            {
                // アニメーション再生速度を落とす
                owner_->SetAnimationSpeed(0.2f);

                // 先行入力があれば次の攻撃
                if (isNextInput_)
                {
                    owner_->PlayBlendAnimation(Player::Animation::AttackRush3, false, 1.0f, 0.15f);
                    isNextInput_ = false;

                    AudioManager::Instance().PlaySE(SE::SowrdSlash2);

                    attackData_.Initialize(0.15f, 0.35f);
                    owner_->ResetFlags();

                    ++currentAttackNum_;
                }
            }
        }
        if (animationIndex == Player::Animation::AttackRush3)
        {
            // 先行入力取得
            if (owner_->IsRushAttackKeyDown()) isNextInput_ = true;

            // 攻撃判定処理
            const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
            owner_->SetIsAttackValid(attackFlag);

            // 攻撃終了フレーム
            if (owner_->GetAnimationSeconds() > 0.35f)
            {
                // アニメーション再生速度を落とす
                owner_->SetAnimationSpeed(0.2f);

                // 先行入力があれば次の攻撃
                if (isNextInput_)
                {
                    owner_->PlayBlendAnimation(Player::Animation::AttackRush0, false, 1.0f, 0.18f);
                    isNextInput_ = false;

                    AudioManager::Instance().PlaySE(SE::SowrdSlash2);

                    attackData_.Initialize(0.18f, 0.28f);
                    owner_->ResetFlags();

                    ++currentAttackNum_;
                }

                if (currentAttackNum_ >= 8)
                {
                    owner_->ChangeState(Player::STATE::Idle);
                    return;
                }
            }
        }

        if (animationIndex == Player::Animation::AttackRush0 ||
            animationIndex == Player::Animation::AttackRush1 ||
            animationIndex == Player::Animation::AttackRush2 ||
            animationIndex == Player::Animation::AttackRush3)
        {
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->ChangeState(Player::STATE::Idle);
                return;
            }
        }
    }

    // ----- 終了化 -----
    void RushAttackState::Finalize()
    {
        // ラジアルブラーリセット
        PostProcess::Instance().SetUseRadialBlur(false);
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 1;

        // ビネットリセット
        PostProcess::Instance().SetUseVignette(false);

        SystemManager::Instance().SetAllSlowSpeed(1.0f);
        SystemManager::Instance().SetPlayerSlowSpeed(1.0f);

        // 無敵状態解除
        owner_->SetIsInvincible(false);

        owner_->SetUseRootMotion(false);

        // ラッシュ攻撃UIを削除する
        owner_->RemoveUIRush();
    }

    // ----- ImGui用 -----
    void RushAttackState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragFloat("DashRootMotionValue", &dashRootMotionValue_, 0.1f, 0.0f, 10.0f);

            ImGui::DragFloat("RotationSpeed", &rotationSpeed_);
            ImGui::DragFloat3("TargetPosition", &targetPosition_.x);

            ImGui::DragInt("CurrentAttackNum", &currentAttackNum_);

            ImGui::TreePop();
        }
    }

    // ----- 旋回処理 -----
    void RushAttackState::Turn(const float& elapsedTime)
    {
        DirectX::XMFLOAT3 ownerFront_float3 = owner_->GetTransform()->CalcForward();
        DirectX::XMFLOAT2 ownerFront        = { ownerFront_float3.x, ownerFront_float3.z };
        DirectX::XMFLOAT2 ownerPosition     = { owner_->GetTransform()->GetPositionX(), owner_->GetTransform()->GetPositionZ() };
        DirectX::XMFLOAT2 targetPosition    = { targetPosition_.x, targetPosition_.z };
        DirectX::XMFLOAT2 vec               = XMFloat2Normalize(targetPosition - ownerPosition);

        ownerFront = XMFloat2Normalize(ownerFront);

        // 外積でどっちに回転するか判断
        float cross = XMFloat2Cross(vec, ownerFront);

        // 内積で回転幅を算出
        float dot = std::clamp(XMFloat2Dot(vec, ownerFront), -1.0f, 1.0f);
        float angle = acosf(dot);

        if (angle < DirectX::XMConvertToRadians(1)) return;

        const float speed = rotationSpeed_ * elapsedTime;
        angle *= speed;

        // 回転処理
        if (cross > 0)
        {
            owner_->GetTransform()->AddRotationY(-angle);
        }
        else
        {
            owner_->GetTransform()->AddRotationY(angle);
        }
    }

    // ----- 敵のアニメーションに合わせてターゲットを設定する -----
    void RushAttackState::SetTargetPosition()
    {
        Enemy* enemy = EnemyManager::Instance().GetEnemy(0);
        const int animationIndex = enemy->GetAnimationIndex();

        const int animationData[] =
        {
            static_cast<int>(Enemy::DragonAnimation::AttackSlam0), static_cast<int>(Enemy::DragonAnimation::AttackTurn), static_cast<int>(Enemy::DragonAnimation::AttackKnockBackEnd0), 
            static_cast<int>(Enemy::DragonAnimation::AttackTackle1), static_cast<int>(Enemy::DragonAnimation::AttackTackle3), static_cast<int>(Enemy::DragonAnimation::Nova1)
        };
        const char* jointName[] =
        {
            "Dragon15_r_hand", "Dragon15_l_horselink", "Dragon15_l_foot",
            "Dragon15_l_calf", "Dragon15_r_calf", "Dragon15_r_foot"
        };
        
        for (int i = 0; i < _countof(animationData); ++i)
        {
            if (animationData[i] != animationIndex) continue;

            targetJointName_ = jointName[i];
            break;
        }
        
        targetPosition_ = enemy->GetJointPosition(targetJointName_.c_str());
    }
}

// ----- カウンター -----
namespace PlayerState
{
    // ----- 初期化 -----
    void CounterState::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        // アニメーション設定
        SetAnimation();

        // カウンターカメラを使用する
        Camera::Instance().UseCounterCamera();

        // 変数初期化
        addForceBack_.Initialize(0.16f, 0.2f, 0.5f);
        addForceFront_.Initialize(0.66f, 0.30f, 1.0f);
        gamePadVibration_.Initialize(0.3f, 0.4f, 1.0f);
        attackData_.Initialize(0.75f, 1.0f);

        isNextInput_ = false;

        isCounterReaction = false;


        isRotating_ = false;


        const DirectX::XMFLOAT3 pos = owner_->GetJointPosition("spine_02");
        Effect* counterEffect = EffectManager::Instance().GetEffect("Mikiri");
        mikiriEffectHandle_ = counterEffect->Play(pos, 0.05f, 2.0f);
    }

    // ----- 更新 -----
    void CounterState::Update(const float& elapsedTime)
    {
        // カウンター判定
        if (owner_->GetAnimationSeconds() > counterEndFrame_)
        {
            if (owner_->GetIsCounter()) owner_->SetIsCounter(false);
        }
        else if (owner_->GetAnimationSeconds() > counterStartFrame_)
        {
            if (owner_->GetIsCounter() == false) owner_->SetIsCounter(true);
        }

        // エフェクト
        if (addForceBack_.GetIsAddForce())
        {
            EffectManager::Instance().AddPosition(mikiriEffectHandle_, mikiriEffectAddPosition_ * 3.0f * elapsedTime);
        }

        // 見切りが成功したか
        if (owner_->GetIsAbleCounterAttack() && isCounterReaction == false)
        {
            // --------------------------------------------------
            //      コントローラー振動、エフェクト、効果音を出す。
            //       出すタイミングはコントローラー振動に任せる
            // --------------------------------------------------
            bool isVibrated = gamePadVibration_.Update(owner_->GetAnimationSeconds());

            if (isVibrated)
            {
                // エフェクトを再生する
                Effect* counterEffect = EffectManager::Instance().GetEffect("Counter");
                if (counterEffect != nullptr)
                {
                    // エフェクトは剣の位置に出す
                    const DirectX::XMFLOAT3 offsetPosition = { -50.0f, 13.0f, 20.0f };
                    const DirectX::XMFLOAT3 position = owner_->GetJointPosition("hand_r", offsetPosition);

                    // 位置を更新するためのデータを保存する
                    effectOffsetVec_ = XMFloat3Normalize(position - owner_->GetTransform()->GetPosition());
                    effectLength_ = XMFloat3Length(position - owner_->GetTransform()->GetPosition());
                    counterEffectHandle_ = counterEffect->Play(position, 0.1f, 4.0f);
                }

                // 効果音再生 (見切成功)
                AudioManager::Instance().PlaySE(SE::Mikiri);

                // ポストエフェクトを使用 (ビネット)
                PostProcess::Instance().SetUseVignette();
                const DirectX::XMFLOAT3 playerPosition = owner_->GetJointPosition("pelvis");
                DirectX::XMFLOAT2 vignetteCenter = Sprite::ConvertToScreenPos(playerPosition);
                vignetteCenter.x /= SCREEN_WIDTH;
                vignetteCenter.y /= SCREEN_HEIGHT;
                PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteCenter_ = vignetteCenter;
                PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteIntensity_ = vignetteMaxIntensity_;
                PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteSmoothness_ = 2.2f;
                PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteColor_ = vignetteColor_;
                vignetteTimer_ = 0.0f;
                isVignetteActive_ = true;

                isCounterReaction = true;
            }
        }

        // ビネット更新
        if (isVignetteActive_)
        {
            vignetteTimer_ += vignetteFadeOutSpeed_ * elapsedTime;
            vignetteTimer_ = std::min(vignetteTimer_, 1.0f);
            const float intensity = XMFloatLerp(vignetteMaxIntensity_, 0.0f, vignetteTimer_);
            PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteIntensity_ = intensity;
        }

        // エフェクトの位置を更新する
        if (isCounterReaction)
        {
            const DirectX::XMFLOAT3 position = owner_->GetTransform()->GetPosition() + effectOffsetVec_ * effectLength_;
            EffectManager::Instance().SetPosition(counterEffectHandle_, position);
        }

        // アニメーションの速度設定
        SetAnimationSpeed();

        // 移動処理
        Move();

        // 旋回処理
        Turn(elapsedTime);

        // 攻撃判定処理
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);

        // アニメーション再生終了
        //if(owner_->GetAnimationSeconds() > 1.0f)
        if(owner_->GetAnimationSeconds() > 1.2f)
        //if(owner_->IsPlayAnimation() == false)
        {
            EffectManager::Instance().StopEffect(counterEffectHandle_);

            owner_->ChangeState(Player::STATE::Idle);

            return;
        }

        // カウンター成功
        if (owner_->GetIsAbleCounterAttack())
        {
            if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_RIGHT_TRIGGER)
            {
                isNextInput_ = true;
            }
        }
        if (isNextInput_ && owner_->GetAnimationSeconds() > 0.9f)
        {
            EffectManager::Instance().StopEffect(counterEffectHandle_);

            owner_->ChangeState(Player::STATE::CounterCombo);
            return;
        }
    }

    // ----- 終了化 -----
    void CounterState::Finalize()
    {
        // ビネット使用フラグを下げる
        PostProcess::Instance().SetUseVignette(false);
    }

    // ----- ImGui用 -----
    void CounterState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- Vignette ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("Timer", &vignetteTimer_, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("FadeOutSpeed", &vignetteFadeOutSpeed_, 0.01f, 0.0f, 5.0f);
                ImGui::DragFloat("Intensity", &vignetteMaxIntensity_, 0.01f, 0.0f, 5.0f);
                ImGui::ColorEdit4("Color", &vignetteColor_.x);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション設定 -----
    void CounterState::SetAnimation()
    {
        const Player::STATE oldState = owner_->GetOldState();
        
        if (oldState == Player::STATE::GuardCounter)
        {
            return;
        }
        
        float transitionTime = 0.1f;

        if (oldState == Player::STATE::ComboAttack0_0 || oldState == Player::STATE::ComboAttack0_1 ||
            oldState == Player::STATE::ComboAttack0_2 || oldState == Player::STATE::RunAttack)
        {
            owner_->PlayBlendAnimation(Player::Animation::Counter, false, 1.0f, 0.15f);
            owner_->SetTransitionTime(0.1f);
            return;
        }

        if (oldState == Player::STATE::Idle)
        {
            transitionTime = transitionIdle_;
        }
        else if (oldState == Player::STATE::Run)
        {
            transitionTime = transitionRun_;
        }

        owner_->PlayBlendAnimation(Player::Animation::Counter, false, 1.0f, 0.15f);
        owner_->SetTransitionTime(transitionTime);
    }

    // ----- 移動処理 -----
    void CounterState::Move()
    {
        // 後ろ方向に進む
#pragma region ---------- 後ろ方向に進む ----------
        if (addForceBack_.Update(owner_->GetAnimationSeconds()))
        {
            // --------------------------------------------------
            //  左スティックの入力があればその方向に向くようにする
            //          何も入力がなければ後ろに下がる
            // --------------------------------------------------

            // 左スティックの入力があるか判定
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                // カメラから見た左スティックの傾きを適応した方向を算出する
                DirectX::XMFLOAT2 direction = Camera::Instance().ConvertTo2DVectorFromCamera(DirectX::XMFLOAT2(aLx, aLy));
                addForceDirection_ = XMFloat2Normalize(direction) * -1.0f;

                DirectX::XMFLOAT2 ownerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
                ownerForward = XMFloat2Normalize(ownerForward);

                // 外積をしてどちらに回転するのかを判定する
                float corss = XMFloat2Cross(direction, ownerForward);

                // 内積で回転幅を算出
                float angle = acosf(std::clamp(XMFloat2Dot(direction, ownerForward), -1.0f, 1.0f));

                if (corss > 0)
                {
                    owner_->GetTransform()->AddRotationY(-angle);
                }
                else
                {
                    owner_->GetTransform()->AddRotationY(angle);
                }
            }
            // 左スティックの入力がない場合後ろ方向に引く
            else
            {
                const DirectX::XMFLOAT3 ownerBack = owner_->GetTransform()->CalcForward() * -1.0f;

                addForceDirection_ = DirectX::XMFLOAT2(ownerBack.x, ownerBack.z);
            }

            mikiriEffectAddPosition_ = DirectX::XMFLOAT3(addForceDirection_.x, 0.0f, addForceDirection_.y);

            owner_->AddForce(DirectX::XMFLOAT3(addForceDirection_.x, 0.0f, addForceDirection_.y), addForceBack_.GetForce(), addForceBack_.GetDecelerationForce());
        }
#pragma endregion ---------- 後ろ方向に進む ----------

        // 前方向に進む
#pragma region ---------- 前方向に進む ----------
        if (addForceFront_.Update(owner_->GetAnimationSeconds()))
        {
            // --------------------------------------------------
            //  左スティックの入力があればその方向に向くようにする
            //          何も入力がなければそのまま前に進む
            // --------------------------------------------------

            // 左スティックの入力があるか判定
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                // カメラから見た左スティックの傾きを適応した方向を算出する
                DirectX::XMFLOAT2 direction = Camera::Instance().ConvertTo2DVectorFromCamera(DirectX::XMFLOAT2(aLx, aLy));
                addForceDirection_ = XMFloat2Normalize(direction);

                // ------------------------------------------------------------
                //              回転する角度は左右ともに９０度まで
                // 
                //                      ownerFront
                //                          |
                //                          | 
                //             Left ================== Right
                //                         Back
                // 
                //           0° ~ 90°, 270° ~ 360° になるように補正する
                // ------------------------------------------------------------
                DirectX::XMFLOAT2 ownerFront = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
                ownerFront = XMFloat2Normalize(ownerFront);

                // 内積で角度を算出
                float angle = acosf(std::clamp(XMFloat2Dot(addForceDirection_, ownerFront), -1.0f, 1.0f));

                // 90度以上回転角がある
                if (angle > DirectX::XM_PIDIV2)
                {
                    // 左右判定
                    float cross = XMFloat2Cross(addForceDirection_, ownerFront);

                    const DirectX::XMFLOAT3 ownerRight_float3 = owner_->GetTransform()->CalcRight();
                    DirectX::XMFLOAT2 ownerRight_float2 = XMFloat2Normalize({ ownerRight_float3.x, ownerRight_float3.z });

                    if (cross > 0)
                    {
                        addForceDirection_ = ownerRight_float2  * -1.0f;
                    }
                    else
                    {
                        addForceDirection_ = ownerRight_float2;
                    }
                }

                isRotating_ = true;
            }
            else
            {
                const DirectX::XMFLOAT3 ownerForward = owner_->GetTransform()->CalcForward();
                addForceDirection_ = DirectX::XMFLOAT2(ownerForward.x, ownerForward.z);
            }

            owner_->AddForce(DirectX::XMFLOAT3(addForceDirection_.x, 0.0f, addForceDirection_.y), addForceFront_.GetForce(), addForceFront_.GetDecelerationForce());
        }
#pragma endregion ---------- 前方向に進む ----------
    }

    // ----- 旋回処理 -----
    void CounterState::Turn(const float& elapsedTime)
    {
        // 回転角度がないのでここで終了
        if (isRotating_ == false) return;

        // まだ旋回処理を行わない
        if (addForceFront_.GetIsAddForce() == false) return;

        DirectX::XMFLOAT2 ownerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
        ownerForward = XMFloat2Normalize(ownerForward);

        // 外積をしてどちらに回転するのかを判定する
        float corss = XMFloat2Cross(XMFloat2Normalize(addForceDirection_), ownerForward);

        // 内積で回転幅を算出
        float angle = acosf(std::clamp(XMFloat2Dot(XMFloat2Normalize(addForceDirection_), ownerForward), -1.0f, 1.0f));
        if (angle < DirectX::XMConvertToRadians(1)) return;

        const float speed = owner_->GetRotateSpeed() * elapsedTime;
        angle *= speed;

        if (corss > 0)
        {
            owner_->GetTransform()->AddRotationY(-angle);
        }
        else
        {
            owner_->GetTransform()->AddRotationY(angle);
        }
    }

    // ----- アニメーションの速度設定 -----
    void CounterState::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > 0.7f)
        {
            owner_->SetAnimationSpeed(1.0f);
        }
        else if (animationSeconds > 0.4f)
        {
            owner_->SetAnimationSpeed(0.7f);
        }
    }
}

// ----- カウンター攻撃 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void CounterComboState::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        // アニメーション再生
        owner_->PlayBlendAnimation(Player::Animation::CounterAttack1, false, 1.0f, 0.35f);        

        // 無敵状態にする
        owner_->SetIsInvincible(true);

        // カウンター攻撃カメラを使用する
        Camera::Instance().UseCounterAttackCamera();

        // 変数初期化
        attackData_.Initialize(0.35f, 0.7f);

        isPlayCameraVibration_ = false;
    }

    // ----- 更新 -----
    void CounterComboState::Update(const float& elapsedTime)
    {
        // デバッグ用に現在のアニメーションフレームを保存
        currentAnimationFrame_ = owner_->GetAnimationSeconds();

        // 旋回処理
        Turn(elapsedTime);

        // アニメーション速度調整
        UpdateAnimationSpeed();

        // カメラシェイク
        if (owner_->GetAnimationSeconds() > cameraVibrationFrame_ && isPlayCameraVibration_ == false)
        {
            Camera::Instance().ScreenVibrate(cameraVibrationVolume_, cameraVibrationTime_);

            isPlayCameraVibration_ = true;
        }

        // RootMotionの設定
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotionを使用する
            owner_->SetUseRootMotion(true);
            owner_->SetRootMotionValue(1.0f);
        }

        // 攻撃判定処理
        const bool attackFlag = attackData_.Update(currentAnimationFrame_, owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);

        // 剣の軌跡更新
        if (currentAnimationFrame_ > swordTrailEndFrame_)
        {
            if(owner_->GetIsDrawSwordTrail()) owner_->SetIsDrawSwordTrail(false);
        }
        else if (currentAnimationFrame_ > swordTrailStartFrame_ && owner_->GetIsDrawSwordTrail() == false)
        {
            owner_->SetIsDrawSwordTrail(true);
        }

        // アニメーション終了
        if (!owner_->IsPlayAnimation())
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void CounterComboState::Finalize()
    {
        // 無敵状態にを解除する
        owner_->SetIsInvincible(false);

        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui用 -----
    void CounterComboState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragFloat("AnimationFrame", &currentAnimationFrame_);
            if (ImGui::TreeNodeEx("---------- CameraVibration ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("Frame", &cameraVibrationFrame_, 0.01f, 0.0f, 3.0f);
                ImGui::DragFloat("Volume", &cameraVibrationVolume_, 0.01f, 0.0f, 3.0f);
                ImGui::DragFloat("Time", &cameraVibrationTime_, 0.01f, 0.0f, 3.0f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Slow ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("EndFrame", &slowEndFrame_, 0.01f, 0.0f, 3.0f);
                ImGui::DragFloat("AnimationSpeed", &slowAnimationSpeed_, 0.01f, 0.0f, 1.0f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- SwordTrail ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("StartFrame", &swordTrailStartFrame_, 0.01f, 0.0f, 3.0f);
                ImGui::DragFloat("EndFrame", &swordTrailEndFrame_, 0.01f, 0.0f, 3.0f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション速度調整 -----
    void CounterComboState::UpdateAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();
        float animationSpeed = 1.0f;

        if (animationSeconds >= 0.35f && animationSeconds < 0.45f)
        {
            animationSpeed = 0.8f;
        }
        else if (animationSeconds < slowEndFrame_)
        {
            animationSpeed = slowAnimationSpeed_;
        }

        owner_->SetAnimationSpeed(animationSpeed);
    }

    // ----- 旋回処理 -----
    void CounterComboState::Turn(const float& elapsedTime)
    {
        // 旋回処理フレームを過ぎている
        if (owner_->GetAnimationSeconds() > rotationEndFrame_) return;

        const float aLX = Input::Instance().GetGamePad().GetAxisLX();
        const float aLY = Input::Instance().GetGamePad().GetAxisLY();

        DirectX::XMFLOAT2 direction = {};

        // スティック入力がある時はその方向へ旋回
        if (fabsf(aLX) > 0.0f || fabsf(aLY) > 0.0f)
        {
            direction = XMFloat2Normalize(Camera::Instance().ConvertTo2DVectorFromCamera(DirectX::XMFLOAT2(aLX, aLY)));
        }
        // 入力がない場合敵の方向へ旋回
        else
        {
            const DirectX::XMFLOAT3 ownerPosition = owner_->GetTransform()->GetPosition();
            const DirectX::XMFLOAT3 dragonPosition = EnemyManager::Instance().GetEnemy(0)->GetTransform()->GetPosition();
            direction = XMFloat2Normalize(DirectX::XMFLOAT2(dragonPosition.x - ownerPosition.x, dragonPosition.z - ownerPosition.z));
        }

        DirectX::XMFLOAT2 ownerForward = XMFloat2Normalize({ owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z });

        // 外積をしてどちらに回転するのかを判定する
        float corss = XMFloat2Cross(direction, ownerForward);

        // 内積で回転幅を算出
        float angle = acosf(std::clamp(XMFloat2Dot(direction, ownerForward), -1.0f, 1.0f));
        if (angle < DirectX::XMConvertToRadians(1)) return;

        const float speed = owner_->GetRotateSpeed() * elapsedTime;
        angle *= speed;

        if (corss > 0)
        {
            owner_->GetTransform()->AddRotationY(-angle);
        }
        else
        {
            owner_->GetTransform()->AddRotationY(angle);
        }
    }
}

// ----- 走り攻撃 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void RunAttackState::Initialize()
    {
        // フラグリセット
        owner_->ResetFlags();

        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::RunAttack1, false, 1.0f, 0.2f);
        owner_->SetTransitionTime(0.1f);

        // 先行入力設定
        owner_->SetNextInputStartFrame(0.0f, 0.3f, 0.3f, 0.8f);
        owner_->SetNextInputEndFrame(1.8f, 1.8f, 1.8f);
        owner_->SetNextInputTransitionFrame(0.6f, 0.6f, 0.6f);

        // ルートモーションを使用しない
        owner_->SetUseRootMotion(false);

        // 変数初期化
        addForceData_.Initialize(0.2f, 0.4f, 1.0f);
        attackData_.Initialize(0.25f, 0.5f);
    }

    // ----- 更新 -----
    void RunAttackState::Update(const float& elapsedTime)
    {
        // 先行入力処理
        if (CheckNextInput()) return;

        // 移動処理
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // 攻撃判定処理
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void RunAttackState::Finalize()
    {
    }

    void RunAttackState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {

            ImGui::TreePop();
        }
    }

    // ----- 先行入力処理 -----
    const bool RunAttackState::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // -----------------------------------
        //      回避による攻撃キャンセル
        // -----------------------------------
        if (owner_->GetIsDodgeAttackCancel())
        {
            if (owner_->IsDodgeKeyDown())
            {
                owner_->ChangeState(Player::STATE::Dodge);
                return true;
            }
        }
        else
        {
            // 回避先行入力受付
            if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
                animationSeconds <= owner_->GetDodgeInputEndFrame())
            {
                if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
            }
            // 回避遷移チェック
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                if (animationSeconds >= owner_->GetDodgeTransitionFrame())
                {
                    owner_->ChangeState(Player::STATE::Dodge);
                    return true;
                }
            }
        }

#pragma region ----- 先行入力受付 -----
        // 攻撃先行入力受付
        if (animationSeconds >= owner_->GetAttackInputStartFrame() &&
            animationSeconds <= owner_->GetAttackInputEndFrame())
        {
            if (owner_->IsComboAttack0KeyDown()) owner_->SetNextInput(Player::NextInput::ComboAttack0);
        }
        // カウンター先行入力受付
        if (animationSeconds >= owner_->GetCounterInputStartFrame() &&
            animationSeconds <= owner_->GetCounterInputEndFrame())
        {
            if (owner_->IsCounterStanceKey()) owner_->SetNextInput(Player::NextInput::Counter);
        }

#pragma endregion ----- 先行入力受付 -----

#pragma region ----- 遷移チェック -----
        // 攻撃遷移チェック
        if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
        {
            if (animationSeconds >= owner_->GetAttackTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_1);
                return true;
            }
        }
        // カウンター遷移チェック
        else if (owner_->GetNextInput() == Player::NextInput::Counter)
        {
            if (animationSeconds >= owner_->GetCounterTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Counter);
                return true;
            }
        }

#pragma endregion ----- 遷移チェック -----

        // 移動入力判定
        if (animationSeconds >= owner_->GetMoveInputStartFrame())
        {
            // スティック入力があるか
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (aLx == 0.0f && aLy == 0.0f) return false;

            owner_->ChangeState(Player::STATE::Run);
            return true;
        }

        return false;

    }
}

// ----- コンボ攻撃0_0 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void ComboAttack0_0::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        // アニメーション設定
        SetAnimation();

        // 回転補正量を求める
        owner_->CalculateRotationAdjustment();

        // 先行入力設定
        owner_->SetNextInputStartFrame(0.13f, 0.13f, 0.13f, 0.6f);
        owner_->SetNextInputEndFrame(1.583f, 0.75f, 1.5f);
        owner_->SetNextInputTransitionFrame(0.4f, 0.3f, 0.3f);

        // 変数初期化
        attackData_.Initialize(0.1f, 0.35f);      

        // SE再生
        AudioManager::Instance().PlaySE(SE::SowrdSlash0);
    }

    // ----- 更新 -----
    void ComboAttack0_0::Update(const float& elapsedTime)
    {
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y)
        {
            if (owner_->GetSwordSpirit() != 0.0f)
            {
                owner_->ChangeState(Player::STATE::Helmbreaker);
                return;
            }
        }

        // 先行入力判定
        if (CheckNextInput()) return;

        // RootMotionの設定
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotionを使用する
            owner_->SetUseRootMotion(true);
            owner_->SetRootMotionValue(1.0f);
        }
        
        // アニメーションの速度設定
        SetAnimationSpeed();

        // 攻撃判定処理
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);
        

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void ComboAttack0_0::Finalize()
    {
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui用 -----
    void ComboAttack0_0::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {

            ImGui::TreePop();
        }
    }

    // ----- アニメーション設定 -----
    void ComboAttack0_0::SetAnimation()
    {
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        // oldAnimationが回避の場合攻撃の途中フレームから開始する
        if (animationIndex == Player::Animation::RollFront ||
            animationIndex == Player::Animation::RollBack    ||
            animationIndex == Player::Animation::RollRight   ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->PlayBlendAnimation(Player::Animation::Attack0_0, false, 1.0f, 0.1f);
        }
        else if (animationIndex == Player::Animation::Run)
        {
            owner_->PlayBlendAnimation(Player::Animation::Attack0_0, false, 1.0f, 0.1f);
            owner_->SetTransitionTime(0.1f);
        }
        else if (animationIndex == Player::Animation::Attack0_1)
        {
            owner_->PlayBlendAnimation(Player::Animation::Attack0_0, false, 1.0f, 0.1f);
            owner_->SetTransitionTime(0.1f);
        }
        else
        {
            owner_->PlayBlendAnimation(Player::Animation::Attack0_0, false);
            owner_->SetTransitionTime(0.1f);
        }
    }

    // ----- アニメーション速度設定 -----
    void ComboAttack0_0::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > 1.2f)
        {
            owner_->SetAnimationSpeed(3.0f);
        }
        else if (animationSeconds > 1.0f)
        {
            owner_->SetAnimationSpeed(2.0f);
        }
        else if (animationSeconds > 0.6f)
        {
            owner_->SetAnimationSpeed(1.5f);
        }
        else if (animationSeconds > 0.3f)
        {
            owner_->SetAnimationSpeed(1.3f);
        }
    }
    
    // ----- 先行入力判定 -----
    const bool ComboAttack0_0::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // -----------------------------------
        //      回避による攻撃キャンセル
        // -----------------------------------
        if (owner_->GetIsDodgeAttackCancel())
        {
            if (owner_->IsDodgeKeyDown())
            {
                owner_->ChangeState(Player::STATE::Dodge);
                return true;
            }
        }
        else
        {
            // 回避先行入力受付
            if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
                animationSeconds <= owner_->GetDodgeInputEndFrame())
            {
                if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
            }

            // 回避遷移チェック
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                if (animationSeconds >= owner_->GetDodgeTransitionFrame())
                {
                    owner_->ChangeState(Player::STATE::Dodge);
                    return true;
                }
            }
        }


        // -----------------------------------
        //      移動入力による後隙キャンセル
        // -----------------------------------
        if (animationSeconds >= owner_->GetMoveInputStartFrame())
        {
            // スティック入力があるか
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (aLx == 0.0f && aLy == 0.0f) return false;

            owner_->SetIsMoveAttackCancel(true);

            owner_->ChangeState(Player::STATE::Run);
            return true;
        }




#pragma region ----- 先行入力受付 -----

        // 攻撃先行入力受付
        if (animationSeconds >= owner_->GetAttackInputStartFrame() &&
            animationSeconds <= owner_->GetAttackInputEndFrame())
        {
            if (owner_->IsComboAttack0KeyDown()) owner_->SetNextInput(Player::NextInput::ComboAttack0);
        }
        // カウンター先行入力受付
        if (animationSeconds >= owner_->GetCounterInputStartFrame() &&
            animationSeconds <= owner_->GetCounterInputEndFrame())
        {
            if (owner_->IsCounterStanceKey()) owner_->SetNextInput(Player::NextInput::Counter);
        }

#pragma endregion ----- 先行入力受付 -----

#pragma region ----- 遷移チェック -----

        // 攻撃遷移チェック
        if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
        {
            if (animationSeconds >= owner_->GetAttackTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_1);
                return true;
            }
        }
        // カウンター遷移チェック
        else if (owner_->GetNextInput() == Player::NextInput::Counter)
        {
            if (animationSeconds >= owner_->GetCounterTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Counter);
                return true;
            }
        }

#pragma endregion ----- 遷移チェック -----

        return false;
    }
}

// ----- コンボ攻撃0_1 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void ComboAttack0_1::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        // アニメーション設定
        SetAnimation();

        // 回転補正量を求める
        owner_->CalculateRotationAdjustment();

        // 先行入力設定
        owner_->SetNextInputStartFrame(0.13f, 0.13f, 0.13f, 0.5f);
        owner_->SetNextInputEndFrame(1.583f, 0.75f, 1.5f);
        owner_->SetNextInputTransitionFrame(0.4f, 0.3f, 0.3f);

        // 変数初期化
        attackData_.Initialize(0.06f, 0.3f);

        // SE再生
        AudioManager::Instance().PlaySE(SE::SowrdSlash1);
    }

    // ----- 更新 -----
    void ComboAttack0_1::Update(const float& elapsedTime)
    {
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y)
        {
            if (owner_->GetSwordSpirit() != 0.0f)
            {
                owner_->ChangeState(Player::STATE::Helmbreaker);
                return;
            }
        }

        // 先行入力処理
        if (CheckNextInput()) return;

        // RootMotionの設定
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotionを使用する
            owner_->SetUseRootMotion(true);
            owner_->SetRootMotionValue(1.0f);
        }

        // アニメーションの速度設定
        SetAnimationSpeed();

        // 攻撃判定処理
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);

        //if (owner_->IsPlayAnimation() == false)
        if(owner_->GetAnimationSeconds() > 1.0f)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void ComboAttack0_1::Finalize()
    {
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui用 -----
    void ComboAttack0_1::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {

            ImGui::TreePop();
        }
    }

    // ----- アニメーション設定 -----
    void ComboAttack0_1::SetAnimation()
    {
        if (owner_->GetAnimationIndex() == static_cast<int>(Player::Animation::RunAttack1))
        {
            owner_->SetTransitionTime(0.2f);
            owner_->PlayBlendAnimation(Player::Animation::Attack0_1, false, 1.0f, 0.1f);
        }
        else
        {
            owner_->SetTransitionTime(0.1f);
            owner_->PlayBlendAnimation(Player::Animation::Attack0_1, false);
        }
    }

    // ----- アニメーション速度設定 -----
    void ComboAttack0_1::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > 1.0f)
        {
            owner_->SetAnimationSpeed(2.5f);
        }
        else if (animationSeconds > 0.6f)
        {
            owner_->SetAnimationSpeed(1.5f);
        }
        else if (animationSeconds > 0.3f)
        {
            owner_->SetAnimationSpeed(1.3f);
        }
    }

    // ----- 先行入力処理 -----
    const bool ComboAttack0_1::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // -----------------------------------
        //      回避による攻撃キャンセル
        // -----------------------------------
        if (owner_->GetIsDodgeAttackCancel())
        {
            if (owner_->IsDodgeKeyDown())
            {
                owner_->ChangeState(Player::STATE::Dodge);
                return true;
            }
        }
        else
        {
            // 回避先行入力受付
            if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
                animationSeconds <= owner_->GetDodgeInputEndFrame())
            {
                if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
            }

            // 回避遷移チェック
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                if (animationSeconds >= owner_->GetDodgeTransitionFrame())
                {
                    owner_->ChangeState(Player::STATE::Dodge);
                    return true;
                }
            }
        }

        // -----------------------------------
        //      移動入力による後隙キャンセル
        // -----------------------------------
        if (animationSeconds >= owner_->GetMoveInputStartFrame())
        {
            // スティック入力があるか
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (aLx == 0.0f && aLy == 0.0f) return false;

            owner_->SetIsMoveAttackCancel(true);

            owner_->ChangeState(Player::STATE::Run);
            return true;
        }

#pragma region ----- 先行入力受付 -----
        // 攻撃先行入力受付
        if (animationSeconds >= owner_->GetAttackInputStartFrame() &&
            animationSeconds <= owner_->GetAttackInputEndFrame())
        {
            if (owner_->IsComboAttack0KeyDown()) owner_->SetNextInput(Player::NextInput::ComboAttack0);
        }
        // カウンター先行入力受付
        if (animationSeconds >= owner_->GetCounterInputStartFrame() &&
            animationSeconds <= owner_->GetCounterInputEndFrame())
        {
            if (owner_->IsCounterStanceKey()) owner_->SetNextInput(Player::NextInput::Counter);
        }

#pragma endregion ----- 先行入力受付 -----

#pragma region ----- 遷移チェック -----
        // 攻撃遷移チェック
        if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
        {
            if (animationSeconds >= owner_->GetAttackTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_2);
                return true;
            }
        }
        // カウンター遷移チェック
        else if (owner_->GetNextInput() == Player::NextInput::Counter)
        {
            if (animationSeconds >= owner_->GetCounterTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Counter);
                return true;
            }
        }

#pragma endregion ----- 遷移チェック -----

        return false;
    }
}

// ----- コンボ攻撃0_2 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void ComboAttack0_2::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::Attack0_2, false, 1.3f, 0.4f);
        owner_->SetTransitionTime(0.3f);

        // 回転補正量を求める
        owner_->CalculateRotationAdjustment();

        // 先行入力設定
        owner_->SetNextInputStartFrame(0.7f, 0.7f, 0.7f, 1.4f);
        owner_->SetNextInputEndFrame(1.9f, 1.3f, 1.3f);
        owner_->SetNextInputTransitionFrame(1.1f, 0.9f, 0.9f);
        
        // 変数初期化
        attackData_.Initialize(0.7f, 0.9f);
        isPlaySwordSlashSE_ = false;
    }

    // ----- 更新 -----
    void ComboAttack0_2::Update(const float& elapsedTime)
    {
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y)
        {
            if (owner_->GetSwordSpirit() != 0.0f)
            {
                owner_->ChangeState(Player::STATE::Helmbreaker);
                return;
            }
        }

        // 先行入力処理
        if (CheckNextInput()) return;

        // RootMotionの設定
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotionを使用する
            owner_->SetUseRootMotion(true);
            owner_->SetRootMotionValue(1.0f);
        }

        // アニメーションの速度設定
        SetAnimationSpeed();

        // 攻撃判定処理
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);

        // SE再生
        if (isPlaySwordSlashSE_ == false && owner_->GetAnimationSeconds() > swordSlashSEPlayFrame_)
        {
            AudioManager::Instance().PlaySE(SE::SowrdSlash0);
            isPlaySwordSlashSE_ = true;
        }


        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void ComboAttack0_2::Finalize()
    {
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui用 -----
    void ComboAttack0_2::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            if(ImGui::TreeNodeEx("---------- SE ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("SwordSlash");
                ImGui::Checkbox("IsPlay", &isPlaySwordSlashSE_);
                ImGui::DragFloat("PlayFrame", &swordSlashSEPlayFrame_);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーションの速度設定 -----
    void ComboAttack0_2::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > 1.4f)
        {
            owner_->SetAnimationSpeed(2.5f);
        }
        else if (animationSeconds > 1.2f)
        {
            owner_->SetAnimationSpeed(2.0f);
        }
        else if (animationSeconds > 0.7f)
        {
            owner_->SetAnimationSpeed(1.2f);
        }
    }

    // ----- 先行入力処理 -----
    const bool ComboAttack0_2::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // -----------------------------------
        //      回避による攻撃キャンセル
        // -----------------------------------
        if (owner_->GetIsDodgeAttackCancel())
        {
            if (owner_->IsDodgeKeyDown())
            {
                owner_->ChangeState(Player::STATE::Dodge);
                return true;
            }
        }
        else
        {
            // 回避先行入力受付
            if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
                animationSeconds <= owner_->GetDodgeInputEndFrame())
            {
                if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
            }

            // 回避遷移チェック
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                if (animationSeconds >= owner_->GetDodgeTransitionFrame())
                {
                    owner_->ChangeState(Player::STATE::Dodge);
                    return true;
                }
            }
        }

        // -----------------------------------
        //      移動入力による後隙キャンセル
        // -----------------------------------
        if (animationSeconds >= owner_->GetMoveInputStartFrame())
        {
            // スティック入力があるか
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (aLx == 0.0f && aLy == 0.0f) return false;

            owner_->SetIsMoveAttackCancel(true);

            owner_->ChangeState(Player::STATE::Run);
            return true;
        }

#pragma region ----- 先行入力受付 -----
        // 攻撃先行入力受付
        if (animationSeconds >= owner_->GetAttackInputStartFrame() &&
            animationSeconds <= owner_->GetAttackInputEndFrame())
        {
            if (owner_->IsComboAttack0KeyDown()) owner_->SetNextInput(Player::NextInput::ComboAttack0);
        }
        // カウンター先行入力受付
        if (animationSeconds >= owner_->GetCounterInputStartFrame() &&
            animationSeconds <= owner_->GetCounterInputEndFrame())
        {
            if (owner_->IsCounterStanceKey()) owner_->SetNextInput(Player::NextInput::Counter);
        }

#pragma endregion ----- 先行入力受付 -----

#pragma region ----- 遷移チェック -----
        // 攻撃遷移チェック
        if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
        {
            if (animationSeconds >= owner_->GetAttackTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_3);
                return true;
            }
        }
        // カウンター遷移チェック
        else if (owner_->GetNextInput() == Player::NextInput::Counter)
        {
            if (animationSeconds >= owner_->GetCounterTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Counter);
                return true;
            }
        }

#pragma endregion ----- 遷移チェック -----

        return false;
    }
}

// ----- コンボ攻撃0_3 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void ComboAttack0_3::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::Attack0_3, false);
        owner_->SetTransitionTime(0.1f);

        // 回転補正量を求める
        owner_->CalculateRotationAdjustment();

        // 先行入力設定
        owner_->SetNextInputStartFrame(0.7f, 3.0f, 3.0f, 3.0f);
        owner_->SetNextInputEndFrame(2.0f, 3.0f, 3.0f);
        owner_->SetNextInputTransitionFrame(1.3f, 3.0f, 3.0f);

        // 変数初期化
        attackData_.Initialize(0.65f, 0.8f);
        isVibration_ = false;
        isPlaySwordSlashSE_ = false;
    }

    // ----- 更新 -----
    void ComboAttack0_3::Update(const float& elapsedTime)
    {
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y)
        {
            if (owner_->GetSwordSpirit() != 0.0f)
            {
                owner_->ChangeState(Player::STATE::Helmbreaker);
                return;
            }
        }

        // 先行入力判定
        if (CheckNextInput()) return;

        // アニメーションの速度設定
        SetAnimationSpeed();

        // RootMotionの設定
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotionを使用する
            owner_->SetUseRootMotion(true);
            owner_->SetRootMotionValue(1.0f);
        }

        // 旋回処理
        

        // 攻撃判定処理
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);

        // SE再生
        if (isPlaySwordSlashSE_ == false && owner_->GetAnimationSeconds() > swordSlashSEPlayFrame_)
        {
            AudioManager::Instance().PlaySE(SE::SowrdSlash1);
            isPlaySwordSlashSE_ = true;
        }

        // コントローラー＆カメラ 振動
        if (owner_->GetAnimationSeconds() > 0.8f && isVibration_ == false)
        {
            Input::Instance().GetGamePad().Vibration(0.2f, 1.0f);
            Camera::Instance().ScreenVibrate(0.1f, 0.2f);

            isVibration_ = true;
        }

        // ルートモーションの移動値を更新
        UpdateRootMotionMovement();

        if(owner_->GetAnimationSeconds() > 1.7f)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void ComboAttack0_3::Finalize()
    {
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui用 -----
    void ComboAttack0_3::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragFloat("RootMotionValue", &rootMotionValue_);

            if (ImGui::TreeNodeEx("---------- SE ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("SwordSlash");
                ImGui::Checkbox("IsPlay", &isPlaySwordSlashSE_);
                ImGui::DragFloat("PlayFrame", &swordSlashSEPlayFrame_);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーションの速度設定 -----
    void ComboAttack0_3::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();


    }
    
    // ----- 先行入力処理 -----
    const bool ComboAttack0_3::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // -----------------------------------
        //      回避による攻撃キャンセル
        // -----------------------------------
        if (owner_->GetIsDodgeAttackCancel())
        {
            if (owner_->IsDodgeKeyDown())
            {
                owner_->ChangeState(Player::STATE::Dodge);
                return true;
            }
        }
        else
        {
            // 回避先行入力受付
            if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
                animationSeconds <= owner_->GetDodgeInputEndFrame())
            {
                if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
            }

            // 回避遷移チェック
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                if (animationSeconds >= owner_->GetDodgeTransitionFrame())
                {
                    owner_->ChangeState(Player::STATE::Dodge);
                    return true;
                }
            }
        }

        return false;
    }
    
    // ----- ルートモーションの移動値を更新する -----
    void ComboAttack0_3::UpdateRootMotionMovement()
    {
        const float aLX = Input::Instance().GetGamePad().GetAxisLX();
        const float aLY = Input::Instance().GetGamePad().GetAxisLY();

        float rootMotionValue = 1.0f;

        // 移動入力がなかったらルートモーションの移動値を微量にする
        if (fabsf(aLX) == 0.0f && fabsf(aLY) == 0.0f)
        {
            rootMotionValue = rootMotionValue_;
        }

        owner_->SetRootMotionValue(rootMotionValue);
    }

    // ----- 旋回処理 -----
    void ComboAttack0_3::Turn(const float& elapsedTime)
    {
    }
}

// ----- 樽設置 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void PlacingBarrelState::Initialize()
    {
        // アニメーション再生
        owner_->PlayBlendAnimation(Player::Animation::DownStart, false, firstAnimationSpeed_, firstAnimationStartFrame_);
        owner_->SetTransitionTime(firstAnimationTransitionTime_);
    }

    // ----- 更新 -----
    void PlacingBarrelState::Update(const float& elapsedTime)
    {
        if (owner_->GetAnimationIndex() == static_cast<int>(Player::Animation::DownStart))
        {
            if (owner_->GetAnimationSeconds() > firstAnimationEndFrame_)
            {
                // 樽設置
                DirectX::XMFLOAT3 generatePosition = owner_->GetTransform()->GetPosition();
                generatePosition = generatePosition + XMFloat3Normalize(owner_->GetTransform()->CalcForward()) * generatePosition_;
                Barrel* barrel = new Barrel(generatePosition);

                // 設置効果音を再生
                AudioManager::Instance().PlaySE(SE::Put);

                owner_->PlayBlendAnimation(Player::Animation::DownEnd, false, secondAnimationSpeed_, secondAnimationStartFrame_);
                owner_->SetTransitionTime(secondAniamtionTransitionTime_);
            }
        }

        if(owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void PlacingBarrelState::Finalize()
    {
    }

    // ----- ImGui用 -----
    void PlacingBarrelState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragFloat("generatePosition_", &generatePosition_, 0.01f);
            ImGui::DragFloat("FirstAnimationStartFrame", &firstAnimationStartFrame_, 0.01f);
            ImGui::DragFloat("FirstAnimationEndFrame", &firstAnimationEndFrame_, 0.01f);
            ImGui::DragFloat("FirstAnimationSpeed", &firstAnimationSpeed_, 0.01f);
            ImGui::DragFloat("FirstAnimationTransitionTime", &firstAnimationTransitionTime_, 0.01f);
            ImGui::DragFloat("SecondAnimationStartFrame", &secondAnimationStartFrame_, 0.01f);
            ImGui::DragFloat("SecondAnimationSpeed", &secondAnimationSpeed_, 0.01f);
            ImGui::DragFloat("SecondAniamtionTransitionTime", &secondAniamtionTransitionTime_, 0.01f);            

            ImGui::TreePop();
        }
    }
}

// ----- 兜割り -----
namespace PlayerState
{
    // ----- 初期化 -----
    void HelmbreakerState::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        // アニメーション再生
        owner_->PlayBlendAnimation(Player::Animation::Attack4_0, false, 1.0f, firstAnimationStartFrame_);
        owner_->SetTransitionTime(0.2f);

        // 兜割りカメラ使用する
        Camera::Instance().UseHelmbreakerCamera();

        hitCounter_ = 0;
    }

    // ----- 更新 -----
    void HelmbreakerState::Update(const float& elapsedTime)
    {
        // 攻撃判定更新
        UpdateAttackJudgment();


        // ルートモーションの設定
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotionを使用する
            owner_->SetUseRootMotion(true);
            owner_->SetRootMotionValue(1.0f);
        }

        if (owner_->GetAnimationSeconds() > 0.4f)
        {
            owner_->SetRootMotionValue(rootMotionMoveValue_);
        }

        if (owner_->GetAnimationSeconds() > firstAnimationEndFrame_ && owner_->GetAnimationIndex() == static_cast<int>(Player::Animation::Attack4_0))
        {
            owner_->PlayBlendAnimation(Player::Animation::Attack4_2, false, secondAnimationSpeed_);
            owner_->SetTransitionTime(transitionTime_);
            owner_->SetUseRootMotion(false);

            startPositionY_ = owner_->GetTransform()->GetPositionY();

            // 剣の色を無くす
            owner_->SetSwordSpirit(0.0f);
            owner_->SetSwordColor({});
        }

        if (owner_->GetAnimationIndex() == static_cast<int>(Player::Animation::Attack4_2))
        {
            if (owner_->GetIsBlendAnimation())
            {
                owner_->GetTransform()->AddPositionY(moveValueY_ * elapsedTime);
                startPositionY_ = owner_->GetTransform()->GetPositionY();
            }

            if (owner_->GetAnimationSeconds() <= totalMoveFrame_)
            {
                const float positionY = Easing::InSine(owner_->GetAnimationSeconds(), totalMoveFrame_, 0.0f, startPositionY_);
                owner_->GetTransform()->SetPositionY(positionY);
            }
            else
            {
                owner_->GetTransform()->SetPositionY(0.0f);
            }
        }

        // アニメーション速度調整
        UpdateAnimationSpeed();

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

    }

    // ----- 終了化 -----
    void HelmbreakerState::Finalize()
    {
        owner_->GetTransform()->SetPositionY(0.0f);

        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui用 -----
    void HelmbreakerState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragInt("HitCounter", &hitCounter_);
            ImGui::DragInt("MaxHit", &maxHitNum_);

            ImGui::DragFloat("FirstAnimationStartFrame", &firstAnimationStartFrame_, 0.01f);
            ImGui::DragFloat("FirstAnimationEndFrame", &firstAnimationEndFrame_, 0.01f);
            ImGui::DragFloat("FirstAnimationSpeed", &firstAnimationSpeed_, 0.01f);

            ImGui::DragFloat("SecondAnimationSpeed", &secondAnimationSpeed_, 0.01f);
            
            ImGui::DragFloat("TransitionTime", &transitionTime_, 0.01f);
            ImGui::DragFloat("RootMotionMoveValue", &rootMotionMoveValue_, 0.01f);

            ImGui::DragFloat("MoveValueY", &moveValueY_, 0.01f);

            ImGui::TreePop();
        }
    }

    // ----- 攻撃判定更新 -----
    void HelmbreakerState::UpdateAttackJudgment()
    {
        const Player::Animation currentAnimation = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        // 上昇中の攻撃判定
        if (currentAnimation == Player::Animation::Attack4_0)
        {
            // 既に攻撃が当たってる
            if (owner_->GetIsAttackHit()) return;

            owner_->SetIsAttackValid(true);
        }
        // 降下時の攻撃判定設定
        else
        {
            // 攻撃判定有効時間を過ぎている
            if (owner_->GetAnimationSeconds() >= 0.2f)
            {
                owner_->SetIsAttackHit(true);
                owner_->SetIsAttackValid(false);
                return;
            }

            // ヒット回数上限
            if (hitCounter_ > maxHitNum_) return;

            if (owner_->GetIsAttackHit())
            {
                owner_->SetIsAttackHit(false);
                owner_->SetIsAttackValid(true);

                ++hitCounter_;
            }
        }
    }

    // ----- アニメーション速度調整 -----
    void HelmbreakerState::UpdateAnimationSpeed()
    {
        const Player::Animation curretAnimationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());
        const float animationSeconds = owner_->GetAnimationSeconds();
        float animationSpeed = 1.0f;

        if (curretAnimationIndex == Player::Animation::Attack4_0)
        {
            if (animationSeconds > 0.4f) animationSpeed = firstAnimationSpeed_;
        }
        else if (curretAnimationIndex == Player::Animation::Attack4_2)
        {
            if (animationSeconds < 0.3f) animationSpeed = secondAnimationSpeed_;
        }

        owner_->SetAnimationSpeed(animationSpeed);
    }
}

// ----- チャージ攻撃 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void ChargeAttackState::Initialize()
    {
        // アニメーション再生
        PlayAnimation();        

        currentCharge_ = 0;
        chargeTimer_ = 0.5f;

        isOutlineActive_ = false;
    }

    // ----- 更新 -----
    void ChargeAttackState::Update(const float& elapsedTime)
    {
        const Player::Animation curretAnimation = static_cast<Player::Animation>(owner_->GetAnimationIndex());
        const bool isChargeButtonPressed = Input::Instance().GetGamePad().GetButton() & GamePad::BTN_Y;

        switch (curretAnimation)
        {
        case Player::Animation::ChargeStart:
#pragma region ---------- ChargeStart ----------
            // チャージループアニメーションを再生する
            if (owner_->IsPlayAnimation() == false) owner_->PlayAnimation(Player::Animation::ChargeLoop, true);

#pragma endregion ---------- ChargeStart ----------
            break;
        case Player::Animation::ChargeLoop:
#pragma region ---------- ChargeLoop ----------
            // ボタンを押していなかった場合攻撃に移る
            if (isChargeButtonPressed == false)
            {
                owner_->PlayBlendAnimation(Player::Animation::AttackRush1, false, firstAttackAnimationSpeed_, firstAttackAnimationStartFrame_);
                owner_->SetTransitionTime(transitionChargeLoop_);

                owner_->SetIsDrawSwordTrail(true);

                break;
            }

            // チャージ処理
            chargeTimer_ += chargeSpeed_ * elapsedTime;

#pragma endregion ---------- ChargeLoop ----------
            break;
        case Player::Animation::AttackRush1:
#pragma region ---------- AttackRush1 ----------
            if (owner_->GetAnimationSeconds() > firstAttackAnimationEndFrame_)
            {
                // ここで攻撃は終了する
                if (currentCharge_ < 2)
                {
                    owner_->ChangeState(Player::STATE::Idle);
                    return;
                }
                // コンボが続く
                else
                {
                    owner_->PlayBlendAnimation(Player::Animation::CounterAttack1, false, secondAttackAnimationSpeed_, secondAttackAnimationStartFrame_);
                    owner_->SetTransitionTime(transitionFirstAttack_);
                }
            }

#pragma endregion ---------- AttackRush1 ----------
            break;
        case Player::Animation::CounterAttack1:
#pragma region ---------- CounterAttack1 ----------
            if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
            {
                owner_->SetUseRootMotion(true);
                owner_->SetRootMotionValue(1.0f);
            }

            if (owner_->GetAnimationSeconds() > secondAttackAnimationEndFrame_)
            {
                // ここで攻撃は終了する
                if (currentCharge_ < 3)
                {
                    owner_->ChangeState(Player::STATE::Idle);
                    return;
                }
                // コンボが続く
                else
                {
                    owner_->PlayBlendAnimation(Player::Animation::Attack4_0, false, riseAnimationSpeed_, riseAnimationStartFrame_);
                    owner_->SetTransitionTime(transitionSecondAttack_);

                    owner_->SetUseRootMotion(false);
                }
            }

#pragma endregion ---------- CounterAttack1 ----------
            break;
        case Player::Animation::Attack4_0:

            if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
            {
                owner_->SetUseRootMotion(true);
                owner_->SetRootMotionValue(riseAnimationMoveValue_);
            }

            if (owner_->GetAnimationSeconds() > riseAnimationEndFrame_)
            {
                owner_->PlayBlendAnimation(Player::Animation::Attack4_2, false, thirdAttackAnimationSpeed_, thirdAttackAnimationStartFrame_);
                owner_->SetTransitionTime(transitionRise_);

                owner_->SetUseRootMotion(false);

                startPositionY_ = owner_->GetTransform()->GetPositionY();
            }

            break;
        case Player::Animation::Attack4_2:
#pragma region ---------- Attack4_2 ----------
        {
            if (owner_->GetIsBlendAnimation() == false)
            {
                if (owner_->GetAnimationSeconds() <= 0.2f)
                {
                    const float positionY = Easing::InSine(owner_->GetAnimationSeconds(), 0.2f, 0.0f, startPositionY_);
                    owner_->GetTransform()->SetPositionY(positionY);
                }
                else
                {
                    owner_->GetTransform()->SetPositionY(0.0f);
                }
            }


            if (owner_->IsPlayAnimation() == false)
            {
                owner_->ChangeState(Player::STATE::Idle);
                return;
            }
        }
#pragma endregion ---------- Attack4_2 ----------
            break;
        }

        if (chargeTimer_ >= maxChargeTime_)
        {
            if (currentCharge_ < maxChargeNum_)
            {
                currentChargeColor_ = chargeColor_[currentCharge_];

                owner_->SetOutlineColor(currentChargeColor_);
                owner_->SetWeaponOutlineColor(currentChargeColor_);

                // コントローラー振動させる
                Input::Instance().GetGamePad().Vibration(vibrationTime_, vibrationVolume_[currentCharge_].x, vibrationVolume_[currentCharge_].y);

                // エフェクトを再生する
                const DirectX::XMFLOAT3 emitterPosition = owner_->GetJointPosition("spine_02");
                chargeEffect_ = EffectManager::Instance().GetEffect("Charge0")->Play(emitterPosition, chargeEffectSize_, chargeEffectSpeed_);
                EffectManager::Instance().GetEffect("Charge0")->SetColor(chargeEffect_, chargeColor_[currentCharge_]);

                // アウトラインを使用する
                if (isOutlineActive_ == false)
                {
                    owner_->SetIsOutlineActive(true);
                    isOutlineActive_ = true;
                }

                ++currentCharge_;
            }

            chargeTimer_ = 0.0f;
        }
    }

    // ----- 終了化 -----
    void ChargeAttackState::Finalize()
    {
        // アウトラインの使用を終了
        owner_->SetIsOutlineActive(false);

        owner_->SetIsDrawSwordTrail(false);

        owner_->SetUseRootMotion(false);
        owner_->SetRootMotionValue(1.0f);
    }

    // ----- ImGui用 -----
    void ChargeAttackState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- ChargeEffect ----------"))
            {
                ImGui::DragFloat("Size", &chargeEffectSize_, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Speed", &chargeEffectSpeed_, 0.01f, 0.0f, 10.0f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- GamePadVibration ----------"))
            {
                ImGui::DragFloat("VibrationTime", &vibrationTime_, 0.01f, 0.0f, 10.0f);

                ImGui::DragFloat2("Volume0", &vibrationVolume_[0].x, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat2("Volume1", &vibrationVolume_[1].x, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat2("Volume2", &vibrationVolume_[2].x, 0.01f, 0.0f, 1.0f);

                

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Outline ----------"))
            {
                ImGui::DragInt("CurrentCharge", &currentCharge_);
                ImGui::DragInt("MaxCharge", &maxChargeNum_);
                ImGui::DragFloat("ChargeTimer", &chargeTimer_);
                ImGui::DragFloat("MaxChargeTime", &maxChargeTime_, 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat("ChargeSpeed", &chargeSpeed_, 0.01f, 0.0f, 10.0f);

                ImGui::ColorEdit4("CurrentColor", &currentChargeColor_.x);
                ImGui::ColorEdit4("Charge0Color", &chargeColor_[0].x);
                ImGui::ColorEdit4("Charge1Color", &chargeColor_[1].x);
                ImGui::ColorEdit4("Charge2Color", &chargeColor_[2].x);
                                                               
                ImGui::TreePop();
            }


            if (ImGui::TreeNodeEx("---------- ChargeStart ----------"))
            {
                ImGui::DragFloat("AnimationSpeed", &chargeStartAnimationSpeed_, 0.01f, 0.0f, 2.0f);
                ImGui::DragFloat("StartFrame", &chargeStartAnimationStartFrame_, 0.01f, 0.0f, 2.0f);

                ImGui::DragFloat("TransitionIdle", &transitionIdle_, 0.01f, 0.0f, 1.0f);

                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("---------- FirstAttack ----------"))
            {
                ImGui::DragFloat("AnimationSpeed", &firstAttackAnimationSpeed_, 0.01f);
                ImGui::DragFloat("StartFrame", &firstAttackAnimationStartFrame_, 0.01f);
                ImGui::DragFloat("EndFrame", &firstAttackAnimationEndFrame_, 0.01f);
                ImGui::DragFloat("TransitionChargeLoop", &transitionChargeLoop_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- SecondAttack ----------"))
            {
                ImGui::DragFloat("AnimationSpeed", &secondAttackAnimationSpeed_, 0.01f);
                ImGui::DragFloat("StartFrame", &secondAttackAnimationStartFrame_, 0.01f);
                ImGui::DragFloat("EndFrame", &secondAttackAnimationEndFrame_, 0.01f);
                ImGui::DragFloat("TransitionFirstAttack", &transitionFirstAttack_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- ThirdAttack ----------"))
            {
                ImGui::DragFloat("RiseAnimationSpeed", &riseAnimationSpeed_, 0.01f);
                ImGui::DragFloat("RiseAnimationStartFrame", &riseAnimationStartFrame_, 0.01f);
                ImGui::DragFloat("RiseAnimationEndFrame", &riseAnimationEndFrame_, 0.01f);
                ImGui::DragFloat("RiseAnimationMoveValue", &riseAnimationMoveValue_, 0.01f);
                ImGui::DragFloat("TransitionSecondAttack", &transitionSecondAttack_, 0.01f);

                ImGui::DragFloat("AnimationSpeed", &thirdAttackAnimationSpeed_, 0.01f);
                ImGui::DragFloat("StartFrame", &thirdAttackAnimationStartFrame_, 0.01f);
                ImGui::DragFloat("TransitionRise", &transitionRise_, 0.01f);

                ImGui::TreePop();
            }


            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void ChargeAttackState::PlayAnimation()
    {
        owner_->PlayBlendAnimation(Player::Animation::ChargeStart, false, chargeStartAnimationSpeed_, chargeStartAnimationStartFrame_);
        owner_->SetTransitionTime(transitionIdle_);

    }
}

#pragma endregion ---------- SwordMan ----------

#pragma region ---------- Mage ----------
// ----- 待機 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void MageIdleState::Initialize()
    {
        // フラグをリセット
        owner_->ResetFlags();

        // アニメーション再生
        owner_->PlayBlendAnimation(Player::Animation::MageIdle, true);
    }

    // ----- 更新 -----
    void MageIdleState::Update(const float& elapsedTime)
    {
        // 先行入力判定
        if (CheckNextInput()) return;
    }

    // ----- 終了化 -----
    void MageIdleState::Finalize()
    {
    }

    // ----- ImGui用 -----
    void MageIdleState::DrawDebug()
    {
    }

    // ----- 先行入力判定 -----
    const bool MageIdleState::CheckNextInput()
    {
        // ----- 回避に遷移 -----
        if (owner_->IsDodgeKeyDown())
        {
            owner_->ChangeState(Player::STATE::MageDodge);
            return true;
        }

        if (owner_->IsMageCounterStanceKey())
        {
            owner_->ChangeState(Player::STATE::MageCounter);
            return true;
        }

        // ----- 攻撃に遷移 -----
        if (owner_->IsMageAttackKeyDown())
        {
            owner_->ChangeState(Player::STATE::MageRainAttack);
            //owner_->ChangeState(Player::STATE::MageAttack);
            return true;
        }

        // ----- 攻撃1_0に遷移 -----
        if (owner_->IsMageAttack1KeyDown())
        {
            owner_->ChangeState(Player::STATE::MageAttack1_0);
            return true;
        }

        const float aLX = Input::Instance().GetGamePad().GetAxisLX();
        const float aLY = Input::Instance().GetGamePad().GetAxisLY();
        // ----- 移動入力があれば走りに遷移 -----
        if (aLX != 0.0f || aLY != 0.0f)
        {
            owner_->ChangeState(Player::STATE::MageRun);
            return true;
        }

        return false;
    }
}

// ----- 走り -----
namespace PlayerState
{
    // ----- コンストラクタ -----
    MageRunState::MageRunState(Player* player)
        : State(player, "MageRunState")
    {
        footPrints_.SetEmitParameter("FootPrints");
    }

    // ----- 初期化 -----
    void MageRunState::Initialize()
    {
        // フラグをリセット
        owner_->ResetFlags();

        // アニメーション再生
        PlayAnimation();

        // 最大速度を設定
        owner_->SetMaxSpeed(5.0f);

        // 変数初期化
        isLeftFootPrintEffectActive_ = true;
    }

    // ----- 更新 -----
    void MageRunState::Update(const float& elapsedTime)
    {
        // 先行入力判定
        if (CheckNextInput()) return;

        // 旋回
        owner_->Turn(elapsedTime);

        // ダッシュ処理
        UpdateDash(elapsedTime);

        // エフェクト処理
        GenerateFootPrintsEffect();
    }

    // ----- 終了化 -----
    void MageRunState::Finalize()
    {
        owner_->SetMoveDirection({});
        owner_->SetVelocity({});
        owner_->SetIsDash(false);
    }

    // ----- ImGui用 -----
    void MageRunState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- Animation ----------"))
            {
                ImGui::DragFloat("TransitionDodge", &transitionDodge_, 0.01f);
                ImGui::DragFloat("TransitionAttack1_0", &transitionAttack1_0_, 0.01f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void MageRunState::PlayAnimation()
    {
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        float transitionTime = 0.1f;

        // 回避からの遷移
        if (animationIndex == Player::Animation::MageRollFront || animationIndex == Player::Animation::MageRollBack ||
            animationIndex == Player::Animation::MageRollRight || animationIndex == Player::Animation::MageRollLeft)
        {
            transitionTime = transitionDodge_;
        }
        // 攻撃1_0からの遷移
        else if (animationIndex == Player::Animation::MageAttack1_0)
        {
            transitionTime = transitionAttack1_0_;
        }
        else if (animationIndex == Player::Animation::MageAttack1_1)
        {
            transitionTime = transitionAttack1_1_;
        }
        owner_->SetTransitionTime(transitionTime);

        owner_->PlayBlendAnimation(Player::Animation::MageRun, true);
    }

    // ----- 先行入力判定 -----
    const bool MageRunState::CheckNextInput()
    {
        // ----- 回避に遷移 -----
        if (owner_->IsDodgeKeyDown())
        {
            // ダッシュ中
            if (owner_->IsDashKey())
            {
                owner_->ChangeState(Player::STATE::MageDashDodge);
            }
            else
            {
                owner_->ChangeState(Player::STATE::MageDodge);
            }
            return true;
        }

        if (owner_->IsMageCounterStanceKey())
        {
            owner_->ChangeState(Player::STATE::MageCounter);
            return true;
        }

        // ----- 攻撃に遷移 -----
        if (owner_->IsMageAttackKeyDown())
        {
            owner_->ChangeState(Player::STATE::MageRainAttack);
            //owner_->ChangeState(Player::STATE::MageAttack);
            return true;
        }

        // ----- 攻撃1_0に遷移 -----
        if (owner_->IsMageAttack1KeyDown())
        {
            owner_->ChangeState(Player::STATE::MageAttack1_0);
            return true;
        }

        const float aLX = Input::Instance().GetGamePad().GetAxisLX();
        const float aLY = Input::Instance().GetGamePad().GetAxisLY();
        // ----- 移動入力が無ければ待機に遷移 -----
        if (aLX == 0.0f && aLY == 0.0f)
        {
            owner_->ChangeState(Player::STATE::MageIdle);
            return true;
        }

        return false;
    }

    // ----- ダッシュ処理 -----
    void MageRunState::UpdateDash(const float& elapsedTime)
    {
        const bool isDashKey = owner_->IsDashKey();

        // スタミナが底をついた場合速度を遅くする
        if (owner_->GetIsStaminaDepleted())
        {
            owner_->SetAnimationSpeed(0.8f);

            // 最大速度を設定
            owner_->SetMaxSpeed(3.0f);

            owner_->UseDashStamina(elapsedTime);

            // ダッシュしているか設定
            owner_->SetIsDash(isDashKey);

            return;
        }

        // ダッシュの処理
        if (isDashKey)
        {
            owner_->SetAnimationSpeed(owner_->GetDashAnimationSpeed());

            // 最大速度を設定
            owner_->SetMaxSpeed(owner_->GetDashSpeed());

            owner_->SetIsDash(true);

            owner_->UseDashStamina(elapsedTime);
        }
        else
        {
            owner_->SetAnimationSpeed(1.0f);

            // 最大速度を設定
            owner_->SetMaxSpeed(5.0f);

            owner_->SetIsDash(false);
        }
    }

    // ----- 足跡のエフェクト生成 -----
    void MageRunState::GenerateFootPrintsEffect()
    {
        const float currentAnimationSeconds = owner_->GetAnimationSeconds();

        // 両足エフェクトを出し終わっている
        if (isLeftFootPrintEffectActive_ && currentAnimationSeconds >= rightFootPrintFrame_) return;

        // 左足にエフェクトを生成
        if (isLeftFootPrintEffectActive_ && currentAnimationSeconds >= leftFootPrintFrame_)
        {
            footPrints_.SetEmitPosition(owner_->GetJointPosition("ball_l"));
            footPrints_.EmitParticle();

            isLeftFootPrintEffectActive_ = false;
            return;
        }

        // 右足にエフェクトを生成
        if (currentAnimationSeconds >= rightFootPrintFrame_)
        {
            footPrints_.SetEmitPosition(owner_->GetJointPosition("ball_r"));
            footPrints_.EmitParticle();

            isLeftFootPrintEffectActive_ = true;
        }
    }
}

// ----- 回避 -----
namespace PlayerState
{
#define USE_DODGE_EFFECT 0

    // ----- コンストラクタ -----
    MageDodgeState::MageDodgeState(Player* player)
        : State(player, "MageDodgeState")
    {
#if USE_DODGE_EFFECT
        dodgePrintsEffectEmitter_.SetEmitParameter("DodgePrints");
#endif
    }

    // ----- 初期化 -----
    void MageDodgeState::Initialize()
    {
        // フラグをリセット
        owner_->ResetFlags();

        // アニメーション再生
        PlayAnimation();

        // 無敵状態にする
        owner_->SetIsInvincible(true);

        // スタミナ消費
        owner_->UseDodgeStamina();

        // 変数初期化
        isRotating_         = false;
        isDodgeFirstTime_   = false;

#if USE_DODGE_EFFECT
        isDodgePrintsEffectCreated_ = false;
#endif
    }

    // ----- 更新 -----
    void MageDodgeState::Update(const float& elapsedTime)
    {
        // ルートモーションの設定
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            owner_->SetUseRootMotion(true);
            owner_->SetRootMotionValue(rootMotionMoveValue_);
        }

        // 先行入力処理
        if (CheckNextInput()) return;

        // 旋回処理
        Turn(elapsedTime);

        // アニメーション速度更新
        UpdateAnimationSpeed(elapsedTime);

        // 無敵判定更新
        if (owner_->GetIsInvincible() && owner_->GetAnimationSeconds() > invincibleFrame_)
        {
            owner_->SetIsInvincible(false);
        }

#if USE_DODGE_EFFECT
        if (owner_->GetAnimationSeconds() > dodgePrintsEffectCreateFrame_ && isDodgePrintsEffectCreated_ == false)
        {
            dodgePrintsEffectEmitter_.SetEmitParameter("DodgePrints");
            
            DirectX::XMFLOAT3 emitPosition = owner_->GetTransform()->GetPosition();
            emitPosition.y = 0.1f;
            dodgePrintsEffectEmitter_.SetEmitPosition(emitPosition);
            dodgePrintsEffectEmitter_.EmitParticle();

            isDodgePrintsEffectCreated_ = true;
        }
#endif

        // 回避終了判定
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::MageIdle);
            return;
        }
    }

    // ----- 終了化 -----
    void MageDodgeState::Finalize()
    {
        // ルートモーションリセット
        owner_->SetUseRootMotion(false);
        owner_->SetRootMotionValue(1.0f);

        // 変数リセット
        isDodgeFirstTime_ = true;
    }

    // ----- ImGui用 -----
    void MageDodgeState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
#if USE_DODGE_EFFECT
            ImGui::DragFloat("dodgePrintsEffectCreateFrame_", &dodgePrintsEffectCreateFrame_, 0.01f);
#endif

            if (ImGui::TreeNodeEx("---------- Animation ----------"))
            {
                ImGui::DragFloat("PlayAnimationSpeed", &playAnimationSpeed_, 0.01f);
                ImGui::DragFloat("ChangeAnimationSpeed", &changeAnimationSpeed_, 0.01f);

                ImGui::DragFloat("AniamtionStartFrame", &animationStartFrame_, 0.01f);
                ImGui::DragFloat("TransitionDodge", &transitionDodge_, 0.01f);

                ImGui::Checkbox("IsDodgeFirstTime", &isDodgeFirstTime_);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- RootMotion ----------"))
            {
                ImGui::DragFloat("RootMotionMoveValue", &rootMotionMoveValue_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- NextInput ----------"))
            {
                ImGui::DragFloat("NextInputStartFrame", &nextInputStartFrame_, 0.01f);
                
                ImGui::DragFloat("DodgeStateChangeFrame", &dodgeStateChangeFrame_, 0.01f);
                ImGui::DragFloat("RunStateChangeFrame", &runStateChangeFrame_, 0.01f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void MageDodgeState::PlayAnimation()
    {
        // --------------------------------------------------
        //  回避を連続して出している場合
        // --------------------------------------------------
        if (isDodgeFirstTime_ == false)
        {
            // 前方向のアニメーション再生
            owner_->PlayBlendAnimation(Player::Animation::MageRollFront, false, playAnimationSpeed_, animationStartFrame_);
            owner_->SetTransitionTime(transitionDodge_);
            return;
        }

        // ------------------------------------------------------------
        // プレイヤーの姿勢に合わせてアニメーションの方向を設定する
        // ------------------------------------------------------------
        const float aLX = Input::Instance().GetGamePad().GetAxisLX();
        const float aLY = Input::Instance().GetGamePad().GetAxisLY();
        Player::Animation animationIndex = Player::Animation::MageRollFront;
        // ----- 入力値がある場合 -----
        if (aLX != 0.0f || aLY != 0.0f)
        {
            // カメラから見たスティックの入力方向を算出
            DirectX::XMFLOAT2 cameraInput = Camera::Instance().ConvertTo2DVectorFromCamera(DirectX::XMFLOAT2(aLX, aLY));
            DirectX::XMFLOAT2 ownerFront = XMFloat2Normalize({ owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z });

            // 内積で角度を算出
            float dot = std::clamp(XMFloat2Dot(cameraInput, ownerFront), -1.0f, 1.0f);
            float angle = acosf(dot);

            // 左右判定
            float cross = XMFloat2Cross(cameraInput, ownerFront);

            // ----- 90度よりも小さければ、前,右,左 の三択 -----
            if (angle < DirectX::XM_PIDIV2)
            {
                // ----- 回転角が４５度よりも小さければ 前方向 -----
                if (angle < DirectX::XM_PIDIV4)
                {
                    animationIndex = Player::Animation::MageRollFront;
                }
                else
                {
                    // ----- 右方向 -----
                    if (cross < 0)  animationIndex = Player::Animation::MageRollRight;
                    // ----- 左方向 -----
                    else            animationIndex = Player::Animation::MageRollLeft;
                }
            }
            // ----- 90度よりも大きければ、後,右,左 の三択 -----
            else
            {
                // ----- 135度よりも大きければ 後方向 -----
                if (angle > DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4)
                {
                    animationIndex = Player::Animation::MageRollBack;
                }
                else
                {
                    // ----- 右方向 -----
                    if (cross < 0)  animationIndex = Player::Animation::MageRollRight;
                    // ----- 左方向 -----
                    else            animationIndex = Player::Animation::MageRollLeft;
                }
            }
        }
        // ----- 入力値がない場合、前方向のアニメーションを設定 -----
        else
        {
            animationIndex = Player::Animation::MageRollFront;
        }
        owner_->PlayBlendAnimation(animationIndex, false, playAnimationSpeed_, animationStartFrame_);

        owner_->SetTransitionTime(0.05f);
    }

    // ----- アニメーション速度更新 -----
    void MageDodgeState::UpdateAnimationSpeed(const float& elapsedTime)
    {
        if (owner_->GetAnimationSeconds() > animationSpeedChangeFrame_)
        {
            owner_->SetAnimationSpeed(changeAnimationSpeed_);
        }
    }

    // ----- 旋回処理 -----
    void MageDodgeState::Turn(const float& elapsedTime)
    {
        // 旋回処理をしない
        if (isRotating_ == false) return;
        // 入力値がないので回転する必要なし
        if (isInputStick_ == false) return;

        DirectX::XMFLOAT2 playerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
        playerForward = XMFloat2Normalize(playerForward);

        float dot = std::clamp(XMFloat2Dot(inputDirection_, playerForward), -1.0f, 1.0f);
        float angle = acosf(dot);

        if (angle < DirectX::XMConvertToRadians(1))
        {
            isRotating_ = false;
            return;
        }

        float cross = XMFloat2Cross(inputDirection_, playerForward);

        const float speed = owner_->GetRotateSpeed() * elapsedTime;
        float rotateY = angle * speed;

        if (cross > 0)
        {
            owner_->GetTransform()->AddRotationY(-rotateY);
        }
        else
        {
            owner_->GetTransform()->AddRotationY(rotateY);
        }
    }

    // ----- このステートをリセット(初期化)する -----
    void MageDodgeState::ResetState()
    {
        // 旋回処理を行う
        isRotating_ = true;

        // アニメーション再生
        PlayAnimation();

        // フラグをリセット
        owner_->ResetFlags();

        // スタミナ消費
        owner_->UseDodgeStamina();

        // ルートモーションリセット
        owner_->SetUseRootMotion(false);
    }

    // ----- 先行入力判定 -----
    const bool MageDodgeState::CheckNextInput()
    {
        const float currentAnimationFrame = owner_->GetAnimationSeconds();

        // ----------------------------------------
        //              先行入力受付
        // ----------------------------------------
        if (currentAnimationFrame > nextInputStartFrame_)
        {
            // ----- 回避入力受付 -----
            if (owner_->IsDodgeKeyDown() && owner_->GetAnimationIndex() != static_cast<int>(Player::Animation::MageRollBack))
            {
                owner_->SetNextInput(Player::NextInput::Dodge);

                const float aLX = Input::Instance().GetGamePad().GetAxisLX();
                const float aLY = Input::Instance().GetGamePad().GetAxisLY();
                // 移動入力がある
                if (aLX != 0.0f || aLY != 0.0f)
                {
                    inputDirection_ = Camera::Instance().ConvertTo2DVectorFromCamera(DirectX::XMFLOAT2(aLX, aLY));
                    inputDirection_ = XMFloat2Normalize(inputDirection_);

                    isInputStick_ = true;
                }
                // 移動入力なし
                else
                {
                    isInputStick_ = false;
                }
            }
        }

        // ----------------------------------------
        //         先行入力によるステート変更
        // ----------------------------------------

        // ----- 回避へ遷移 -----
        if (owner_->GetNextInput() == Player::NextInput::Dodge)
        {
            if (currentAnimationFrame > dodgeStateChangeFrame_)
            {
                // 回避は現在と同じステートの為、リセットを呼ぶ
                ResetState();
                return true;
            }
        }
        // ----- 走りへ遷移 -----
        else if (currentAnimationFrame > runStateChangeFrame_)
        {
            const float aLX = Input::Instance().GetGamePad().GetAxisLX();
            const float aLY = Input::Instance().GetGamePad().GetAxisLY();

            // 移動値があれば遷移する
            if (aLX != 0.0f || aLY != 0.0f)
            {
                owner_->ChangeState(Player::STATE::MageRun);
                return true;
            }
        }

        return false;
    }
}

// ----- ダッシュ回避 -----
namespace PlayerState
{
    // ----- コンストラクタ -----
    MageDashDodgeState::MageDashDodgeState(Player* player)
        : State(player, "MageDashDodgeState")
    {
        computeParticleEmitter_.SetEmitParameter("DashDodge");
    }

    // ----- 初期化 -----
    void MageDashDodgeState::Initialize()
    {
        // フラグをリセット
        owner_->ResetFlags();

        // アニメーション再生
        PlayAnimation();

        // ラジアルブラーを使用する
        PostProcess::Instance().SetUseRadialBlur(true);
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 5;
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = 0.0f;

        // 変数初期化
        isGamePadVibration_ = false;
    }

    // ----- 更新 -----
    void MageDashDodgeState::Update(const float& elapsedTime)
    {
        // 先行入力判定
        CheckNextInput();

        // ルートモーションを使用する
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            owner_->SetUseRootMotion(true);
            owner_->SetRootMotionValue(rootMotionMoveValue_);
        }
        
        // パーティクル生成
        computeParticleEmitter_.SetEmitPosition(owner_->GetJointPosition("foot_l"));
        computeParticleEmitter_.EmitParticle();
        computeParticleEmitter_.SetEmitPosition(owner_->GetJointPosition("foot_r"));
        computeParticleEmitter_.EmitParticle();

        // ラジアルブラー更新
        UpdateRadialBlur(elapsedTime);

        // コントローラー振動
        if (isGamePadVibration_ == false && owner_->GetAnimationSeconds() >= gamePadVibrationFrame_)
        {
            Input::Instance().GetGamePad().Vibration(gamePadVibrationTime_, gamePadVibrationPower_.x, gamePadVibrationPower_.y);
            isGamePadVibration_ = true;
        }

        // 回避終了チェック
        if (owner_->IsPlayAnimation() == false)
        {
            if (owner_->GetNextInput() == Player::NextInput::MageAttack)        owner_->ChangeState(Player::STATE::MageAttack);
            else if (owner_->GetNextInput() == Player::NextInput::MageAttack1)  owner_->ChangeState(Player::STATE::MageAttack1_0);
            else if (owner_->GetNextInput() == Player::NextInput::Dodge)        owner_->ChangeState(Player::STATE::MageDodge);
            else                                                                owner_->ChangeState(Player::STATE::MageIdle);

            return;
        }
    }

    // ----- 終了化 -----
    void MageDashDodgeState::Finalize()
    {
        // ルートモーション使用終了
        owner_->SetUseRootMotion(false);
        owner_->SetRootMotionValue(1.0f);

        // ラジアルブラー使用終了
        PostProcess::Instance().SetUseRadialBlur(false);
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 1;
    }

    // ----- ImGui用 -----
    void MageDashDodgeState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragFloat("CheckNextInputFrame", &checkNextInputFrame_, 0.01f);
            if (ImGui::TreeNodeEx("---------- Animation ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("AnimationStartFrame", &animationStartFrame_, 0.01f);
                ImGui::DragFloat("PlayAnimationSpeed", &playAnimationSpeed_, 0.01f);
                ImGui::DragFloat("RootMotionMoveValue", &rootMotionMoveValue_, 0.01f);
                ImGui::DragFloat("TransitionRun", &transitionRun_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- RadialBlur ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("RadialBlurStrength", &radialBlurStrength_, 0.01f);
                ImGui::DragFloat("RadialBlurStartFrame", &radialBlurStartFrame_, 0.01f);
                ImGui::DragFloat("RadialBlurEndFrame", &radialBlurEndFrame_, 0.01f);
                ImGui::DragFloat("RadialBlurEndTime", &radialBlurEndTime_, 0.01f);
                
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- GamePadVibration ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat2("Power", &gamePadVibrationPower_.x, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Time", &gamePadVibrationTime_, 0.01f);
                ImGui::DragFloat("Frame", &gamePadVibrationFrame_, 0.01f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void MageDashDodgeState::PlayAnimation()
    {
        owner_->PlayBlendAnimation(Player::Animation::MageDush, false, playAnimationSpeed_, animationStartFrame_);
        owner_->SetTransitionTime(transitionRun_);
    }

    // ----- ラジアルブラー更新 -----
    void MageDashDodgeState::UpdateRadialBlur(const float& elapsedTime)
    {
        // =========================
        //      中心点を決める
        // =========================
        DirectX::XMFLOAT2 center = Sprite::ConvertToScreenPos(owner_->GetTransform()->GetPosition());
        center.x /= SCREEN_WIDTH;
        center.y /= SCREEN_HEIGHT;
        // 中心点を 0.0 ~ 1.0 の間に収める
        center.x = std::clamp(center.x, 0.0f, 1.0f);
        center.y = std::clamp(center.y, 0.0f, 1.0f);
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->uvOffset_ = center;

        // =========================
        //      強度を設定する
        // =========================
        const float currentAnimationSeconds = owner_->GetAnimationSeconds();
        if (currentAnimationSeconds <= radialBlurStartFrame_)
        {
            const float strength = Easing::InSine(currentAnimationSeconds, radialBlurStartFrame_, radialBlurStrength_, 0.0f);
            PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = strength;
        }
        else if(currentAnimationSeconds >= radialBlurEndFrame_)
        {
            const float strength = Easing::InSine(currentAnimationSeconds - radialBlurEndFrame_, radialBlurEndTime_, 0.0f, radialBlurStrength_);
            PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = std::max(strength, 0.0f);
        }
    }

    // ----- 先行入力判定 -----
    void MageDashDodgeState::CheckNextInput()
    {
        if (owner_->GetAnimationSeconds() < checkNextInputFrame_) return;

        // 回避
        if (owner_->IsDodgeKeyDown())
        {
            owner_->SetNextInput(Player::NextInput::Dodge);
        }
        // 攻撃
        if (owner_->IsMageAttackKeyDown())
        {
            owner_->SetNextInput(Player::NextInput::MageAttack);
        }
        // 攻撃1_0
        if (owner_->IsMageAttack1KeyDown())
        {
            owner_->SetNextInput(Player::NextInput::MageAttack1);
        }
    }
}

// ----- ダメージ -----
namespace PlayerState
{
    // ----- 初期化 -----
    void MageDamageState::Initialize()
    {
        // フラグをリセット
        owner_->ResetFlags();

        // アニメーション再生
        owner_->PlayAnimation(Player::Animation::MageDamage, false, playAnimationSpeed_);

        // 無敵状態にする
        owner_->SetIsInvincible(true);

        // AddForceData 初期化
        addForceData_.Initialize(0.1f, 0.3f, 0.5f);
        DirectX::XMFLOAT3 playerPosition = owner_->GetTransform()->GetPosition();
        DirectX::XMFLOAT3 dragonPosition = EnemyManager::Instance().GetEnemy(0)->GetTransform()->GetPosition();
        playerPosition.y = dragonPosition.y = 0.0f;
        addForceDirection_ = XMFloat3Normalize(playerPosition - dragonPosition);

        // 旋回
        Turn();

        // ビネット設定
        SetVignette();

        // 変数初期化
        isFirstAnimation_       = true;
        isCameraShakeActive_    = false;
    }

    // ----- 更新 -----
    void MageDamageState::Update(const float& elapsedTime)
    {
        // アニメーション再生速度更新
        UpdateAnimationSpeed();

        // カメラシェイク処理
        if (isCameraShakeActive_ == false)
        {
            if (owner_->GetAnimationSeconds() > cameraShakeStartFrame_)
            {
                Camera::Instance().ScreenVibrate(cameraShakePower_, cameraShakeTime_);
                isCameraShakeActive_ = true;
            }
        }

        // ビネット更新
        vignetteTimer_ += vignetteFadeOutSpeed_ * elapsedTime;
        vignetteTimer_ = std::min(vignetteTimer_, 1.0f);
        const float maxIntensity = isHighDamage_ ? highDamageMaxIntensity_ : normalDamageMaxIntensity_;
        const float intensity = XMFloatLerp(maxIntensity, 0.0f, vignetteTimer_);
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteIntensity_ = intensity;

        // 移動処理
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(addForceDirection_, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // ----- ダメージアニメーション -----
        if (isFirstAnimation_)
        {
            // 入力があれば起き上がる
            if (owner_->GetAnimationSeconds() > getUpStartFrame_ && owner_->IsGetUpKeyDown())
            {
                owner_->PlayBlendAnimation(Player::Animation::MageGetUp, false);
                owner_->SetTransitionTime(transitionDamage_);
                isFirstAnimation_ = false;
            }

            // アニメーション再生終了
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayBlendAnimation(Player::Animation::MageGetUp, false);
                owner_->SetTransitionTime(transitionDamage_);
                isFirstAnimation_ = false;
            }
        }
        // ----- 起き上がりアニメーション -----
        else
        {
            // 回避に遷移チェック
            if (owner_->GetAnimationIndex() > dodgeStateChangeFrame_)
            {
                if (owner_->IsDodgeKeyDown())
                {
                    owner_->ChangeState(Player::STATE::MageDodge);
                    return;
                }
            }

            if (owner_->GetAnimationSeconds() > getUpEndFrame_)
            {
                owner_->ChangeState(Player::STATE::MageIdle);
                return;
            }
        }
    }

    // ----- 終了化 -----
    void MageDamageState::Finalize()
    {
        // 無敵状態を解除する
        owner_->SetIsInvincible(false);

        // ビネット使用終了
        PostProcess::Instance().SetUseVignette(false);
    }

    // ----- ImGui用 -----
    void MageDamageState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- Animation ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("PlayAnimationSpeed", &playAnimationSpeed_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- CameraShake ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("CameraShakeStartFrame", &cameraShakeStartFrame_, 0.01f);
                ImGui::DragFloat("CameraShakePower", &cameraShakePower_, 0.01f);
                ImGui::DragFloat("CameraShakeTime", &cameraShakeTime_, 0.01f);
                ImGui::Checkbox("IsCameraShakeActive", &isCameraShakeActive_);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- 旋回処理 -----
    void MageDamageState::Turn()
    {
        DirectX::XMFLOAT2 ownerFront = XMFloat2Normalize({ owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z });
        DirectX::XMFLOAT2 addForceDirection = XMFloat2Normalize(DirectX::XMFLOAT2(addForceDirection_.x, addForceDirection_.z) * -1.0f);

        float dot = std::clamp(XMFloat2Dot(addForceDirection, ownerFront), -1.0f, 1.0f);
        float angle = acosf(dot);
        float cross = XMFloat2Cross(addForceDirection, ownerFront);

        if (cross > 0)
        {
            owner_->GetTransform()->SetRotationY(-angle);
        }
        else
        {
            owner_->GetTransform()->SetRotationY(angle);
        }
    }

    // ----- ビネット設定 -----
    void MageDamageState::SetVignette()
    {
        // HighDamageかの判定
        isHighDamage_ = (EnemyManager::Instance().GetEnemy(0)->GetAnimationIndex() == static_cast<int>(Enemy::DragonAnimation::Nova1));

        PostProcess::Instance().SetUseVignette();
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteCenter_ = { 0.5f, 0.5f };
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteSmoothness_ = 2.2f;
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteIntensity_ = isHighDamage_ ? highDamageMaxIntensity_ : normalDamageMaxIntensity_;
        PostProcess::Instance().GetVignetteConstants()->GetData()->vignetteColor_ = isHighDamage_ ? highDamageColor_ : normalDamageColor_;

        vignetteTimer_ = 0.0f;
    }

    // ----- アニメーション再生速度更新 -----
    void MageDamageState::UpdateAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // 一つ目のアニメーション ( 吹き飛ばされ )
        if (isFirstAnimation_)
        {
            if (animationSeconds > 1.2f)
            {// 起き上がるまでの時間を延ばす
                owner_->SetAnimationSpeed(0.4f);
            }
            else if (animationSeconds > 1.0f)
            {
                owner_->SetAnimationSpeed(1.0f);
            }
        }
        // 二つ目のアニメーション ( 起き上がり )
        else
        {
            if (animationSeconds > 1.0f)
            {
                owner_->SetAnimationSpeed(1.5f);
            }
        }
    }
}

// ----- カウンター -----
namespace PlayerState
{
    // ----- コンストラクタ -----
    MageCounterState::MageCounterState(Player* player)
        : State(player, "MageCounterState")
    {
    }

    // ----- 初期化 -----
    void MageCounterState::Initialize()
    {
        // アニメーション再生
        PlayAnimation();

        isCreateMagicCircle_ = false;
    }

    // ----- 更新 -----
    void MageCounterState::Update(const float& elapsedTime)
    {
        // ルートモーションを使用する
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            owner_->SetUseRootMotion(true);
            owner_->SetRootMotionValue(rootMotionMoveValue_);
        }

        // 魔法陣生成
        if (isCreateMagicCircle_ == false && owner_->GetAnimationSeconds() >= magicCircleCreateFrame_)
        {
            const DirectX::XMFLOAT3 staffPosition = owner_->GetStaffJointPosition("joint1");
            const DirectX::XMFLOAT3 playerPosition = owner_->GetTransform()->GetPosition();
            const DirectX::XMFLOAT3 playerForward = owner_->GetTransform()->CalcForward();
            const DirectX::XMFLOAT3 createPosition = playerPosition + XMFloat3Normalize(playerForward) * createForwardLength_;

            MagicCircle* magicCircle = new MagicCircle(staffPosition, createPosition);

            isCreateMagicCircle_ = true;
        }       


        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::MageIdle);
            return;
        }
    }

    // ----- 終了化 -----
    void MageCounterState::Finalize()
    {
        owner_->SetUseRootMotion(false);
        owner_->SetRootMotionValue(1.0f);
    }

    // ----- ImGui用 -----
    void MageCounterState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- Animation ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("TransitionIdle", &transitionIdle_, 0.01f);
                ImGui::DragFloat("RootMotionMoveValue", &rootMotionMoveValue_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- MagicCircle ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("CreateFrame", &magicCircleCreateFrame_, 0.01f);
                ImGui::DragFloat("CreateForwardLength", &createForwardLength_, 0.01f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void MageCounterState::PlayAnimation()
    {
        owner_->PlayBlendAnimation(Player::Animation::MageCounter, false);
        owner_->SetTransitionTime(transitionIdle_);
    }
}

// ----- 雨魔法攻撃 -----
namespace PlayerState
{
    // ----- コンストラクタ -----
    MageRainAttackState::MageRainAttackState(Player* player)
        : State(player, "MageRainAttackState")
    {
        smokeParticleEmitter_.SetEmitParameter("DarkSmoke");
    }

    // ----- 初期化 -----
    void MageRainAttackState::Initialize()
    {
        // フラグをリセット
        owner_->ResetFlags();

        // アニメーション再生
        PlayAnimation();

        // 変数初期化
        isFirstAnimation_ = true;
    }

    // ----- 更新 -----
    void MageRainAttackState::Update(const float& elapsedTime)
    {
        // 煙エフェクト生成
        if (owner_->GetIsBlendAnimation() == false && isFirstAnimation_)
        {
            smokeParticleEmitter_.SetEmitPosition(owner_->GetStaffJointPosition("joint1"));
            smokeParticleEmitter_.EmitParticle();
        }

        // 終了チェック & アニメーション変更チェック
        if (owner_->IsPlayAnimation() == false)
        {
            if (isFirstAnimation_)
            {
                owner_->PlayAnimation(Player::Animation::MageSkillAttack0_1, false);

                // ルートモーション使用
                owner_->SetUseRootMotion(true);

                RainClouds* rainClouds = new RainClouds();

                isFirstAnimation_ = false;
            }
            else
            {
                owner_->ChangeState(Player::STATE::MageIdle);
                return;
            }
        }
    }

    // ----- 終了化 -----
    void MageRainAttackState::Finalize()
    {
        // ルートモーション使用終了
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui用 -----
    void MageRainAttackState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- Animation ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("TransitionIdle", &transitionIdle_, 0.01f);
                ImGui::DragFloat("TransitionRun", &transitionRun_, 0.01f);
                ImGui::DragFloat("TransitionDashDodge", &transitionDashDodge_, 0.01f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void MageRainAttackState::PlayAnimation()
    {
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());
        float transitionTime = 0.1f;
        if (animationIndex == Player::Animation::MageIdle)
        {
            transitionTime = transitionIdle_;
        }
        else if (animationIndex == Player::Animation::MageRun)
        {
            transitionTime = transitionRun_;
        }
        else if (animationIndex == Player::Animation::MageDush)
        {
            transitionTime = transitionDashDodge_;
        }
        owner_->SetTransitionTime(transitionTime);

        owner_->PlayBlendAnimation(Player::Animation::MageSkillAttack0_0, false);
    }
}

// ----- 攻撃 -----
namespace PlayerState
{
    // ----- コンストラクタ -----
    MageAttackState::MageAttackState(Player* player)
        : State(player, "MageAttackState")
    {
        smokeParticleEmitter_.SetEmitParameter("DarkSmoke");
    }

    // ----- 初期化 -----
    void MageAttackState::Initialize()
    {
        // フラグをリセット
        owner_->ResetFlags();

        // アニメーション再生
        PlayAnimation();

        // 変数初期化
        isFirstAnimation_ = true;
        isDarkFireballInitialized_ = false;
    }

    // ----- 更新 -----
    void MageAttackState::Update(const float& elapsedTime)
    {
        if (isDarkFireballInitialized_ == false && isFirstAnimation_ && owner_->GetAnimationSeconds() > 0.6f)
        {
            owner_->SetDarkFireballEmittNum(8);
            isDarkFireballInitialized_ = true;
        }

        // 煙エフェクト生成
        if (owner_->GetIsBlendAnimation() == false && isFirstAnimation_)
        {
            smokeParticleEmitter_.SetEmitPosition(owner_->GetStaffJointPosition("joint1"));
            smokeParticleEmitter_.EmitParticle();
        }

        // 終了チェック & アニメーション変更チェック
        if (owner_->IsPlayAnimation() == false)
        {
            if (isFirstAnimation_)
            {
                owner_->PlayAnimation(Player::Animation::MageSkillAttack0_1, false);

                // ルートモーション使用
                owner_->SetUseRootMotion(true);

                isFirstAnimation_ = false;
            }
            else
            {
                owner_->ChangeState(Player::STATE::MageIdle);
                return;
            }
        }
    }

    // ----- 終了化 -----
    void MageAttackState::Finalize()
    {
        // ルートモーション使用終了
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui用 -----
    void MageAttackState::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- Animation ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("TransitionIdle", &transitionIdle_, 0.01f);
                ImGui::DragFloat("TransitionRun", &transitionRun_, 0.01f);
                ImGui::DragFloat("TransitionDashDodge", &transitionDashDodge_, 0.01f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void MageAttackState::PlayAnimation()
    {     
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());
        float transitionTime = 0.1f;
        if (animationIndex == Player::Animation::MageIdle)
        {
            transitionTime = transitionIdle_;
        }
        else if (animationIndex == Player::Animation::MageRun)
        {
            transitionTime = transitionRun_;
        }
        else if (animationIndex == Player::Animation::MageDush)
        {
            transitionTime = transitionDashDodge_;
        }
        owner_->SetTransitionTime(transitionTime);

        owner_->PlayBlendAnimation(Player::Animation::MageSkillAttack0_0, false);
    }
}

// ----- 攻撃1_0 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void MageAttack1_0::Initialize()
    {
        // フラグをリセット
        owner_->ResetFlags();

        // アニメーション再生
        PlayAnimation();

        // 変数初期化
        isCreateHailBolt_ = false;
    }

    // ----- 更新 -----
    void MageAttack1_0::Update(const float& elapsedTime)
    {
        // 先行入力判定
        if (CheckNextInput()) return;

        // ルートモーションの設定
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            owner_->SetUseRootMotion(true);
        }
        
        // 旋回処理
        Turn(elapsedTime);

        // 魔法の弾発射
        if (owner_->GetAnimationSeconds() > hailBoltLaunchFrame_ && isCreateHailBolt_ == false)
        {
            HailBolt* hailBolt = new HailBolt();
            hailBolt->Launch(owner_->GetJointPosition("index_03_l"), owner_->GetTransform()->CalcForward(), hailBoltMoveSpeed_);

            // カメラシェイク
            Camera::Instance().ScreenVibrate(cameraShakePower_, cameraShakeTime_);

            // コントローラー振動
            Input::Instance().GetGamePad().Vibration(gamePadVibrationTime_, gamePadVibrationPower_.x, gamePadVibrationPower_.y);

            isCreateHailBolt_ = true;
        }

        // 攻撃終了判定
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::MageIdle);
            return;
        }
    }

    // ----- 終了化 -----
    void MageAttack1_0::Finalize()
    {
        // ルートモーション使用終了
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui用 -----
    void MageAttack1_0::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- Animation ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("DashDodgeBlendFrame", &dashDodgeBlendFrame_, 0.01f);
                ImGui::DragFloat("TransitionDashDodge", &transitionDashDodge_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Turn ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("TurnEndFrame", &turnEndFrame_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- NextInput ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("RunStateChangeFrame", &runStateChangeFrame_, 0.01f);
                ImGui::DragFloat("DodgeStateChangeFrame", &dodgeStateChangeFrame_, 0.01f);
                ImGui::DragFloat("Attack1_1ChangeFrame", &attack1_1ChangeFrame_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- HailBolt ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("LaunchFrame", &hailBoltLaunchFrame_, 0.01f);
                ImGui::DragFloat("MoveSpeed", &hailBoltMoveSpeed_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- GamePadVibration ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat2("Power", &gamePadVibrationPower_.x, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Time", &gamePadVibrationTime_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- CameraShake ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("Power", &cameraShakePower_, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Time", &cameraShakeTime_, 0.01f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void MageAttack1_0::PlayAnimation()
    {
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());
        float animationStartFrame = 0.0f;
        float transitionTime = 0.15f;
        if (animationIndex == Player::Animation::MageDush)
        {
            animationStartFrame = dashDodgeBlendFrame_;
            transitionTime = transitionDashDodge_;
        }

        owner_->PlayBlendAnimation(Player::Animation::MageAttack1_0, false, playAniamtionSpeed_, animationStartFrame);
        owner_->SetTransitionTime(transitionTime);
    }

    // ----- 先行入力判定 -----
    const bool MageAttack1_0::CheckNextInput()
    {
        if (owner_->IsDodgeKeyDown())
        {
            owner_->SetNextInput(Player::NextInput::Dodge);
        }

        if (owner_->IsMageAttack1KeyDown())
        {
            owner_->SetNextInput(Player::NextInput::MageAttack1);
        }

        const float curretAnimationSeconds = owner_->GetAnimationSeconds();
                
        // ----- 回避に遷移 -----
        if(owner_->GetNextInput() == Player::NextInput::Dodge && curretAnimationSeconds >= dodgeStateChangeFrame_)
        {
            owner_->ChangeState(Player::STATE::MageDodge);
            return true;
        }
        // ----- 攻撃1_1に遷移 -----
        else if (owner_->GetNextInput() == Player::NextInput::MageAttack1 && curretAnimationSeconds >= attack1_1ChangeFrame_)
        {
            owner_->ChangeState(Player::STATE::MageAttack1_1);
            return true;
        }        
        

        // ----- 走りに遷移 -----
        if (curretAnimationSeconds >= runStateChangeFrame_)
        {
            const float aLX = Input::Instance().GetGamePad().GetAxisLX();
            const float aLY = Input::Instance().GetGamePad().GetAxisLY();
            // 移動入力がない
            if (aLX == 0.0f && aLY == 0.0f) return false;

            owner_->ChangeState(Player::STATE::MageRun);
            return true;
        }
        // =============================================
        // ========== これより下に何も書かない ===========
        // =============================================
        return false;
    }

    // ----- 旋回処理 -----
    void MageAttack1_0::Turn(const float& elapsedTime)
    {
        const float currentAnimationSeconds = owner_->GetAnimationSeconds();

        // 旋回処理を行わない
        if (currentAnimationSeconds > turnEndFrame_) return;

        const float aLX = Input::Instance().GetGamePad().GetAxisLX();
        const float aLY = Input::Instance().GetGamePad().GetAxisLY();
        // 入力がないので旋回しない
        if (aLX == 0.0f && aLY == 0.0f) return;

        DirectX::XMFLOAT2 ownerForward = XMFloat2Normalize({ owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z });
        DirectX::XMFLOAT2 leftStick = Camera::Instance().ConvertTo2DVectorFromCamera(DirectX::XMFLOAT2(aLX, aLY));

        // 回転角度を算出
        const float angle = acosf(std::clamp(XMFloat2Dot(leftStick, ownerForward), -1.0f, 1.0f));

        // 回転角度が微量なので旋回しない
        if (angle < DirectX::XMConvertToRadians(1)) return;

        //どっち方向に回転するか
        float forwardCross = XMFloat2Cross(leftStick, ownerForward);

        const float rotationY = angle * owner_->GetRotateSpeed() * elapsedTime;

        if (forwardCross > 0)
        {
            owner_->GetTransform()->AddRotationY(-rotationY);
        }
        else
        {
            owner_->GetTransform()->AddRotationY(rotationY);
        }
    }
}

// ----- 攻撃1_1 -----
namespace PlayerState
{
    // ----- コンストラクタ -----
    MageAttack1_1::MageAttack1_1(Player* player)
        : State(player, "MageAttack1_1")
    {
        aquaBulletChargeEmitter_.SetEmitParameter("AquaBulletCharge");
        aquaBulletLaunchEmitter_.SetEmitParameter("AquaBulletLaunch");
    }

    // ----- 初期化 -----
    void MageAttack1_1::Initialize()
    {
        // フラグをリセット
        owner_->ResetFlags();

        // アニメーション再生
        PlayAnimation();

        isAquaBulletCreated_ = false;
        isAquaBulletLaunched_ = false;
    }

    // ----- 更新 -----
    void MageAttack1_1::Update(const float& elapsedTime)
    {
        // 先行入力判定
        if (CheckNextInput()) return;

        // ルートモーションの設定
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            owner_->SetUseRootMotion(true);
        }

        // 弾の処理
        UpdateAquaBullet();

        // 攻撃終了判定
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::MageIdle);
            return;
        }
    }

    // ----- 終了化 -----
    void MageAttack1_1::Finalize()
    {
        // ルートモーション使用終了
        owner_->SetUseRootMotion(false);

        aquaBullet_ = nullptr;
    }

    // ----- ImGui用 -----
    void MageAttack1_1::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- AquaBullet ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("CreateStartFrame", &aquaBulletCreateStartFrame_, 0.01f);
                ImGui::DragFloat("CreateEndFrame", &aquaBulletCreateEndFrame_, 0.01f);
                ImGui::DragFloat("MoveSpeed", &aquaBulletMoveSpeed_, 0.1f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- NextInput ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("RunStateChangeFrame", &runStateChangeFrame_, 0.01f);
                ImGui::DragFloat("DodgeStateChangeFrame", &dodgeStateChangeFrame_, 0.01f);
                ImGui::DragFloat("Attack1_2ChangeFrame", &attack1_2ChangeFrame_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- GamePadVibration ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat2("Power", &gamePadVibrationPower_.x, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Time", &gamePadVibrationTime_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- CameraShake ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("Power", &cameraShakePower_, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Time", &cameraShakeTime_, 0.01f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void MageAttack1_1::PlayAnimation()
    {
        owner_->PlayBlendAnimation(Player::Animation::MageAttack1_1, false, playAnimationSpeed_, animationStartFrame_);

        owner_->SetTransitionTime(0.15f);
    }

    // ----- 先行入力判定 -----
    const bool MageAttack1_1::CheckNextInput()
    {
        const float curretAnimationSeconds = owner_->GetAnimationSeconds();
        
        // ----------------------------------------
        //              先行入力受付
        // ----------------------------------------

        // ----- 回避入力受付 -----
        if (owner_->IsDodgeKeyDown())
        {
            owner_->SetNextInput(Player::NextInput::Dodge);
        }
        // ----- 攻撃1_2入力受付 -----
        if (owner_->IsMageAttack1KeyDown())
        {
            owner_->SetNextInput(Player::NextInput::MageAttack1);
        }

        // ----------------------------------------
        //         先行入力によるステート変更
        // ----------------------------------------

        // ----- 回避へ遷移 -----
        if (owner_->GetNextInput() == Player::NextInput::Dodge && curretAnimationSeconds >= dodgeStateChangeFrame_)
        {
            owner_->ChangeState(Player::STATE::MageDodge);
            return true;
        }
        // ----- 攻撃1_2へ遷移 -----
        else if (owner_->GetNextInput() == Player::NextInput::MageAttack1 && curretAnimationSeconds >= attack1_2ChangeFrame_)
        {
            owner_->ChangeState(Player::STATE::MageAttack1_2);
            return true;
        }

        // ----- 走りに遷移 -----
        if (curretAnimationSeconds >= runStateChangeFrame_)
        {
            const float aLX = Input::Instance().GetGamePad().GetAxisLX();
            const float aLY = Input::Instance().GetGamePad().GetAxisLY();
            // 移動入力がない
            if (aLX == 0.0f && aLY == 0.0f) return false;

            owner_->ChangeState(Player::STATE::MageRun);
            return true;
        }
        // =============================================
        // ========== これより下に何も書かない ===========
        // =============================================
        return false;
    }

    // ----- 水の弾更新 -----
    void MageAttack1_1::UpdateAquaBullet()
    {
        // 既に発射済み
        if (isAquaBulletLaunched_) return;

        const float currentAniamtionSeconds = owner_->GetAnimationSeconds();

        // 生成フレームに達していない
        if (currentAniamtionSeconds < aquaBulletCreateStartFrame_) return;

        // 弾生成処理
        if (isAquaBulletCreated_ == false)
        {
            aquaBullet_ = new AquaBullet();
            aquaBullet_->GetTransform()->SetPosition(owner_->GetStaffJointPosition("joint1"));

            // チャージエフェクト生成
            aquaBulletChargeEmitter_.SetEmitPosition(owner_->GetStaffJointPosition("joint1"));
            aquaBulletChargeEmitter_.EmitParticle();

            isAquaBulletCreated_ = true;
        }

        // 徐々に大きくする
        if (currentAniamtionSeconds <= aquaBulletCreateEndFrame_)
        {
            const float totalFrame = aquaBulletCreateEndFrame_ - aquaBulletCreateStartFrame_;
            const float currentFrame = currentAniamtionSeconds - aquaBulletCreateStartFrame_;

            aquaBullet_->GetTransform()->SetScaleFactor(XMFloatLerp(0.0f, 0.25f, currentFrame / totalFrame));
        }
        // 発射
        else
        {
            const DirectX::XMFLOAT3 ownerForward = owner_->GetTransform()->CalcForward();

            aquaBullet_->Launch(ownerForward, aquaBulletMoveSpeed_);

            // 発射エフェクト再生
            aquaBulletLaunchEmitter_.SetEmitPosition(owner_->GetStaffJointPosition("joint1"));
            aquaBulletLaunchEmitter_.EmitParticle();

            // カメラシェイク
            Camera::Instance().ScreenVibrate(cameraShakePower_, cameraShakeTime_);

            // コントローラ振動
            Input::Instance().GetGamePad().Vibration(gamePadVibrationTime_, gamePadVibrationPower_.x, gamePadVibrationPower_.y);

            isAquaBulletLaunched_ = true;
        }
    }
}

// ----- 攻撃1_2 -----
namespace PlayerState
{
    // ----- コンストラクタ -----
    MageAttack1_2::MageAttack1_2(Player* player)
        : State(player, "MageAttack1_2")
    {
        smokeEmitter_.SetEmitParameter("AquaSmoke");
    }

    // ----- 初期化 -----
    void MageAttack1_2::Initialize()
    {
        // フラグをリセット
        owner_->ResetFlags();

        // アニメーション再生
        PlayAnimation();

        // 変数初期化
        aquaSeekerEmitter[0].Initialize(0.19f);
        aquaSeekerEmitter[1].Initialize(0.20f);
        aquaSeekerEmitter[2].Initialize(0.21f);
    }

    // ----- 更新 -----
    void MageAttack1_2::Update(const float& elapsedTime)
    {
        // 先行入力判定
        if (CheckNextInput()) return;

        // ルートモーションを使用する
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            owner_->SetUseRootMotion(true);
            owner_->SetRootMotionValue(rootMotionMoveValue_);
        }

        if (owner_->GetAnimationSeconds() >= smokeEffectStartFrame_ && owner_->GetAnimationSeconds() <= smokeEffectEndFrame_)
        {
            smokeEmitter_.SetEmitPosition(owner_->GetStaffJointPosition("joint1"));
            smokeEmitter_.EmitParticle();
        }

        // 弾丸発射
        const DirectX::XMFLOAT3 emitPosition = owner_->GetStaffJointPosition("joint1");
        for (int i = 0; i < 3; ++i)
        {
            aquaSeekerEmitter[i].Update(owner_->GetAnimationSeconds(), projectileMoveSpeed_, emitPosition);
        }

        // 攻撃終了判定
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::MageIdle);
            return;
        }
    }

    // ----- 終了化 -----
    void MageAttack1_2::Finalize()
    {
        // ルートモーション使用終了
        owner_->SetUseRootMotion(false);    
        owner_->SetRootMotionValue(1.0f);
    }

    // ----- ImGui用 -----
    void MageAttack1_2::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- AquaSeekerEmitter ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                for (int i = 0; i < 3; ++i)
                {
                    aquaSeekerEmitter[i].DrawDebug();
                }

                ImGui::DragFloat("ProjectileMoveSpeed", &projectileMoveSpeed_, 0.01f);

                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("---------- Animation ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("PlayAnimationSpeed", &playAnimationSpeed_, 0.01f);
                ImGui::DragFloat("AnimationStartFrame", &animationStartFrame_, 0.01f);
                ImGui::DragFloat("TransitionAttack1_1", &transitionAttack1_1_, 0.01f);
                ImGui::DragFloat("RootMotionMoveValue", &rootMotionMoveValue_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- NextInput ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("RunStateChangeFrame", &runStateChangeFrame_, 0.01f);
                ImGui::DragFloat("DodgeStateChangeFrame", &dodgeStateChangeFrame_, 0.01f);
                ImGui::DragFloat("Attack1_3ChangeFrame", &attack1_3ChangeFrame_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Effect ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("SmokeStartFrame", &smokeEffectStartFrame_, 0.01f);
                ImGui::DragFloat("SmokeEndFrame", &smokeEffectEndFrame_, 0.01f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void MageAttack1_2::PlayAnimation()
    {
        owner_->PlayBlendAnimation(Player::Animation::MageAttack1_2, false, playAnimationSpeed_, animationStartFrame_);
        owner_->SetTransitionTime(transitionAttack1_1_);
        
    }

    // ----- 先行入力判定 -----
    const bool MageAttack1_2::CheckNextInput()
    {
        const float curretAnimationSeconds = owner_->GetAnimationSeconds();

        // ----------------------------------------
        //              先行入力受付
        // ----------------------------------------

        // ----- 回避入力受付 -----
        if (owner_->IsDodgeKeyDown())
        {
            owner_->SetNextInput(Player::NextInput::Dodge);
        }
        // ----- 攻撃1_3入力受付 -----
        if (owner_->IsMageAttack1KeyDown())
        {
            owner_->SetNextInput(Player::NextInput::MageAttack1);
        }

        // ----------------------------------------
        //         先行入力によるステート変更
        // ----------------------------------------

        // ----- 回避へ遷移 -----
        if (owner_->GetNextInput() == Player::NextInput::Dodge && curretAnimationSeconds >= dodgeStateChangeFrame_)
        {
            owner_->ChangeState(Player::STATE::MageDodge);
            return true;
        }
        // ----- 攻撃1_2へ遷移 -----
        else if (owner_->GetNextInput() == Player::NextInput::MageAttack1 && curretAnimationSeconds >= attack1_3ChangeFrame_)
        {
            owner_->ChangeState(Player::STATE::MageAttack1_3);
            return true;
        }

        // ----- 走りに遷移 -----
        if (curretAnimationSeconds >= runStateChangeFrame_)
        {
            const float aLX = Input::Instance().GetGamePad().GetAxisLX();
            const float aLY = Input::Instance().GetGamePad().GetAxisLY();
            // 移動入力がない
            if (aLX == 0.0f && aLY == 0.0f) return false;

            owner_->ChangeState(Player::STATE::MageRun);
            return true;
        }
        // =============================================
        // ========== これより下に何も書かない ===========
        // =============================================
        return false;
    }

    // =================================
    // ========== メンバ構造体 ==========
    // =================================
    
    // ----- 初期化 -----
    void MageAttack1_2::AquaSeekerEmitter::Initialize(const float& frame)
    {
        emitFrame_ = frame;
        isLaunched_ = false;        
    }

    // ----- 更新 -----
    void MageAttack1_2::AquaSeekerEmitter::Update(const float& animationSeconds, const float& moveSpeed, const DirectX::XMFLOAT3& emitPosition)
    {
        if (isLaunched_) return;

        if (animationSeconds > emitFrame_)
        {
            AquaSeeker* aquaSeeker = new AquaSeeker();
            aquaSeeker->GetTransform()->SetPosition(emitPosition);
            aquaSeeker->SetMoveSpeed(moveSpeed);
            aquaSeeker->SetMoveType(num_);

            isLaunched_ = true;
        }
    }

    // ----- ImGui用 -----
    void MageAttack1_2::AquaSeekerEmitter::DrawDebug()
    {
        if (ImGui::TreeNodeEx(name_.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("EmitFrame", &emitFrame_, 0.01f);

            ImGui::TreePop();
        }
    }
}

// ----- 攻撃1_3 -----
namespace PlayerState
{
    // ----- コンストラクタ -----
    MageAttack1_3::MageAttack1_3(Player* player)
        : State(player, "MageAttack1_3")
    {
        chargeEffectEmitter_.SetEmitParameter("AquaCharge");
    }

    // ----- 初期化 -----
    void MageAttack1_3::Initialize()
    {
        // フラグをリセット
        owner_->ResetFlags();

        // アニメーション再生
        PlayAnimation();

        Camera::Instance().UseMageAttackCamera();

        isCreateAquaMeteor_ = false;
        isLaunchedAquaMeteor_ = false;
    }

    // ----- 更新 -----
    void MageAttack1_3::Update(const float& elapsedTime)
    {
        // ルートモーションを使用する
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            owner_->SetUseRootMotion(true);
        }

        // チャージエフェクト
        UpdateChargeEffect();

        // 弾生成
        if (isCreateAquaMeteor_ == false)
        {
            if (owner_->GetAnimationSeconds() >= aquaMeteorCreateFrame_)
            {
                aquaMeteor_ = new AquaMeteor();
                isCreateAquaMeteor_ = true;
            }
        }

        // 発射
        if (isLaunchedAquaMeteor_ == false)
        {
            if (owner_->GetAnimationIndex() == static_cast<int>(Player::Animation::MageAttack1_3End) &&
                owner_->GetAnimationSeconds() >= aquaMeteorLaunchFrame_)
            {
                // カメラシェイクを入れる
                Camera::Instance().ScreenVibrate(launchCameraShakePower_, launchCameraShakeTime_);

                aquaMeteor_->Launch({}, owner_->GetTransform()->CalcForward(), aquaMeteorMoveSpeed_);
                isLaunchedAquaMeteor_ = true;
            }
        }

        // 攻撃終了判定
        if (owner_->IsPlayAnimation() == false)
        {
            if (owner_->GetAnimationIndex() == static_cast<int>(Player::Animation::MageAttack1_3Start))
            {
                owner_->PlayBlendAnimation(Player::Animation::MageAttack1_3Loop, false);
                owner_->SetTransitionTime(0.1f);
                owner_->SetUseRootMotion(false);

                // チャージのカメラシェイクを入れる
                Camera::Instance().ScreenVibrate(chargeCameraShakePower_, chargeCameraShakeTime_);
            }
            else if (owner_->GetAnimationIndex() == static_cast<int>(Player::Animation::MageAttack1_3Loop))
            {
                owner_->PlayBlendAnimation(Player::Animation::MageAttack1_3End, false);
                owner_->SetTransitionTime(0.1f);
                owner_->SetUseRootMotion(false);
            }
            else
            {
                owner_->ChangeState(Player::STATE::MageIdle);
                return;
            }
        }
    }

    // ----- 終了化 -----
    void MageAttack1_3::Finalize()
    {
        // ルートモーション使用終了
        owner_->SetUseRootMotion(false);

        aquaMeteor_ = nullptr;
    }

    // ----- ImGui用 -----
    void MageAttack1_3::DrawDebug()
    {
        if (ImGui::TreeNodeEx(GetName(), ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- Animation ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("PlayAnimationSpeed", &playAnimationSpeed_, 0.01f);
                ImGui::DragFloat("AnimationStartFrame", &animationStartFrame_, 0.01f);
                ImGui::DragFloat("TransitionAttack1_2", &transitionAttack1_2_, 0.01f);

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- AquaMeteor ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("CreateFrame", &aquaMeteorCreateFrame_, 0.01f);
                ImGui::DragFloat("LaunchFrame", &aquaMeteorLaunchFrame_, 0.01f);
                ImGui::DragFloat("MoveSpeed", &aquaMeteorMoveSpeed_, 0.01f);                

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Effect ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::TreeNodeEx("===== ChargeEffect =====", ImGuiTreeNodeFlags_Framed))
                {
                    ImGui::DragFloat("StartFrame", &chargeEffectStartFrame_, 0.01f);
                    ImGui::DragFloat("EndFrame", &chargeEffectEndFrame_, 0.01f);

                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("---------- Effect ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat("ChargePower", &chargeCameraShakePower_, 0.01f);
                ImGui::DragFloat("ChargeTime", &chargeCameraShakeTime_, 0.01f);

                ImGui::DragFloat("launchPower", &launchCameraShakePower_, 0.01f);
                ImGui::DragFloat("launchTime", &launchCameraShakeTime_, 0.01f);

                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
    }

    // ----- アニメーション再生 -----
    void MageAttack1_3::PlayAnimation()
    {
        owner_->PlayBlendAnimation(Player::Animation::MageAttack1_3Start, false, playAnimationSpeed_, animationStartFrame_);
        owner_->SetTransitionTime(transitionAttack1_2_);
    }

    // ----- チャージエフェクト -----
    void MageAttack1_3::UpdateChargeEffect()
    {
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());
        const float animationSeconds = owner_->GetAnimationSeconds();

        // Start Animation
        if (animationIndex == Player::Animation::MageAttack1_3Start)
        {
            if (animationSeconds < chargeEffectStartFrame_) return;
        }
        // End Animation
        if (animationIndex == Player::Animation::MageAttack1_3End)
        {
            if (animationSeconds > chargeEffectEndFrame_) return;
        }

        chargeEffectEmitter_.SetEmitParameter("AquaCharge");
        chargeEffectEmitter_.SetEmitPosition(owner_->GetStaffJointPosition("joint1"));
        chargeEffectEmitter_.EmitParticle();
    }
}

namespace PlayerState
{

}

#pragma endregion ---------- Mage ----------