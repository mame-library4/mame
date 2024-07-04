#include "PlayerState.h"
#include <cmath>
#include "Input.h"
#include "Camera.h"
#include "Easing.h"
#include "MathHelper.h"
#include "../Enemy/EnemyManager.h"

namespace PlayerState
{
    // ----- 初期化 -----
    void AddForceData::Initialize(const float& addForceFrame, const float& force, const float& decelerationForce)
    {
        addForceFrame_ = addForceFrame;
        force_ = force;
        decelerationForce_ = decelerationForce;
        isAddforce_ = false;
    }

    // ----- 更新 -----
    bool AddForceData::IsAbleAddForce(const float& animationFrame)
    {
        // 既にAddForceしている
        if (isAddforce_) return false;

        // アニメーションのフレームが指定のフレームに達していない
        if (animationFrame < addForceFrame_) return false;

        isAddforce_ = true;
        return true;
    }
}

// ----- 待機 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void IdleState::Initialize()
    {
        // アニメーション設定
        SetAnimation();

        // フラグをリセットする
        owner_->ResetFlags();

        owner_->SetTransitionTime(0.15f);
    }

    // ----- 更新 -----
    void IdleState::Update(const float& elapsedTime)
    {
        // カウンター受付
        if (owner_->GetCounterStanceKey())
        {
            owner_->ChangeState(Player::STATE::Counter);
            return;
        }

        // 攻撃入力,回避入力受付
        if (owner_->CheckNextInput(Player::NextInput::None)) return;

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

        if (animationIndex == Player::Animation::RollForward ||
            animationIndex == Player::Animation::RollBack    ||
            animationIndex == Player::Animation::RollRight   ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->PlayBlendAnimation(Player::Animation::Idle, true);
        }
        else
        {
            owner_->PlayBlendAnimation(Player::Animation::Idle, true);
        }
    }
}

// ----- 歩き -----
namespace PlayerState
{
    // ----- 初期化 -----
    void WalkState::Initialize()
    {
        // アニメーション設定
        SetAnimation();

        owner_->ResetFlags();

        // 最大速度を設定
        owner_->SetMaxSpeed(2.5f);
    }

    // ----- 更新 -----
    void WalkState::Update(const float& elapsedTime)
    {
        // カウンター受付
        if (owner_->GetCounterStanceKey())
        {
            owner_->ChangeState(Player::STATE::Counter);
            return;
        }

        // 攻撃入力,回避入力受付
        if (owner_->CheckNextInput(Player::NextInput::None)) return;

        // 旋回
        owner_->Turn(elapsedTime);

        const float aLx = fabsf(Input::Instance().GetGamePad().GetAxisLX());
        const float aLy = fabsf(Input::Instance().GetGamePad().GetAxisLY());
        if (aLx == 0.0f && aLy == 0.0f)
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
            animationIndex == Player::Animation::RollBack ||
            animationIndex == Player::Animation::RollRight ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->SetTransitionTime(0.3f);
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
        // アニメーション設定
        SetAnimation();

        owner_->ResetFlags();

        // 最大速度を設定
        owner_->SetMaxSpeed(5.0f);
    }

    // ----- 更新 -----
    void RunState::Update(const float& elapsedTime)
    {
        // カウンター受付
        if (owner_->GetCounterStanceKey())
        {
            owner_->ChangeState(Player::STATE::Counter);
            return;
        }

        // 攻撃入力,回避入力受付
        if (owner_->CheckNextInput(Player::NextInput::None)) return;

        // 旋回
        owner_->Turn(elapsedTime);

        const float aLx = fabsf(Input::Instance().GetGamePad().GetAxisLX());
        const float aLy = fabsf(Input::Instance().GetGamePad().GetAxisLY());
        if (aLx == 0.0f && aLy == 0.0f)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

        if ((Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER) == false)
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
            animationIndex == Player::Animation::RollBack ||
            animationIndex == Player::Animation::RollRight ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->SetTransitionTime(0.2f);
        }
        else
        {
            owner_->SetTransitionTime(0.15f);
        }
        owner_->PlayBlendAnimation(Player::Animation::Run, true);
    }
}

// ----- 怯み -----
namespace PlayerState
{
    // ----- 初期化 -----
    void FlinchState::Initialize()
    {
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
        // アニメーション再生 
        owner_->PlayAnimation(Player::Animation::HitLarge, false);

        // フラグをリセットする
        owner_->ResetFlags();
    }

    // ----- 更新 -----
    void DamageState::Update(const float& elapsedTime)
    {
        // アニメーション終了
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void DamageState::Finalize()
    {
    }
}

// ----- 死亡 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void DeathState::Initialize()
    {
    }

    // ----- 更新 -----
    void DeathState::Update(const float& elapsedTime)
    {
    }

    // ----- 終了化 -----
    void DeathState::Finalize()
    {
    }
}

// ----- 回避 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void AvoidanceState::Initialize()
    {
        // アニメーション設定
        SetAnimation();

        // 移動方向を算出
        CalcMoveDirection();

        // フラグをリセットする
        owner_->ResetFlags();

        // 変数初期化
        addForceData_.Initialize(0.15f, 0.27f, 0.4f);
    }

    // ----- 更新 -----
    void AvoidanceState::Update(const float& elapsedTime)
    {
        // 先行入力処理
        if (CheckNextInput()) return;

        // アニメーションの速度設定
        SetAnimationSpeed();

        // 移動処理
        if (addForceData_.IsAbleAddForce(owner_->GetAnimationSeconds()))
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
    }

    // ----- 先行入力処理 -----
    const bool AvoidanceState::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // 先行入力受付
#pragma region 先行入力受付
        switch (static_cast<Player::Animation>(owner_->GetAnimationIndex()))
        {
        case Player::Animation::RollForward:
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
        case Player::Animation::RollBack:
            break;
        case Player::Animation::RollRight:
            break;
        case Player::Animation::RollLeft:
            break;
        }
#pragma endregion 先行入力受付

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
                    Initialize();
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
        case Player::Animation::RollBack:
            break;
        case Player::Animation::RollRight:
            break;
        case Player::Animation::RollLeft:
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
                
            }

            break;
        case Player::Animation::RollRight:// 右
            break;
        case Player::Animation::RollLeft:// 左
            break;
        }
    }

    // ----- アニメーション設定 -----
    void AvoidanceState::SetAnimation()
    {
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        if (animationIndex == Player::Animation::ComboAttack0_0)
        {
            owner_->SetTransitionTime(0.1f);
        }
        else
        {
            owner_->SetTransitionTime(0.05f);
        }

        // プレイヤーの姿勢に合わせてアニメーションの方向を設定する
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
            float corss = XMFloat2Cross(cameraInput, ownerFront);

            // 回転角が９０度よりも小さければ 前,右,左 の三択
            if (dot < DirectX::XM_PIDIV2)
            {
                // 回転角が４５度よりも小さければ 前方向
                if (dot < DirectX::XM_PIDIV4)
                {                    
                    owner_->PlayBlendAnimation(Player::Animation::RollForward, false, 1.0f, 0.15f);
                    return;
                }

                // 右方向
                if (corss < 0)
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollRight, false);
                }
                // 左方向
                else
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollLeft, false);
                }

            }
            // 回転角が９０度よりも大きければ 後,右,左 の三択
            else
            {
                // 回転角が１３５度よりも大きければ 後方向
                if (dot > DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4)
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollBack, false, 0.15f);
                    return;
                }

                // 右方向
                if (corss < 0)
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollRight, false);
                }
                // 左方向
                else
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollLeft, false);
                }
            }
        }
        // 入力値がない場合前方向のアニメーションを設定する
        else
        {
            owner_->PlayBlendAnimation(Player::Animation::RollForward, false, 0.3f);
            return;
        }
    }

    // ----- 移動方向算出 -----
    void AvoidanceState::CalcMoveDirection()
    {        
        // アニメーションによって移動方向を分ける
        switch (static_cast<Player::Animation>(owner_->GetAnimationIndex()))
        {
        case Player::Animation::RollForward:
            // プレイヤーの前方向
            moveDirection_ = owner_->GetTransform()->CalcForward();
            direction_ = Direction::Fornt;
            
            break;
        case Player::Animation::RollBack:
            // プレイヤーの後ろ方向
            moveDirection_ = owner_->GetTransform()->CalcForward() * -1;
            direction_ = Direction::Back;
            
            break;
        case Player::Animation::RollRight:
            // プレイヤーの右方向
            moveDirection_ = owner_->GetTransform()->CalcRight();
            direction_ = Direction::Right;
            
            break;
        case Player::Animation::RollLeft:
            // プレイヤーの左方向
            moveDirection_ = owner_->GetTransform()->CalcRight() * -1;
            direction_ = Direction::Left;
            
            break;
        }
    }
}

// ----- カウンター -----
namespace PlayerState
{
    // ----- 初期化 -----
    void CounterState::Initialize()
    {
        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::ParryCounterAttack0, false);

        // 変数初期化
        power_[Direction::Front] = 0.6f;
        power_[Direction::Back] = 0.4f;
        addForceFrame_[Direction::Front] = 0.66f;
        addForceFrame_[Direction::Back]  = 0.16f;
        //isAddForce_[Direction::Front] = false;
        //isAddForce_[Direction::Back]  = false;
        isAddForce_[Direction::Front] = true;
        isAddForce_[Direction::Back] = true;


        animationSlowStartFrame_ = 0.5f;
        animationSlowEndFrame_ = 0.7f;

        // フラグをリセットする
        owner_->ResetFlags();
    }

    // ----- 更新 -----
    void CounterState::Update(const float& elapsedTime)
    {
        // アニメーションに合わせて前方向に移動処理
        if (isAddForce_[Direction::Front] == false) AddForceFront(elapsedTime);
        
        // アニメーションに合わせて後ろ方向に移動処理
        if (isAddForce_[Direction::Back] == false) AddForceBack(elapsedTime);        


        // アニメーション再生終了
        if (!owner_->IsPlayAnimation())
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

        // カウンター成功
        // TODO:ここつくる。カウンター
        //if (GetAsyncKeyState('B') & 1)
        //if(owner_->GetStrongAttackKeyDown())
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_A)
        {
            //if (currentAnimationFrame >= animationSlowEndFrame_) return;

            owner_->ChangeState(Player::STATE::CounterCombo);
            return;
        }
    }

    // ----- 終了化 -----
    void CounterState::Finalize()
    {
    }

    // ----- 前方向に移動処理 -----
    void CounterState::AddForceFront(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // 指定のアニメーションフレームを超えたら処理をする
        if (animationSeconds > addForceFrame_[Direction::Front])
        {
            const DirectX::XMFLOAT3 direction = owner_->GetTransform()->CalcForward();

            owner_->AddForce(direction, power_[Direction::Front]);

            // この処理は一度だけでいいのでフラグを立てる
            isAddForce_[Direction::Front] = true;
        }
    }

    // ----- 後ろ方向に移動処理 -----
    void CounterState::AddForceBack(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // 指定のアニメーションフレームを超えたら処理をする
        if (animationSeconds > addForceFrame_[Direction::Back])
        {
            // 左スティックの入力があるか判定
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                // カメラから見た左スティックの傾きを適応した方向を算出する
                const DirectX::XMFLOAT3 cameraForward = Camera::Instance().CalcForward();
                const DirectX::XMFLOAT3 cameraRight = Camera::Instance().CalcRight();
                DirectX::XMFLOAT3 direction =
                {
                    aLy * cameraForward.x + aLx * cameraRight.x,
                    0,
                    aLy * cameraForward.z + aLx * cameraRight.z
                };
                direction = XMFloat3Normalize(direction);
                direction = direction * -1;

                owner_->AddForce(direction, power_[Direction::Back]);

                // 一気にスティックを反対方向に向けると、プレイヤーが違う方向を向くので修正
                DirectX::XMFLOAT2 cameraForward_float2 = { -direction.x, -direction.z };
                cameraForward_float2 = XMFloat2Normalize(cameraForward_float2);

                DirectX::XMFLOAT2 ownerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
                ownerForward = XMFloat2Normalize(ownerForward);

                // 外積をしてどちらに回転するのかを判定する
                float forwardCorss = XMFloat2Cross(cameraForward_float2, ownerForward);

                // 内積で回転幅を算出
                float forwardDot = XMFloat2Dot(cameraForward_float2, ownerForward) - 1.0f;

                if (forwardCorss > 0)
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
                const DirectX::XMFLOAT3 forwardVec = owner_->GetTransform()->CalcForward();
                const DirectX::XMFLOAT3 direction = forwardVec * -1.0f;

                owner_->AddForce(direction, power_[Direction::Back]);
            }

            // この処理は一度だけでいいのでフラグを立てる
            isAddForce_[Direction::Back] = true;
        }
    }
}

// ----- カウンター攻撃 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void CounterComboState::Initialize()
    {
        // アニメーション再生
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_1, false);
        

        // 変数初期化
        addForceFrame_ = 0.3f;
        isAddForce_ = true;
        //isAddForce_ = false;

        Input::Instance().GetGamePad().Vibration(0.3f, 1.0f);

        // フラグをリセットする
        owner_->ResetFlags();
    }

    // ----- 更新 -----
    void CounterComboState::Update(const float& elapsedTime)
    {
        //const float currentAnimationFrame = owner_->GetBlendAnimationSeconds();
        //if (currentAnimationFrame > addForceFrame_ &&
        //    isAddForce_ == false)
        //{
        //    // 前方向に進む
        //    const DirectX::XMFLOAT3 forwardVec = owner_->GetTransform()->CalcForward();
        //    owner_->AddForce(forwardVec, 0.7f);

        //    isAddForce_ = true;
        //}

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
    }
}

// ----- コンボ攻撃0_0 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void ComboAttack0_0::Initialize()
    {
        // アニメーション設定
        if (owner_->GetAnimationIndex() == static_cast<int>(Player::Animation::RollForward))
        {
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_0, false, 1.0f, 0.1f);
        }
        else
        {
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_0, false);
        }


        // フラグリセット
        owner_->ResetFlags();

        // 変数初期化
        addForceData_.Initialize(0.15f, 0.2f, 1.0f);
    }

    // ----- 更新 -----
    void ComboAttack0_0::Update(const float& elapsedTime)
    {
        // 先行入力
        if (CheckNextInput()) return;
        
        // アニメーションの速度設定
        SetAnimationSpeed();

        // 移動処理
        if (addForceData_.IsAbleAddForce(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(owner_->GetTransform()->CalcForward(),
                addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

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
        if (animationSeconds >= 0.15)
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

        if (animationSeconds >= 0.3f)
        {
            // コンボ攻撃
            if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_1);
                return true;
            }
        }

        if(animationSeconds >= 0.4f)
        {
            // 回避
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
            {
                owner_->ChangeState(Player::STATE::Avoidance);
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
        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_1, false);

        // フラグリセット
        owner_->ResetFlags();

        // 変数初期化
        addForceData_.Initialize(0.05f, 0.25f, 1.0f);
    }

    // ----- 更新 -----
    void ComboAttack0_1::Update(const float& elapsedTime)
    {
        // 先行入力処理
        if (CheckNextInput()) return;

        // アニメーションの速度設定
        SetAnimationSpeed();


        const float animationSeconds = owner_->GetAnimationSeconds();

        if (addForceData_.IsAbleAddForce(animationSeconds))
        {
            owner_->AddForce(owner_->GetTransform()->CalcForward(),
                addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        if (owner_->GetComboAttack0KeyDown())
        {
            owner_->SetNextInput(Player::NextInput::ComboAttack0);
        }
        if (animationSeconds >= 0.3f)
        {
            if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_2);
                return;
            }
        }

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void ComboAttack0_1::Finalize()
    {

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
        return false;
    }
}

// ----- コンボ攻撃0_2 -----
namespace PlayerState
{
    void ComboAttack0_2::Initialize()
    {
        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_2, false);

        // フラグリセット
        owner_->ResetFlags();

    }
    void ComboAttack0_2::Update(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (owner_->GetComboAttack0KeyDown())
        {
            owner_->SetNextInput(Player::NextInput::ComboAttack0);
        }
        if (animationSeconds >= 0.9f)
        {
            if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_3);
                return;
            }
        }

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }
    void ComboAttack0_2::Finalize()
    {

    }
}

// ----- コンボ攻撃0_3 -----
namespace PlayerState
{
    void ComboAttack0_3::Initialize()
    {
        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_3, false);

        // フラグリセット
        owner_->ResetFlags();
    }
    void ComboAttack0_3::Update(const float& elapsedTime)
    {
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }
    void ComboAttack0_3::Finalize()
    {

    }
}