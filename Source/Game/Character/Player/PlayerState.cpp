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

        // 操作UI設定
        if(UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide) !=nullptr)
            UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 0.0f);
    }

    // ----- 更新 -----
    void IdleState::Update(const float& elapsedTime)
    {
        if (owner_->IsGuardCounterKeyDown() && owner_->GetIsGuardGaugeDepleted() == false)
        {
            owner_->ChangeState(Player::STATE::GuardCounter);
            return;
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

        // 操作UI設定
        if (UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide) != nullptr)
            UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 0.0f);

        // 足音の効果音再生
        dashSENum_ = AudioManager::Instance().PlaySE(SE::Dash);

        // 変数初期化
        changeStateTimer_ = 0.0f;
    }

    // ----- 更新 -----
    void RunState::Update(const float& elapsedTime)
    {
        if (owner_->IsGuardCounterKeyDown() && owner_->GetIsGuardGaugeDepleted() == false)
        {
            owner_->ChangeState(Player::STATE::GuardCounter);
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

        // 操作UI設定
        if(owner_->GetIsDash()) UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(750.0f, 0.0f);
        else UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 0.0f);
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
        guardEffect_ = EffectManager::Instance().GetEffect("Guard")->Play(position, 1.0f, 1.0f);

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
        GamePad& gamePad = Input::Instance().GetGamePad();

        const bool guardButton = gamePad.GetButton() & GamePad::BTN_X;
        if (owner_->GetIsBlendUpperLowerBodyAnimation() == false && guardButton == false)
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
        const float aLx = gamePad.GetAxisLX();
        const float aLy = gamePad.GetAxisLY();
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
                // 回転処理
                const DirectX::XMFLOAT3 knockBackDirection_float3 = owner_->GetKnockBackDirection();
                const DirectX::XMFLOAT3 playerFront_float3 = owner_->GetTransform()->CalcForward();
                const DirectX::XMFLOAT2 knockBackDirection = XMFloat2Normalize({ knockBackDirection_float3.x, knockBackDirection_float3.z });
                const DirectX::XMFLOAT2 playerFront = XMFloat2Normalize({ playerFront_float3.x, playerFront_float3.z });
                
                float dot = std::clamp(XMFloat2Dot(knockBackDirection, playerFront), -1.0f, 1.0f);
                float angle = acosf(dot);

                float cross = XMFloat2Cross(knockBackDirection, playerFront);
                if (cross > 0)  owner_->GetTransform()->AddRotationY(-angle);
                else            owner_->GetTransform()->AddRotationY(angle);

                // ガードゲージを消費 (ガードゲージがまだあればtrue)
                if (owner_->UseGuardGaugeOnBlock())
                {// ガードできた
                    // ガードした方向に向かせる

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
        //owner_->PlayBlendAnimation(Player::Animation::CounterAttack0, false, 1.0f, 0.35f);
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
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_RIGHT_TRIGGER)
        {
            owner_->ChangeState(Player::STATE::GuardCounterAttack);
            return;
        }

        if (owner_->IsPlayAnimation() == false)
        {
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

        // 操作UI設定
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(1500.0f, 700.0f);

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

        // 操作UI設定
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(1500.0f, 700.0f);

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

        // 操作UI設定
        if (UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide) != nullptr)
            UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 0.0f);

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

        // 操作UI設定
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(1500.0f, 0.0f);

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

        // 操作UI設定
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(1500.0f, 700.0f);

        // 変数初期化
        addForceData_.Initialize(0.35f, 0.3f, 1.0f);
        attackData_.Initialize(0.35f, 0.7f);

        isPlayCameraVibration_ = false;
    }

    // ----- 更新 -----
    void CounterComboState::Update(const float& elapsedTime)
    {
        // アニメーション速度調整
        UpdateAnimationSpeed();

        // カメラシェイク
        if (owner_->GetAnimationSeconds() > cameraVibrationFrame_ && isPlayCameraVibration_ == false)
        {
            Camera::Instance().ScreenVibrate(cameraVibrationVolume_, cameraVibrationTime_);

            isPlayCameraVibration_ = true;
        }

        currentAnimationFrame_ = owner_->GetAnimationSeconds();

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

        // 操作UI設定
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 350.0f);

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

        // 操作UI設定
        if (UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide) != nullptr)
            UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 350.0f);

        // 変数初期化
        attackData_.Initialize(0.1f, 0.35f);      

        // SE再生
        AudioManager::Instance().PlaySE(SE::SowrdSlash0);
    }

    // ----- 更新 -----
    void ComboAttack0_0::Update(const float& elapsedTime)
    {
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

        // 操作UI設定
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(750.0f, 350.0f);

        // 変数初期化
        attackData_.Initialize(0.06f, 0.3f);

        // SE再生
        AudioManager::Instance().PlaySE(SE::SowrdSlash1);
    }

    // ----- 更新 -----
    void ComboAttack0_1::Update(const float& elapsedTime)
    {
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
        
        // 操作UI設定
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(1500.0f, 350.0f);

        // 変数初期化
        attackData_.Initialize(0.7f, 0.9f);
        isPlaySwordSlashSE_ = false;
    }

    // ----- 更新 -----
    void ComboAttack0_2::Update(const float& elapsedTime)
    {
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

        // 操作UI設定
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 700.0f);

        // 変数初期化
        attackData_.Initialize(0.65f, 0.8f);
        isVibration_ = false;
        isPlaySwordSlashSE_ = false;
    }

    // ----- 更新 -----
    void ComboAttack0_3::Update(const float& elapsedTime)
    {
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
}