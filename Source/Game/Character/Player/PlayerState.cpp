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
    }

    // ----- 更新 -----
    void IdleState::Update(const float& elapsedTime)
    {
        // 攻撃入力,回避入力受付
        if (owner_->CheckNextInput(Player::NextInput::None)) return;

        // カウンター受付
        if (owner_->GetCounterStanceKey())
        {
            owner_->ChangeState(Player::STATE::Counter);
            return;
        }

        // 移動値があれば MoveState へ遷移する
        const float aLx = fabsf(Input::Instance().GetGamePad().GetAxisLX());
        const float aLy = fabsf(Input::Instance().GetGamePad().GetAxisLY());
        if (aLx != 0.0f || aLy != 0.0f)
        {
            owner_->ChangeState(Player::STATE::Move);
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
        // アニメーションが設定されていない
        if (owner_->GetBlendAnimationIndex1() == -1)
        {
            owner_->PlayBlendAnimation(Player::Animation::Walk, Player::Animation::Idle, true);
        }
        // 現在一つ目に設定しているモーションがWalkの場合設定し損ねるので対策
        else if (owner_->GetBlendAnimationIndex1() == static_cast<int>(Player::Animation::Walk))
        {
            owner_->PlayBlendAnimation(Player::Animation::Run, Player::Animation::Idle, true);
        }
        // それ以外はここに入る (基本的にここの処理になる)
        else
        {
            owner_->PlayBlendAnimation(Player::Animation::Walk, Player::Animation::Idle, true);
        }

        owner_->SetWeight(1.0f);
    }
}

// ----- 移動 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void MoveState::Initialize()
    {
        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::Idle, Player::Animation::Run, true);
        owner_->SetWeight(0.0f);
    }

    // ----- 更新 -----
    void MoveState::Update(const float& elapsedTime)
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

        // 移動
        owner_->Move(elapsedTime);
    }

    // ----- 終了化 -----
    void MoveState::Finalize()
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
        owner_->PlayAnimation(Player::Animation::GetHit, false);
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
    }

    // ----- 更新 -----
    void AvoidanceState::Update(const float& elapsedTime)
    {
    }

    // ----- 終了化 -----
    void AvoidanceState::Finalize()
    {
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
        owner_->SetWeight(1.0f);

        // 変数初期化
        power_ = 0.4f;
        addForceFrame_ = 0.16f;
        isAddForce_ = false;

        animationSlowStartFrame_ = 0.5f;
        animationSlowEndFrame_ = 0.7f;
    }

    // ----- 更新 -----
    void CounterState::Update(const float& elapsedTime)
    {
        const float currentAnimationFrame = owner_->GetBlendAnimationSeconds();

        // 指定のアニメーションの間の時間を設定する
        if (currentAnimationFrame > animationSlowStartFrame_ && currentAnimationFrame < animationSlowEndFrame_)
        {
            owner_->SetAnimationSpeed(owner_->slowAnimationSpeed_);
        }
        else
        {
            owner_->SetAnimationSpeed(1.0f);
        }

        if (currentAnimationFrame > addForceFrame_ &&
            isAddForce_ == false)
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

                owner_->AddForce(direction, power_);

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

                owner_->AddForce(direction, power_);
            }

            isAddForce_ = true;
        }


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
            if (currentAnimationFrame >= animationSlowEndFrame_) return;

            owner_->ChangeState(Player::STATE::CounterCombo);
            return;
        }
    }

    // ----- 終了化 -----
    void CounterState::Finalize()
    {
    }
}

// ----- カウンター攻撃 -----
namespace PlayerState
{
    // ----- 初期化 -----
    void CounterComboState::Initialize()
    {
        // アニメーション再生
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_3, false);
        owner_->SetWeight(1.0f);

        // 変数初期化
        addForceFrame_ = 0.3f;
        isAddForce_ = false;

        Input::Instance().GetGamePad().Vibration(0.3f, 1.0f);
    }

    // ----- 更新 -----
    void CounterComboState::Update(const float& elapsedTime)
    {
        const float currentAnimationFrame = owner_->GetBlendAnimationSeconds();
        if (currentAnimationFrame > addForceFrame_ &&
            isAddForce_ == false)
        {
            // 前方向に進む
            const DirectX::XMFLOAT3 forwardVec = owner_->GetTransform()->CalcForward();
            owner_->AddForce(forwardVec, 0.7f);

            isAddForce_ = true;
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
    }
}

namespace PlayerState
{
    void ComboAttack0_0::Initialize()
    {
        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_1, false);
        owner_->SetWeight(1.0f);
    }

    void ComboAttack0_0::Update(const float& elapsedTime)
    {
        owner_->CheckNextInput(Player::NextInput::ComboAttack0);

        // アニメーション再生終了
        if (owner_->IsPlayAnimation() == false)
        {
            switch (owner_->GetNextInput())
            {
            case Player::NextInput::Avoidance:
                owner_->ChangeState(Player::STATE::Avoidance);
                return;
            case Player::NextInput::ComboAttack0:
                owner_->ChangeState(Player::STATE::ComboAttack0_1);
                return;
            case Player::NextInput::ComboAttack1:
                owner_->ChangeState(Player::STATE::ComboAttack1_0);
                return;
            }

            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    void ComboAttack0_0::Finalize()
    {
    }
}

namespace PlayerState
{
    void ComboAttack0_1::Initialize()
    {
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_2, false);
        owner_->SetWeight(1.0f);
    }

    void ComboAttack0_1::Update(const float& elapsedTime)
    {
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    void ComboAttack0_1::Finalize()
    {
    }
}

namespace PlayerState
{
    void ComboAttack0_2::Initialize()
    {
    }

    void ComboAttack0_2::Update(const float& elapsedTime)
    {
    }

    void ComboAttack0_2::Finalize()
    {
    }
}

namespace PlayerState
{
    void ComboAttack0_3::Initialize()
    {
    }

    void ComboAttack0_3::Update(const float& elapsedTime)
    {
    }

    void ComboAttack0_3::Finalize()
    {
    }
}

namespace PlayerState
{
    void ComboAttack1_0::Initialize()
    {
    }

    void ComboAttack1_0::Update(const float& elapsedTime)
    {
    }

    void ComboAttack1_0::Finalize()
    {
    }
}

namespace PlayerState
{
    void ComboAttack1_1::Initialize()
    {
    }

    void ComboAttack1_1::Update(const float& elapsedTime)
    {
    }

    void ComboAttack1_1::Finalize()
    {
    }
}

namespace PlayerState
{
    void ComboAttack1_2::Initialize()
    {
    }

    void ComboAttack1_2::Update(const float& elapsedTime)
    {
    }

    void ComboAttack1_2::Finalize()
    {
    }
}