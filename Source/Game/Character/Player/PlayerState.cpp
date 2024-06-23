#include "PlayerState.h"
#include <cmath>
#include "Input.h"
#include "Camera.h"
#include "Easing.h"
#include "MathHelper.h"

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
        int a = owner_->GetAnimationIndex();
        if (a < 0)
        {
            owner_->PlayAnimation(Player::Animation::Idle, true);
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
        owner_->PlayBlendAnimation(Player::Animation::Walk, true);

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
        if (aLx == 0.0f || aLy == 0.0f)
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
}

// ----- 走り -----
namespace PlayerState
{
    // ----- 初期化 -----
    void RunState::Initialize()
    {
        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::Run, true);

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
        if (aLx == 0.0f || aLy == 0.0f)
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
}

// ----- ダメージ -----
namespace PlayerState
{
    // ----- 初期化 -----
    void DamageState::Initialize()
    {
        // アニメーション再生 
        owner_->PlayAnimation(Player::Animation::GetHit, false);

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
        isAddForce_ = false;
    }

    // ----- 更新 -----
    void AvoidanceState::Update(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > addForceFrame_[static_cast<int>(direction_)] && isAddForce_ == false)
        {
            isAddForce_ = true;
            owner_->AddForce(moveDirection_, power_[static_cast<int>(direction_)]);
        }


        if(animationSeconds > changeStateFrame_[static_cast<int>(direction_)])
        {
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                owner_->ChangeState(Player::STATE::Walk);
                return;
            }

            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void AvoidanceState::Finalize()
    {
    }

    // ----- アニメーション設定 -----
    void AvoidanceState::SetAnimation()
    {
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
                    owner_->PlayAnimation(Player::Animation::StepFront, false);
                    //owner_->PlayBlendAnimation(Player::Animation::StepFront, false);
                    return;
                }

                // 右方向
                if (corss < 0)
                {
                    owner_->PlayBlendAnimation(Player::Animation::StepRight, false);
                }
                // 左方向
                else
                {
                    owner_->PlayBlendAnimation(Player::Animation::StepLeft, false);
                }

            }
            // 回転角が９０度よりも大きければ 後,右,左 の三択
            else
            {
                // 回転角が１３５度よりも大きければ 後方向
                if (dot > DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4)
                {
                    //owner_->PlayBlendAnimation(Player::Animation::StepBack, false);
                    owner_->PlayAnimation(Player::Animation::StepBack, false);
                    return;
                }

                // 右方向
                if (corss < 0)
                {
                    owner_->PlayBlendAnimation(Player::Animation::StepRight, false);
                }
                // 左方向
                else
                {
                    owner_->PlayBlendAnimation(Player::Animation::StepLeft, false);
                }
            }
        }
        // 入力値がない場合前方向のアニメーションを設定する
        else
        {
            owner_->PlayAnimation(Player::Animation::StepBack, false);
            //owner_->PlayBlendAnimation(Player::Animation::StepBack, false);
            return;
        }
    }

    // ----- 移動方向算出 -----
    void AvoidanceState::CalcMoveDirection()
    {
        // アニメーションによって移動方向を分ける
        switch (static_cast<Player::Animation>(owner_->GetAnimationIndex()))
        {
        case Player::Animation::StepFront:
            // プレイヤーの前方向
            moveDirection_ = owner_->GetTransform()->CalcForward();
            direction_ = Direction::Fornt;
            
            break;
        case Player::Animation::StepBack:
            // プレイヤーの後ろ方向
            moveDirection_ = owner_->GetTransform()->CalcForward() * -1;
            direction_ = Direction::Back;
            
            break;
        case Player::Animation::StepRight:
            // プレイヤーの右方向
            moveDirection_ = owner_->GetTransform()->CalcRight();
            direction_ = Direction::Right;
            
            break;
        case Player::Animation::StepLeft:
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
        owner_->PlayBlendAnimation(Player::Animation::Counter, false);

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
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_2, false);
        

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
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_0, false, normalAnimationSpeed_);
        

        // フラグをリセットする
        owner_->ResetFlags();

        // 変数初期化
        nextInputStartFrame_ = 0.3f;
        nextInputEndFrame_ = 0.85f;
        nextAttackFrame_ = 0.55f;
        slowAnimationStartFrame_ = 0.3f;
        isSlowAnimation_ = false;   

        moveDirecion_ = owner_->GetTransform()->CalcForward();
        addForceFrame_ = 0.4f;
        power_ = 0.1f;
        isAddForce_ = false;    
    }

    // ----- 更新 -----
    void ComboAttack0_0::Update(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > addForceFrame_ && isAddForce_ == false)
        {
            isAddForce_ = true;
            owner_->AddForce(moveDirecion_, power_, 0.7f);
        }

        // アニメーションの速度を制御する
        if (animationSeconds > slowAnimationStartFrame_ && isSlowAnimation_ == false)
        {
            owner_->SetAnimationSpeed(slowAnimationSpeed_);
            isSlowAnimation_ = true;
        }

        // 先行入力が有効なフレーム
        if (animationSeconds > nextInputStartFrame_ && animationSeconds < nextInputEndFrame_)
        {
            // 先行入力受付
            owner_->CheckNextInput(Player::NextInput::ComboAttack0, nextAttackFrame_);
            owner_->SetTransitionTime(0.1f);
        }
        if (animationSeconds > nextAttackFrame_ && owner_->GetNextInput() != Player::NextInput::None)
        {
            switch (owner_->GetNextInput())
            {
            case Player::NextInput::ComboAttack0:
                owner_->ChangeState(Player::STATE::ComboAttack0_1);
                break;
            case Player::NextInput::ComboAttack1:
                owner_->ChangeState(Player::STATE::ComboAttack1_0);
                break;
            case Player::NextInput::Avoidance:
                owner_->ChangeState(Player::STATE::Avoidance);
                break;
            }
        }


        // アニメーション再生終了
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void ComboAttack0_0::Finalize()
    {
    }
}

// ----- コンボ攻撃0_1 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void ComboAttack0_1::Initialize()
    {
        // アニメーション設定
        SetAnimation();

        // フラグをリセットする
        owner_->ResetFlags();

        // 変数初期化
        nextInputStartFrame_ = 0.1f;
        nextInputEndFrame_ = 0.85f;
        nextAttackFrame_ = 0.6f;
    }

    // ----- 更新 -----
    void ComboAttack0_1::Update(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // 先行入力が有効なフレーム
        if (animationSeconds > nextInputStartFrame_ && animationSeconds < nextInputEndFrame_)
        {
            // 先行入力受付
            owner_->CheckNextInput(Player::NextInput::ComboAttack0, nextAttackFrame_);
            owner_->SetTransitionTime(0.1f);
        }
        if (animationSeconds > nextAttackFrame_ && owner_->GetNextInput() != Player::NextInput::None)
        {
            switch (owner_->GetNextInput())
            {
            case Player::NextInput::ComboAttack0:
                owner_->ChangeState(Player::STATE::ComboAttack0_2);
                break;
            case Player::NextInput::ComboAttack1:
                owner_->ChangeState(Player::STATE::ComboAttack1_0);
                break;
            case Player::NextInput::Avoidance:
                owner_->ChangeState(Player::STATE::Avoidance);
                break;
            }
        }

        // アニメーション再生終了
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

    // ----- アニメーション設定 -----
    void ComboAttack0_1::SetAnimation()
    {
        if (owner_->GetUseBlendAnimation())
        {
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_1, false, 1.0f);
        }
        else
        {
            owner_->PlayAnimation(Player::Animation::ComboAttack0_1, false, 1.0f);
        }
    }
}

// ----- コンボ攻撃0_2 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void ComboAttack0_2::Initialize()
    {
        // アニメーション設定
        SetAnimation();        

        // フラグをリセットする
        owner_->ResetFlags();

        // 変数初期化
        nextInputStartFrame_ = 0.1f;
        nextInputEndFrame_ = 0.9f;
        nextAttackFrame_ = 0.65f;
                
        moveDirecion_ = owner_->GetTransform()->CalcForward();
        addForceFrame_ = 0.35f;
        power_ = 0.5f;
        isAddForce_ = false;
    }

    // ----- 更新 -----
    void ComboAttack0_2::Update(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > addForceFrame_ && isAddForce_ == false)
        {
            isAddForce_ = true;
            owner_->AddForce(moveDirecion_, power_);
        }

        // 先行入力が有効なフレーム
        if (animationSeconds > nextInputStartFrame_ && animationSeconds < nextInputEndFrame_)
        {
            // 先行入力受付
            owner_->CheckNextInput(Player::NextInput::ComboAttack0, nextAttackFrame_);
            owner_->SetTransitionTime(0.1f);
        }
        if (animationSeconds > nextAttackFrame_ && owner_->GetNextInput() != Player::NextInput::None)
        {
            switch (owner_->GetNextInput())
            {
            case Player::NextInput::ComboAttack0:
                owner_->ChangeState(Player::STATE::ComboAttack0_3);
                break;
            case Player::NextInput::ComboAttack1:
                owner_->ChangeState(Player::STATE::ComboAttack1_0);
                break;
            case Player::NextInput::Avoidance:
                owner_->ChangeState(Player::STATE::Avoidance);
                break;
            }
        }

        // アニメーション再生終了
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

    // ----- アニメーション設定 -----
    void ComboAttack0_2::SetAnimation()
    {
        if (owner_->GetUseBlendAnimation())
        {
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_2, false);
        }
        else
        {
            owner_->PlayAnimation(Player::Animation::ComboAttack0_2, false);
        }
    }
}

// ----- コンボ攻撃0_3 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void ComboAttack0_3::Initialize()
    {
        // アニメーション設定
        SetAnimation();        

        // フラグをリセットする
        owner_->ResetFlags();

        // 変数初期化
        moveDirecion_ = owner_->GetTransform()->CalcForward();
        addForceFrame_[0] = 0.15f;
        addForceFrame_[1] = 0.5f;
        power_[0] = 0.2f;
        power_[1] = 0.3f;
        isAddForce_[0] = false;
        isAddForce_[1] = false;
    }

    // ----- 更新 -----
    void ComboAttack0_3::Update(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > addForceFrame_[0] && isAddForce_[0] == false)
        {
            isAddForce_[0] = true;
            owner_->AddForce(moveDirecion_, power_[0], 0.6f);
        }
        if (animationSeconds > addForceFrame_[1] && isAddForce_[1] == false)
        {
            isAddForce_[1] = true;
            owner_->AddForce(moveDirecion_, power_[1]);
        }

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void ComboAttack0_3::Finalize()
    {
    }

    // ----- アニメーション設定 -----
    void ComboAttack0_3::SetAnimation()
    {
        if (owner_->GetUseBlendAnimation())
        {
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_3, false);
        }
        else
        {
            owner_->PlayAnimation(Player::Animation::ComboAttack0_3, false);
        }
    }
}

// ----- コンボ攻撃1_0 -----
namespace PlayerState
{
    void ComboAttack1_0::Initialize()
    {
        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack1_0, false);
        
        // フラグをリセットする
        owner_->ResetFlags();

        // 変数初期化
        moveDirecion_ = owner_->GetTransform()->CalcForward();
        addForceFrame_ = 0.15f;
        power_ = 0.45f;
        isAddForce_ = true;
        //isAddForce_ = false;
    }

    void ComboAttack1_0::Update(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > addForceFrame_ && isAddForce_ == false)
        {
            isAddForce_ = true;
            owner_->AddForce(moveDirecion_, power_);
        }

        if (owner_->IsPlayAnimation() == false)
        {
            //owner_->ChangeState(Player::STATE::Idle);
            owner_->ChangeState(Player::STATE::ComboAttack1_1);
            return;
        }
    }

    void ComboAttack1_0::Finalize()
    {
    }
}

// ----- コンボ攻撃1_1 -----
namespace PlayerState
{
    void ComboAttack1_1::Initialize()
    {
        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack1_1, false);
        
        // 変数初期化
        moveDirecion_ = owner_->GetTransform()->CalcForward();
        addForceFrame_ = 0.2f;
        power_ = 0.4f;
        isAddForce_ = true;
        //isAddForce_ = false;
    }

    void ComboAttack1_1::Update(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > addForceFrame_ && isAddForce_ == false)
        {
            isAddForce_ = true;
            owner_->AddForce(moveDirecion_, power_, 1.5f);
        }

        if (owner_->IsPlayAnimation() == false)
        {
            //owner_->ChangeState(Player::STATE::Idle);
            owner_->ChangeState(Player::STATE::ComboAttack1_2);
            return;
        }
    }

    void ComboAttack1_1::Finalize()
    {
    }
}

// ----- コンボ攻撃1_2 -----
namespace PlayerState
{
    void ComboAttack1_2::Initialize()
    {
        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack1_2, false);
        
        // 変数初期化
        moveDirecion_ = owner_->GetTransform()->CalcForward();
        addForceFrame_ = 0.3f;
        power_ = 0.4f;
        //isAddForce_ = false;
        isAddForce_ = true;
    }

    void ComboAttack1_2::Update(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > addForceFrame_ && isAddForce_ == false)
        {
            isAddForce_ = true;
            owner_->AddForce(moveDirecion_, power_);
        }

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    void ComboAttack1_2::Finalize()
    {
    }
}