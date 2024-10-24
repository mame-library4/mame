#include "PlayerState.h"
#include <cmath>
#include "Input.h"
#include "Camera.h"
#include "Easing.h"
#include "MathHelper.h"
#include "../Enemy/EnemyManager.h"
#include "Effect/EffectManager.h"

#include "UI/UIFader.h"

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
        if (owner_->IsGuardCounterKeyDown())
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
        if (ImGui::TreeNode(GetName()))
        {
            float a;
            ImGui::DragFloat("a", &a);

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
        // 回避先行入力受付
        if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
            animationSeconds <= owner_->GetDodgeInputEndFrame())
        {
            if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
        }

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

        // 変数初期化
        changeStateTimer_ = 0.0f;
    }

    // ----- 更新 -----
    void RunState::Update(const float& elapsedTime)
    {
        if (owner_->IsGuardCounterKeyDown())
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
    }

    // ----- ImGui用 -----
    void RunState::DrawDebug()
    {
        if (ImGui::BeginMenu(GetName()))
        {


            ImGui::EndMenu();
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
        // 回避先行入力受付
        if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
            animationSeconds <= owner_->GetDodgeInputEndFrame())
        {
            if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
        }
        // 攻撃先行入力受付
        if (animationSeconds >= owner_->GetAttackInputStartFrame() &&
            animationSeconds <= owner_->GetAttackInputEndFrame())
        {
            if (owner_->IsComboAttack0KeyDown()) owner_->SetNextInput(Player::NextInput::ComboAttack0);
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
        owner_->PlayUpperLowerBodyAnimation(static_cast<int>(Player::Animation::BlockLoop), false);

        DirectX::XMFLOAT3 position = owner_->GetJointPosition("pelvis");
        guardEffect_ = EffectManager::Instance().GetEffect("Guard")->Play(position, 1.0f, 1.0f);

        owner_->SetIsGuardCounterStance(true);
        owner_->SetIsGuardCounterSuccessful(false); // リセットする


        // 最大速度を設定
        //owner_->SetMaxSpeed(2.0f);
        owner_->SetMaxSpeed(5.0f);

        // 変数初期化
        gamePadVibration_.Initialize(0.0f, 0.2f, 0.5f);
        guardEffectLerpTimer_       = 0.0f;
    }

    // ----- 更新 -----
    void GuardCounterState::Update(const float& elapsedTime)
    {
        // 移動入力処理
        GamePad& gamePad = Input::Instance().GetGamePad();
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


        // アニメーション更新
        UpdateAnimation();

        // エフェクト更新
        UpdateEffect(elapsedTime);

        // ガードカウンターを無効化する
        if (owner_->GetAnimationIndex() == static_cast<int>(Player::Animation::BlockEnd))
        {
            if (owner_->GetAnimationSeconds() > 0.24f && owner_->GetIsGuardCounterStance())
            {
                owner_->SetIsGuardCounterStance(false);
            }
        }

        // カウンター成功
        if (owner_->GetIsGuardCounterSuccessful())
        {
            owner_->ChangeState(Player::STATE::GuardCounterAttack);

            // TODO:仮で作ってる
            gamePadVibration_.Update(owner_->GetAnimationSeconds());

            return;
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
    }

    // ----- ImGui用 -----
    void GuardCounterState::DrawDebug()
    {
        ImGui::DragFloat("GuardEffectStartSize", &guardEffectStartSize_, 0.1f, 1.0f, 6.0f);
        ImGui::DragFloat("GuardEffectEndSize",   &guardEffectEndSize_,   0.1f, 1.0f, 6.0f);
        ImGui::DragFloat("LerpTimer", &guardEffectLerpTimer_);
        ImGui::DragFloat("LerpSpeed", &guardEffectLerpSpeed_);
    }

    // ----- アニメーション更新 -----
    void GuardCounterState::UpdateAnimation()
    {
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
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
                owner_->PlayAnimation(Player::Animation::DownLoop, true);
                state_ = 1;
            }

            break;
        case 1:
            
            if (EnemyManager::Instance().GetEnemy(0)->GetActiveNodeName() != "Roar")
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
        if(owner_->IsPlayAnimation() == false && isFirstAnimation_ == true)
        {
            owner_->PlayBlendAnimation(Player::Animation::GetUp, false);
            owner_->SetTransitionTime(0.1f);
            isFirstAnimation_ = false;
        }
        //else if(owner_->IsPlayAnimation() == false)
        else if(owner_->GetAnimationSeconds() > 1.8f && isFirstAnimation_ == false)
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
    }

    void DamageState::DrawDebug()
    {
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
        float angle = acosf(XMFloat2Dot(addForceDirection, ownerFront));

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

        owner_->PlayBlendAnimation(Player::Animation::Damage, false, 1.0f, 0.2f);
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

        // テスト用
        if (owner_->GetHealth() > 0)
        {
            owner_->ChangeState(Player::STATE::Idle);
        }
    }

    // ----- 終了化 -----
    void DeathState::Finalize()
    {
        owner_->SetIsInvincible(false);
    }
    void DeathState::DrawDebug()
    {
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

        // 移動方向を算出
        CalcMoveDirection();

        // 無敵状態にする
        owner_->SetIsInvincible(true);

        // スタミナ消費
        owner_->UseDodgeStamina();

        // 操作UI設定
        if (UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide) != nullptr)
            UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 0.0f);

        // 変数初期化
        addForceData_.Initialize(0.15f, 0.27f, 0.4f);
        invincibleTimer_ = 0.0f;

        isRotating_ = false;
        isFirstTime_ = false;
    }

    // ----- 更新 -----
    void DodgeState::Update(const float& elapsedTime)
    {
        // 先行入力処理
        if (CheckNextInput()) return;

        Turn(elapsedTime);

        // アニメーションの速度設定
        SetAnimationSpeed();

        // 移動処理
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(moveDirection_, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // 無敵時間処理
        invincibleTimer_ += elapsedTime;
        if (invincibleTimer_ >= 0.4f)
        {
            owner_->SetIsInvincible(false);
        }

        if(owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void DodgeState::Finalize()
    {
        // 変数をリセットしておく
        isFirstTime_ = true;
    }

    void DodgeState::DrawDebug()
    {
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

        float forwardDot = XMFloat2Dot(inputDirection_, playerForward) - 1.0f;

        if (forwardDot > -0.01f)
        {
            isRotating_ = false;
            return;
        }

        const float speed = owner_->GetRotateSpeed() * elapsedTime;
        float rotateY = forwardDot * speed;
        rotateY = std::min(rotateY, -0.7f * speed);

        if (forwardCross > 0)
        {
            owner_->GetTransform()->AddRotationY(rotateY);
        }
        else
        {
            owner_->GetTransform()->AddRotationY(-rotateY);
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

        // 移動方向を算出
        CalcMoveDirection();

        // フラグをリセットする
        owner_->ResetFlags();

        // スタミナ消費
        owner_->UseDodgeStamina();

        // 変数初期化
        addForceData_.Initialize(0.15f, 0.27f, 0.4f);
        invincibleTimer_ = 0.0f;
    }

    // ----- 先行入力処理 -----
    const bool DodgeState::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();


#if 1
        const float nextInputStartFrame = 0.5f; // 先行入力開始フレーム

        if (animationSeconds > nextInputStartFrame)
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
                if (animationSeconds > avoidanceFrame)
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
                if (animationSeconds > comboAttack0Frame)
                {
                    owner_->ChangeState(Player::STATE::ComboAttack0_0);
                    return true;
                }
            }
            // それ以外
            else
            {
                const float moveFrame = 0.8f;// 移動に遷移できるフレーム
                if (animationSeconds > moveFrame)
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
                if (animationSeconds > comboAttack0Frame)
                {
                    owner_->ChangeState(Player::STATE::ComboAttack0_0);
                    return true;
                }
            }
            // それ以外
            else
            {
                const float moveFrame = 1.0f;// 移動に遷移できるフレーム
                if (animationSeconds > moveFrame)
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
                if (animationSeconds > avoidanceFrame)
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
                if (animationSeconds > comboAttack0Frame)
                {
                    owner_->ChangeState(Player::STATE::ComboAttack0_0);
                    return true;
                }
            }
            // それ以外
            else
            {
                const float moveFrame = 0.8f;// 移動に遷移できるフレーム
                if (animationSeconds > moveFrame)
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
                if (animationSeconds > avoidanceFrame)
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
                if (animationSeconds > comboAttack0Frame)
                {
                    owner_->ChangeState(Player::STATE::ComboAttack0_0);
                    return true;
                }
            }
            // それ以外
            else
            {
                const float moveFrame = 0.8f;// 移動に遷移できるフレーム
                if (animationSeconds > moveFrame)
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
            owner_->PlayBlendAnimation(Player::Animation::RollFront, false, 1.0f, 0.15f);
            owner_->SetTransitionTime(0.05f);
            return;            
        }

        // 元のアニメーションに応じてブレンドの時間を設定する
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());
        if (animationIndex == Player::Animation::Attack0_0)
        {
            owner_->SetTransitionTime(0.1f);
        }
        else if (animationIndex == Player::Animation::RunAttack1)
        {
            owner_->SetTransitionTime(0.1f);
        }
        else if (animationIndex == Player::Animation::GetUp)
        {
            //owner_->SetTransitionTime(0.1f);
            owner_->SetTransitionTime(0.2f);
        }
        else
        {
            owner_->SetTransitionTime(0.05f);
        }

        // ------------------------------------------------------------
        // プレイヤーの姿勢に合わせてアニメーションの方向を設定する
        // ------------------------------------------------------------
        const float animationSpeed = 1.0f;
        const float animationStartFrame = 0.15f;
        const float aLx = Input::Instance().GetGamePad().GetAxisLX();
        const float aLy = Input::Instance().GetGamePad().GetAxisLY();
        // 入力値がある場合
        if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
        {
            // カメラから見たスティックの入力値を算出する
            const DirectX::XMFLOAT3 cameraFront = Camera::Instance().GetTransform()->CalcForward();
            const DirectX::XMFLOAT3 cameraRight = Camera::Instance().GetTransform()->CalcRight();
            DirectX::XMFLOAT2 cameraInput =
            {
                aLy * cameraFront.x + aLx * cameraRight.x,
                aLy * cameraFront.z + aLx * cameraRight.z,
            };
            cameraInput = XMFloat2Normalize(cameraInput);
            DirectX::XMFLOAT2 ownerFront = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
            ownerFront = XMFloat2Normalize(ownerFront);
            
            // 内積で角度を算出
            float dot = acosf(XMFloat2Dot(cameraInput, ownerFront));

            // 左右判定
            float cross = XMFloat2Cross(cameraInput, ownerFront);

            // 回転角が９０度よりも小さければ 前,右,左 の三択
            if (dot < DirectX::XM_PIDIV2)
            {
                // 回転角が４５度よりも小さければ 前方向
                if (dot < DirectX::XM_PIDIV4)
                {                    
                    owner_->PlayBlendAnimation(Player::Animation::RollFront, false, animationSpeed, animationStartFrame);
                    return;
                }

                // 右方向
                if (cross < 0)
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollRight, false, animationSpeed, animationStartFrame);
                }
                // 左方向
                else
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollLeft, false, animationSpeed, animationStartFrame);
                }
            }
            // 回転角が９０度よりも大きければ 後,右,左 の三択
            else
            {
                // 回転角が１３５度よりも大きければ 後方向
                if (dot > DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4)
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollBack, false, animationSpeed, animationStartFrame);
                    return;
                }

                // 右方向
                if (cross < 0)
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollRight, false, animationSpeed, animationStartFrame);
                }
                // 左方向
                else
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollLeft, false, animationSpeed, animationStartFrame);
                }
            }
        }
        // 入力値がない場合前方向のアニメーションを設定する
        else
        {
            owner_->PlayBlendAnimation(Player::Animation::RollFront, false, animationSpeed, animationStartFrame);
            return;
        }
    }

    // ----- 移動方向算出 -----
    void DodgeState::CalcMoveDirection()
    {
        if (isFirstTime_ == false)
        {
            if (isInputStick_)
            {
                moveDirection_ = { inputDirection_.x, 0.0f, inputDirection_.y };
            }
            else
            {
                moveDirection_ = owner_->GetTransform()->CalcForward();
            }
            return;
        }

        // ----------------------------------------
        // 自分自身から見た前後左右のベクトルを用意する
        // ----------------------------------------        
        const DirectX::XMFLOAT3 ownerFront = owner_->GetTransform()->CalcForward();
        const DirectX::XMFLOAT3 ownerRight = owner_->GetTransform()->CalcRight();
        const DirectX::XMFLOAT3 moveDirection[4] =
        {
            ownerFront,
            ownerFront * -1,
            ownerRight,
            ownerRight * -1,
        };
        
        // ----------------------------------------
        // アニメーションに応じて移動方向を設定する
        // ----------------------------------------
        const int animationIndex = owner_->GetAnimationIndex();
        const int differenceNum = static_cast<int>(Player::Animation::RollFront);

        moveDirection_  = moveDirection[animationIndex - differenceNum];
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

        // カウンター時カメラを使用する
        Camera::Instance().SetUseCounterCamera();

        // 操作UI設定
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(1500.0f, 0.0f);

        // 変数初期化
        addForceBack_.Initialize(0.16f, 0.2f, 0.5f);
        addForceFront_.Initialize(0.66f, 0.30f, 1.0f);
        gamePadVibration_.Initialize(0.3f, 0.2f, 0.5f);
        attackData_.Initialize(0.75f, 1.0f);

        isNextInput_ = false;

        isCounterReaction = false;

        isTurnChecked_ = false;

        const DirectX::XMFLOAT3 pos = owner_->GetJointPosition("spine_02");
        Effect* counterEffect = EffectManager::Instance().GetEffect("Mikiri");
        //mikiriEffectHandle_ = counterEffect->Play(pos, 0.3f, 2.0f);
        mikiriEffectHandle_ = counterEffect->Play(pos, 0.05f, 2.0f);
    }

    // ----- 更新 -----
    void CounterState::Update(const float& elapsedTime)
    {
        const float counterStartFrame = 0.1f;
        const float counterEndFrame = 0.6f;
        // カウンター判定
        if (owner_->GetAnimationSeconds() > counterEndFrame)
        {
            if (owner_->GetIsCounter()) owner_->SetIsCounter(false);
        }
        else if (owner_->GetAnimationSeconds() > counterStartFrame)
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

                // TODO: 効果音を鳴らす

                isCounterReaction = true;
            }
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
        //if (owner_->GetIsAbleCounterAttack())
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
    }

    void CounterState::DrawDebug()
    {
    }

    // ----- アニメーション設定 -----
    void CounterState::SetAnimation()
    {
        const Player::STATE oldState = owner_->GetOldState();
        if (oldState == Player::STATE::ComboAttack0_0 ||
            oldState == Player::STATE::ComboAttack0_1 ||
            oldState == Player::STATE::ComboAttack0_2 ||
            oldState == Player::STATE::RunAttack)
        {
            owner_->PlayBlendAnimation(Player::Animation::Counter, false, 1.0f, 0.15f);
            owner_->SetTransitionTime(0.1f);
            return;
        }

        owner_->PlayBlendAnimation(Player::Animation::Counter, false, 1.0f);
        owner_->SetTransitionTime(0.1f);
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
            DirectX::XMFLOAT3 addForceDirection = {};

            // 左スティックの入力があるか判定
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                // カメラから見た左スティックの傾きを適応した方向を算出する
                const DirectX::XMFLOAT3 cameraForward = Camera::Instance().CalcForward();
                const DirectX::XMFLOAT3 cameraRight = Camera::Instance().CalcRight();
                addForceDirection =
                {
                    aLy * cameraForward.x + aLx * cameraRight.x,
                    0,
                    aLy * cameraForward.z + aLx * cameraRight.z
                };
                addForceDirection = XMFloat3Normalize(addForceDirection);
                addForceDirection = addForceDirection * -1;

                // 一気にスティックを反対方向に向けると、プレイヤーが違う方向を向くので修正
                DirectX::XMFLOAT2 cameraForward_float2 = { -addForceDirection.x, -addForceDirection.z };
                cameraForward_float2 = XMFloat2Normalize(cameraForward_float2);

                DirectX::XMFLOAT2 ownerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
                ownerForward = XMFloat2Normalize(ownerForward);

                // 外積をしてどちらに回転するのかを判定する
                float forwardCross = XMFloat2Cross(cameraForward_float2, ownerForward);

                // 内積で回転幅を算出
                float forwardDot = XMFloat2Dot(cameraForward_float2, ownerForward) - 1.0f;

                if (forwardCross > 0)
                {
                    owner_->GetTransform()->AddRotationY(forwardDot);
                }
                else
                {
                    owner_->GetTransform()->AddRotationY(-forwardDot);
                }
            }
            // 左スティックの入力がない場合後ろ方向に引く
            else
            {
                addForceDirection = owner_->GetTransform()->CalcForward() * -1.0f;
            }

            mikiriEffectAddPosition_ = addForceDirection;

            owner_->AddForce(addForceDirection, addForceBack_.GetForce(), addForceBack_.GetDecelerationForce());
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
            DirectX::XMFLOAT3 addForceDirection = {};

            // 左スティックの入力があるか判定
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                // カメラから見た左スティックの傾きを適応した方向を算出する
                const DirectX::XMFLOAT3 cameraForward = Camera::Instance().CalcForward();
                const DirectX::XMFLOAT3 cameraRight = Camera::Instance().CalcRight();
                addForceDirection =
                {
                    aLy * cameraForward.x + aLx * cameraRight.x,
                    0,
                    aLy * cameraForward.z + aLx * cameraRight.z
                };
                addForceDirection = XMFloat3Normalize(addForceDirection);

                addForceDirection_ = { addForceDirection.x, addForceDirection.z };
                addForceDirection_ = XMFloat2Normalize(addForceDirection_);

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
                float dot = acosf(XMFloat2Dot(addForceDirection_, ownerFront));

                // 90度以上回転角がある
                if (dot > DirectX::XM_PIDIV2)
                {
                    // 左右判定
                    float cross = XMFloat2Cross(addForceDirection_, ownerFront);

                    const DirectX::XMFLOAT3 ownerRight_float3 = owner_->GetTransform()->CalcRight();
                    const DirectX::XMFLOAT2 ownerRight_float2 = XMFloat2Normalize({ ownerRight_float3.x, ownerRight_float3.z });

                    if (cross < 0)
                    {
                        addForceDirection_ = ownerRight_float2;
                        addForceDirection = { addForceDirection_.x, 0.0f, addForceDirection_.y };
                    }
                    else
                    {
                        addForceDirection_ = ownerRight_float2 * -1;
                        addForceDirection = { addForceDirection_.x, 0.0f, addForceDirection_.y };
                    }
                }

                isRotating_ = true;
            }
            else
            {
                addForceDirection = owner_->GetTransform()->CalcForward();
            }

            owner_->AddForce(addForceDirection, addForceFront_.GetForce(), addForceFront_.GetDecelerationForce());
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
        float forwardCross = XMFloat2Cross(addForceDirection_, ownerForward);

        // 内積で回転幅を算出
        float forwardDot = XMFloat2Dot(addForceDirection_, ownerForward) - 1.0f;

        if (forwardDot > -0.01f)
        {
            isRotating_ = false;
        }

        // TODO:回転速度を固定値で入れちゃってる
        //const float speed = owner_->GetRotateSpeed() * elapsedTime;
        const float speed = 8.0f * elapsedTime;
        float rotateY = forwardDot * speed;
        rotateY = std::min(rotateY, -0.7f * speed);

        if (forwardCross > 0)
        {
            owner_->GetTransform()->AddRotationY(rotateY);
        }
        else
        {
            owner_->GetTransform()->AddRotationY(-rotateY);
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

        // 操作UI設定
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(1500.0f, 700.0f);

        // 変数初期化
        addForceData_.Initialize(0.35f, 0.3f, 1.0f);
        attackData_.Initialize(0.35f, 0.7f);
    }

    // ----- 更新 -----
    void CounterComboState::Update(const float& elapsedTime)
    {
        // RootMotionの設定
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotionを使用する
            owner_->SetUseRootMotion(true);
        }

        // 攻撃判定処理
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);

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
    void CounterComboState::DrawDebug()
    {
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
        if (ImGui::TreeNode(GetName()))
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
        if (ImGui::TreeNode(GetName()))
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
    void ComboAttack0_2::Finalize()
    {
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui用 -----
    void ComboAttack0_2::DrawDebug()
    {
        if (ImGui::TreeNode(GetName()))
        {

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
        }

        // 攻撃判定処理
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);


        // コントローラー＆カメラ 振動
        if (owner_->GetAnimationSeconds() > 0.8f && isVibration_ == false)
        {
            Input::Instance().GetGamePad().Vibration(0.2f, 1.0f);
            Camera::Instance().ScreenVibrate(0.1f, 0.2f);

            isVibration_ = true;
        }

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
        if (ImGui::TreeNode(GetName()))
        {


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
}