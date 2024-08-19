#include "PlayerState.h"
#include <cmath>
#include "Input.h"
#include "Camera.h"
#include "Easing.h"
#include "MathHelper.h"
#include "../Enemy/EnemyManager.h"
#include "Effect/EffectManager.h"

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
    }

    // ----- 更新 -----
    void IdleState::Update(const float& elapsedTime)
    {
        // 回避、攻撃入力受付
        if (owner_->CheckNextInput(Player::NextInput::None)) return;

        // カウンター受付
        if (owner_->GetCounterStanceKey())
        {
            owner_->ChangeState(Player::STATE::Counter);
            return;
        }

        if (owner_->GetOldState() == Player::STATE::Counter && owner_->GetIsBlendAnimation()) return;

        // 移動値があれば MoveState へ遷移する
        const float aLx = fabsf(Input::Instance().GetGamePad().GetAxisLX());
        const float aLy = fabsf(Input::Instance().GetGamePad().GetAxisLY());
        if (aLx != 0.0f || aLy != 0.0f)
        {
            if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
            {
                owner_->ChangeState(Player::STATE::Run);
                return;
            }

            owner_->ChangeState(Player::STATE::Walk);
            return;
        }
    }

    // ----- 終了化 -----
    void IdleState::Finalize()
    {
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
        else if (animationIndex == Player::Animation::RollForward ||
            animationIndex == Player::Animation::RollBack    ||
            animationIndex == Player::Animation::RollRight   ||
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
}

// ----- 歩き -----
namespace PlayerState
{
    // ----- 初期化 -----
    void WalkState::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        // アニメーション設定
        SetAnimation();

        // 最大速度を設定
        owner_->SetMaxSpeed(2.5f);
    }

    // ----- 更新 -----
    void WalkState::Update(const float& elapsedTime)
    {
        // 回避、攻撃受付
        if (owner_->CheckNextInput(Player::NextInput::None)) return;

        // カウンター受付
        if (owner_->GetCounterStanceKey())
        {
            owner_->ChangeState(Player::STATE::Counter);
            return;
        }

        // 旋回
        owner_->Turn(elapsedTime);

        const float aLx = fabsf(Input::Instance().GetGamePad().GetAxisLX());
        const float aLy = fabsf(Input::Instance().GetGamePad().GetAxisLY());
        if (aLx == 0.0f && aLy == 0.0f && owner_->GetIsBlendAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

        if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
        {
            owner_->ChangeState(Player::STATE::Run);
        }
    }

    // ----- 終了化 -----
    void WalkState::Finalize()
    {
        owner_->SetMoveDirection({});
        owner_->SetVelocity({});
    }

    // ----- アニメーション設定 -----
    void WalkState::SetAnimation()
    {
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        if (animationIndex == Player::Animation::RollForward ||
            animationIndex == Player::Animation::RollBack    ||
            animationIndex == Player::Animation::RollRight   ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->SetTransitionTime(0.3f);
        }
        else if (animationIndex == Player::Animation::ComboAttack0_0)
        {
            owner_->SetTransitionTime(0.3f);
        }
        else if (animationIndex == Player::Animation::ComboAttack0_1)
        {
            owner_->SetTransitionTime(0.4f);
        }
        else if (animationIndex == Player::Animation::ComboAttack0_2)
        {
            owner_->SetTransitionTime(0.25f);
        }
        else if (animationIndex == Player::Animation::RunAttack1)
        {
            owner_->SetTransitionTime(0.5f);
        }
        else if (animationIndex == Player::Animation::Run)
        {
            owner_->SetTransitionTime(0.2f);
        }
        else
        {
            owner_->SetTransitionTime(0.15f);
        }

        owner_->PlayBlendAnimation(Player::Animation::Walk, true, 1.4f);
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

        // 変数初期化
        stateChangeTimer_ = 0.0f;
    }

    // ----- 更新 -----
    void RunState::Update(const float& elapsedTime)
    {
        // 回避入力受付
        if (owner_->GetAvoidanceKeyDown())
        {
            owner_->ChangeState(Player::STATE::Avoidance);
            return;
        }

        // 攻撃入力受付
        if (owner_->GetComboAttack0KeyDown())
        {
            //if (owner_->GetAnimationSeconds() > 0.2f || owner_->GetIsAnimationLooped())
            if(owner_->GetIsBlendAnimation() == false)
            {
                owner_->ChangeState(Player::STATE::RunAttack);
                return;
            }
            else
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_0);
                return;
            }
        }

        // カウンター受付
        if (owner_->GetCounterStanceKey())
        {
            owner_->ChangeState(Player::STATE::Counter);
            return;
        }

        // 旋回
        owner_->Turn(elapsedTime);

        // 制御用
        if (owner_->GetIsBlendAnimation() == false) stateChangeTimer_ += elapsedTime;

        const float aLx = fabsf(Input::Instance().GetGamePad().GetAxisLX());
        const float aLy = fabsf(Input::Instance().GetGamePad().GetAxisLY());
        if (aLx == 0.0f && aLy == 0.0f)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

        if ((Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER) == false &&
            stateChangeTimer_ > 0.1f)
        {
            owner_->ChangeState(Player::STATE::Walk);
            return;
        }
    }
    
    // ----- 終了化 -----
    void RunState::Finalize()
    {
        owner_->SetMoveDirection({});
        owner_->SetVelocity({});
    }

    // ----- アニメーション設定 -----
    void RunState::SetAnimation()
    {
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        if (animationIndex == Player::Animation::RollForward ||
            animationIndex == Player::Animation::RollBack    ||
            animationIndex == Player::Animation::RollRight   ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->SetTransitionTime(0.2f);
        }
        else if (animationIndex == Player::Animation::ComboAttack0_0 ||
            animationIndex == Player::Animation::ComboAttack0_1 ||
            animationIndex == Player::Animation::RunAttack1)
        {
            owner_->SetTransitionTime(0.3f);
        }
        else if (animationIndex == Player::Animation::ComboAttack0_2)
        {
            owner_->SetTransitionTime(0.3f);
        }
        else
        {
            owner_->SetTransitionTime(0.15f);
        }
        owner_->PlayBlendAnimation(Player::Animation::Run, true);
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

        owner_->PlayBlendAnimation(Player::Animation::KnockDownStart, false);
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
        owner_->PlayBlendAnimation(Player::Animation::KnockDownStart, false, 2.0f);

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
                owner_->PlayAnimation(Player::Animation::KnockDownLoop, true);
                state_ = 1;
            }

            break;
        case 1:
            
            if (EnemyManager::Instance().GetEnemy(0)->GetActiveNodeName() != "Roar")
            {
                owner_->PlayBlendAnimation(Player::Animation::KnockDownEnd, false);
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
        owner_->PlayAnimation(Player::Animation::HitLarge, false, 1.2f);

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
    }

    // ----- 更新 -----
    void DamageState::Update(const float& elapsedTime)
    {
        // アニメーションの速度設定
        SetAnimationSpeed();

        // 移動値
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(addForceDirection_, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // 吹き飛ばされアニメーション処理
        if (owner_->GetAnimationSeconds() > 1.2f && isFirstAnimation_ == true)
        {// 入力があれば倒れてる状態を終了する
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) != 0.0f || fabsf(aLy) != 0.0f)
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
                if (owner_->GetAvoidanceKeyDown())
                {
                    owner_->ChangeState(Player::STATE::Avoidance);
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
        DirectX::XMFLOAT2 ownerFront = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
        ownerFront = XMFloat2Normalize(ownerFront);
        DirectX::XMFLOAT2 addForceDirection = { addForceDirection_.x, addForceDirection_.z };
        addForceDirection = XMFloat2Normalize(addForceDirection * -1.0f);
        
        float cross = XMFloat2Cross(addForceDirection, ownerFront);
        float dot = XMFloat2Dot(addForceDirection, ownerFront) - 1.0f;
        if (cross > 0) owner_->GetTransform()->AddRotationY(dot);
        else owner_->GetTransform()->AddRotationY(-dot);
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

        owner_->PlayBlendAnimation(Player::Animation::HitLarge, false, 1.0f, 0.2f);
        //owner_->PlayBlendAnimation(Player::Animation::KnockDownDeath, false, 1.0f, 0.5f);
        owner_->SetTransitionTime(0.3f);

        // 死亡したので無敵状態にする
        owner_->SetIsInvincible(true);
    }

    // ----- 更新 -----
    void DeathState::Update(const float& elapsedTime)
    {
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
}

// ----- 回避 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void AvoidanceState::Initialize()
    {
        // フラグをリセットする
        owner_->ResetFlags();

        // アニメーション設定
        SetAnimation();

        // 移動方向を算出
        CalcMoveDirection();

        // 無敵状態にする
        owner_->SetIsInvincible(true);

        // 変数初期化
        addForceData_.Initialize(0.15f, 0.27f, 0.4f);
        isRotating_ = false;

        isFirstTime_ = false;
    }

    // ----- 更新 -----
    void AvoidanceState::Update(const float& elapsedTime)
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

        if(owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void AvoidanceState::Finalize()
    {
        // 変数をリセットしておく
        isFirstTime_ = true;

        owner_->SetIsInvincible(false);
    }

    // ----- 回転処理 -----
    void AvoidanceState::Turn(const float& elapsedTime)
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
    void AvoidanceState::ResetState()
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

        // 変数初期化
        addForceData_.Initialize(0.15f, 0.27f, 0.4f);
    }

    // ----- 先行入力処理 -----
    const bool AvoidanceState::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();


#if 1
        const float nextInputStartFrame = 0.5f; // 先行入力開始フレーム

        if (animationSeconds > nextInputStartFrame)
        {
            if (owner_->GetComboAttack0KeyDown())
            {
                owner_->SetNextInput(Player::NextInput::ComboAttack0);
            }

            // 回避
            if (owner_->GetAvoidanceKeyDown() &&
                owner_->GetAnimationIndex() != static_cast<int>(Player::Animation::RollBack))
            {
                owner_->SetNextInput(Player::NextInput::Avoidance);

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
                if (owner_->GetComboAttack0KeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::ComboAttack0);
                }
                // 回避
                if (owner_->GetAvoidanceKeyDown())
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
                if (owner_->GetComboAttack0KeyDown())
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
                if (owner_->GetComboAttack0KeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::ComboAttack0);
                }
                // 回避
                if (owner_->GetAvoidanceKeyDown())
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
                if (owner_->GetComboAttack0KeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::ComboAttack0);
                }
                // 回避
                if (owner_->GetAvoidanceKeyDown())
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
        case Player::Animation::RollForward:// 前
        {
            // 回避の先行入力がある場合
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
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
                        // 移動値がある状態で RightShoulderが押されていれば 走りに遷移
                        if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                        {
                            owner_->ChangeState(Player::STATE::Run);
                            return true;
                        }

                        // 歩きに遷移
                        owner_->ChangeState(Player::STATE::Walk);
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
                        // 移動値がある状態で RightShoulderが押されていれば 走りに遷移
                        if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                        {
                            owner_->ChangeState(Player::STATE::Run);
                            return true;
                        }

                        // 歩きに遷移
                        owner_->ChangeState(Player::STATE::Walk);
                        return true;
                    }
                }
            }
        }
            break;
        case Player::Animation::RollRight:
        {
            // 回避の先行入力がある場合
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
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
                        // 移動値がある状態で RightShoulderが押されていれば 走りに遷移
                        if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                        {
                            owner_->ChangeState(Player::STATE::Run);
                            return true;
                        }

                        // 歩きに遷移
                        owner_->ChangeState(Player::STATE::Walk);
                        return true;
                    }
                }
            }
        }
            break;
        case Player::Animation::RollLeft:
        {
            // 回避の先行入力がある場合
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
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
                        // 移動値がある状態で RightShoulderが押されていれば 走りに遷移
                        if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                        {
                            owner_->ChangeState(Player::STATE::Run);
                            return true;
                        }

                        // 歩きに遷移
                        owner_->ChangeState(Player::STATE::Walk);
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
    void AvoidanceState::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        switch (static_cast<Player::Animation>(owner_->GetAnimationIndex()))
        {
        case Player::Animation::RollForward:// 前
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
    void AvoidanceState::SetAnimation()
    {
        // --------------------------------------------------
        //  回避を連続して出している場合
        // --------------------------------------------------
        if (isFirstTime_ == false)
        {
            // 前方向のアニメーションを設定する
            owner_->PlayBlendAnimation(Player::Animation::RollForward, false, 1.0f, 0.15f);
            owner_->SetTransitionTime(0.05f);
            return;            
        }

        // 元のアニメーションに応じてブレンドの時間を設定する
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());
        if (animationIndex == Player::Animation::ComboAttack0_0)
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
                    owner_->PlayBlendAnimation(Player::Animation::RollForward, false, animationSpeed, animationStartFrame);
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
            owner_->PlayBlendAnimation(Player::Animation::RollForward, false, animationSpeed, animationStartFrame);
            return;
        }
    }

    // ----- 移動方向算出 -----
    void AvoidanceState::CalcMoveDirection()
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
        const int differenceNum = static_cast<int>(Player::Animation::RollForward);

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
        owner_->PlayBlendAnimation(Player::Animation::Counter, false);
        owner_->SetTransitionTime(0.1f);

        // 攻撃可能にする
        owner_->SetIsAbleAttack(true);

        // カウンター時カメラを使用する
        Camera::Instance().SetUseCounterCamera();

        // 変数初期化
        addForceBack_.Initialize(0.16f, 0.2f, 0.5f);
        addForceFront_.Initialize(0.66f, 0.30f, 1.0f);
        //gamePadVibration_.Initialize(0.3f, 0.3f, 1.0f);
        gamePadVibration_.Initialize(0.3f, 0.2f, 0.5f);

        attackData_.Initialize(0.75f, 1.0f);

        isNextInput_ = false;

        isCounterReaction = false;

        isTurnChecked_ = false;

        const DirectX::XMFLOAT3 pos = owner_->GetJointPosition("spine_02");
        Effect* counterEffect = EffectManager::Instance().GetEffect("Mikiri");
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
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAbleAttack());
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
        // TODO:ここつくる。カウンター
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
        owner_->PlayBlendAnimation(Player::Animation::ParryCounterAttack1, false, 1.0f, 0.35f);        

        // 攻撃可能にする
        owner_->SetIsAbleAttack(true);

        // 無敵状態にする
        owner_->SetIsInvincible(true);

        // 変数初期化
        addForceData_.Initialize(0.35f, 0.3f, 1.0f);
        attackData_.Initialize(0.35f, 0.7f);
    }

    // ----- 更新 -----
    void CounterComboState::Update(const float& elapsedTime)
    {
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // 攻撃判定処理
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAbleAttack());
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

        // 攻撃可能にする
        owner_->SetIsAbleAttack(true);

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
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAbleAttack());
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

    // ----- 先行入力処理 -----
    const bool RunAttackState::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (owner_->GetAvoidanceKeyDown())
        {
            owner_->SetNextInput(Player::NextInput::Avoidance);
        }
        if (animationSeconds > 0.6f)
        {
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
            {
                owner_->ChangeState(Player::STATE::Avoidance);
                return true;
            }
        }

        

        const float changeComboAttack0StateFrame = 0.6f; // ステート切り替え
        if (animationSeconds > changeComboAttack0StateFrame)
        {
            if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_1);
                return true;
            }
        }

        // 移動入力値があればステート切り替え
        const float changeMoveStateFrame = 0.8f;
        if (animationSeconds > changeMoveStateFrame)
        {
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                {
                    owner_->ChangeState(Player::STATE::Run);
                    return true;
                }

                owner_->ChangeState(Player::STATE::Walk);
                return true;
            }
        }

        const float comboAttack0NextInputStartFrame = 0.3f; // 先行入力開始フレーム
        if (animationSeconds > comboAttack0NextInputStartFrame)
        {
            if (owner_->GetComboAttack0KeyDown())
            {
                owner_->SetNextInput(Player::NextInput::ComboAttack0);
            }
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

        // 攻撃可能にする
        owner_->SetIsAbleAttack(true);

        // 変数初期化
        addForceData_.Initialize(0.15f, 0.2f, 1.0f);
        attackData_.Initialize(0.1f, 0.35f);
    }

    // ----- 更新 -----
    void ComboAttack0_0::Update(const float& elapsedTime)
    {
        // 先行入力
        if (CheckNextInput()) return;
        
        // アニメーションの速度設定
        SetAnimationSpeed();

        // 移動処理
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(owner_->GetTransform()->CalcForward(),
                addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // 攻撃判定処理
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAbleAttack());
        owner_->SetIsAttackValid(attackFlag);

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            //owner_->ChangeState(Player::STATE::ComboAttack0_1);
            return;
        }
    }

    // ----- 終了化 -----
    void ComboAttack0_0::Finalize()
    {
    }

    // ----- アニメーション設定 -----
    void ComboAttack0_0::SetAnimation()
    {
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        // oldAnimationが回避の場合攻撃の途中フレームから開始する
        if (animationIndex == Player::Animation::RollForward ||
            animationIndex == Player::Animation::RollBack    ||
            animationIndex == Player::Animation::RollRight   ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_0, false, 1.0f, 0.1f);
        }
        else if (animationIndex == Player::Animation::Run)
        {
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_0, false, 1.0f, 0.1f);
            owner_->SetTransitionTime(0.1f);
        }
        else if (animationIndex == Player::Animation::ComboAttack0_1)
        {
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_0, false, 1.0f, 0.1f);
            owner_->SetTransitionTime(0.1f);
        }
        else
        {
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_0, false);
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

    // ----- 先行入力処理 -----
    const bool ComboAttack0_0::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // 先行入力受付
        if (animationSeconds > 0.1)
        {
            // コンボ攻撃
            if (owner_->GetComboAttack0KeyDown())
            {
                owner_->SetNextInput(Player::NextInput::ComboAttack0);
            }
            // 回避
            if (owner_->GetAvoidanceKeyDown())
            {
                owner_->SetNextInput(Player::NextInput::Avoidance);
            }
        }

        if (animationSeconds > 0.3f)
        {
            // コンボ攻撃
            if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_1);
                return true;
            }
        }

        if(animationSeconds > 0.4f)
        {
            // 回避
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
            {
                owner_->ChangeState(Player::STATE::Avoidance);
                return true;
            }
        }

        if (animationSeconds > 0.6f)
        {
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                {
                    owner_->ChangeState(Player::STATE::Run);
                    return true;
                }

                owner_->ChangeState(Player::STATE::Walk);
                return true;
            }
        }

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

        // 攻撃可能にする
        owner_->SetIsAbleAttack(true);

        // 変数初期化
        addForceData_.Initialize(0.05f, 0.25f, 1.0f);
        attackData_.Initialize(0.06f, 0.3f);
    }

    // ----- 更新 -----
    void ComboAttack0_1::Update(const float& elapsedTime)
    {
        // 先行入力処理
        if (CheckNextInput()) return;

        // アニメーションの速度設定
        SetAnimationSpeed();

        // 移動処理
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // 攻撃判定処理
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAbleAttack());
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

    }

    // ----- アニメーション設定 -----
    void ComboAttack0_1::SetAnimation()
    {
        if (owner_->GetAnimationIndex() == static_cast<int>(Player::Animation::RunAttack1))
        {
            owner_->SetTransitionTime(0.2f);
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_1, false, 1.0f, 0.1f);
        }
        else
        {
            owner_->SetTransitionTime(0.1f);
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_1, false);
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
        
        if (animationSeconds > 0.1f)
        {
            // コンボ攻撃
            if (owner_->GetComboAttack0KeyDown())
            {
                owner_->SetNextInput(Player::NextInput::ComboAttack0);
            }
        }
        if (animationSeconds > 0.1f)
        {
            // 回避
            if (owner_->GetAvoidanceKeyDown())
            {
                owner_->SetNextInput(Player::NextInput::Avoidance);
            }
        }

        // 回避に遷移できるフレーム
        const float changeAvoidanceFrame = 0.25f;
        if (animationSeconds > changeAvoidanceFrame)
        {
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
            {
                owner_->ChangeState(Player::STATE::Avoidance);
                return true;
            }
        }

        const float changeComboAttack0Frame = 0.3f;
        if (animationSeconds > changeComboAttack0Frame)
        {
            if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_2);
                return true;
            }
        }

        // 移動入力値があればステート切り替え
        const float changeMoveStateFrame = 0.5f;
        if (animationSeconds > changeMoveStateFrame)
        {
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                {
                    owner_->ChangeState(Player::STATE::Run);
                    return true;
                }

                owner_->ChangeState(Player::STATE::Walk);
                return true;
            }
        }

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
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_2, false, 1.3f, 0.4f);
        owner_->SetTransitionTime(0.3f);

        // 攻撃可能にする
        owner_->SetIsAbleAttack(true);

        // 変数初期化
        addForceData_.Initialize(0.6f, 0.2f, 1.0f);
        attackData_.Initialize(0.7f, 0.9f);
    }

    // ----- 更新 -----
    void ComboAttack0_2::Update(const float& elapsedTime)
    {
        // 先行入力処理
        if (CheckNextInput()) return;

        // アニメーションの速度設定
        SetAnimationSpeed();

        // 移動処理        
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }        

        // 攻撃判定処理
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAbleAttack());
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

        if (animationSeconds > 0.7f)
        {
            if (owner_->GetComboAttack0KeyDown())
            {
                owner_->SetNextInput(Player::NextInput::ComboAttack0);
            }
        }

        if (animationSeconds > 0.9f)
        {
            if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_3);
                return true;
            }
        }

        if (animationSeconds > 0.7f)
        {
            if (owner_->GetAvoidanceKeyDown())
            {
                owner_->SetNextInput(Player::NextInput::Avoidance);
            }
        }
        if (animationSeconds > 1.1f)
        {
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
            {
                owner_->ChangeState(Player::STATE::Avoidance);
                return true;
            }
        }

        // 移動値があれば 移動へ遷移
        if (animationSeconds > 1.4f)
        {
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                {
                    owner_->ChangeState(Player::STATE::Run);
                    return true;
                }

                owner_->ChangeState(Player::STATE::Walk);
                return true;
            }
        }

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
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_3, false);
        owner_->SetTransitionTime(0.1f);

        // 攻撃可能にする
        owner_->SetIsAbleAttack(true);

        // 変数初期化
        addForceData_.Initialize(0.45f, 0.25f, 0.5f);
        attackData_.Initialize(0.65f, 0.8f);
        isVibration_ = false;
    }

    // ----- 更新 -----
    void ComboAttack0_3::Update(const float& elapsedTime)
    {
        if (owner_->GetAnimationSeconds() > 0.7f)
        {
            if (owner_->GetAvoidanceKeyDown())
            {
                owner_->SetNextInput(Player::NextInput::Avoidance);
            }
        }
        if (owner_->GetAnimationSeconds() > 1.3f)
        {
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
            {
                owner_->ChangeState(Player::STATE::Avoidance);
                return;
            }
        }

        // アニメーションの速度設定
        SetAnimationSpeed();

        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // 攻撃判定処理
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAbleAttack());
        owner_->SetIsAttackValid(attackFlag);

        // コントローラー＆カメラ 振動
        if (owner_->GetAnimationSeconds() > 0.8f && isVibration_ == false)
        {
            Input::Instance().GetGamePad().Vibration(0.2f, 1.0f);
            Camera::Instance().ScreenVibrate(0.1f, 0.2f);

            isVibration_ = true;
        }

        //if (owner_->IsPlayAnimation() == false)
        //if(owner_->GetAnimationSeconds() > 1.6f)
        if(owner_->GetAnimationSeconds() > 1.7f)
        {
            //const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            //const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            //if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            //{
            //    if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
            //    {
            //        owner_->ChangeState(Player::STATE::Run);
            //        return;
            //    }

            //    owner_->ChangeState(Player::STATE::Walk);
            //    return;
            //}

            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void ComboAttack0_3::Finalize()
    {

    }

    // ----- アニメーションの速度設定 -----
    void ComboAttack0_3::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();


    }
}