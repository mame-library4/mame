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
        owner_->SetWeight(1.0f);
        owner_->PlayBlendAnimation(Player::Animation::Walk, Player::Animation::Idle, true);
    }

    // ----- 更新 -----
    void IdleState::Update(const float& elapsedTime)
    {
        owner_->AddWeight(elapsedTime * 4.0f);

        // 攻撃入力受付 ( ステートが変更された場合ここで終了 )
        if (owner_->CheckAttackButton(Player::NextInput::None)) return;

        GamePad gamePad = Input::Instance().GetGamePad();
        float aLX = fabs(gamePad.GetAxisLX());
        float aLY = fabs(gamePad.GetAxisLY());
        if (aLX != 0.0f && aLY != 0.0f)
        {
            owner_->ChangeState(Player::STATE::Move);
            return;
        }        

        // ステートを切り替えるか判定
        //int isChangeState = isInputMove();
        //if (isChangeState)
        //{
        //    // 値が１の場合 歩き なので WalkState へ遷移
        //    if (isChangeState == 1)
        //    {
        //        owner_->ChangeState(Player::STATE::Walk);
        //        return;
        //    }

        //    // 値が２の場合 走り なので RunState へ遷移
        //    if (isChangeState == 2)
        //    {
        //        owner_->ChangeState(Player::STATE::Run);
        //        return;
        //    }
        //}

        // 速度計算
        DirectX::XMFLOAT3 velocity = owner_->GetVelocity();

        velocity.x = std::max(0.0f, velocity.x - 5.0f * elapsedTime);
        velocity.z = std::max(0.0f, velocity.z - 5.0f * elapsedTime);

        owner_->SetVelocity(velocity);




        // 一定時間操作がないと動き出す処理とかを作りたい

        // ------------------------------------------
    }

    // ----- 終了化 -----
    void IdleState::Finalize()
    {
    }

    // ------ 入力値に応じて返す値を変える -----
    int IdleState::isInputMove()
    {
        GamePad gamePad = Input::Instance().GetGamePad();

        float aLX = fabs(gamePad.GetAxisLX());
        float aLY = fabs(gamePad.GetAxisLY());

        // 入力値が 0.5以上だったら走りの状態なので ２ を返す
        if (aLX > 0.5f || aLY > 0.5f) return 2;

        // 入力値が 0 以上だったら歩きの状態なので １ を返す
        if (aLX > 0.0f || aLY > 0.0f) return 1;

        // 入力値なし
        return 0;
    }

}

// ---------- 移動 ----------
#pragma region 移動

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
        // 攻撃入力受付 ( ステートが変更された場合ここで終了 )
        if (owner_->CheckAttackButton(Player::NextInput::None)) return;

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

#if 0
// ----- 歩き -----
namespace PlayerState
{
    // ----- 初期化 -----
    void WalkState::Initialize()
    {
        owner_->PlayBlendAnimation(Player::Animation::Walk, true);
        owner_->SetWeight(1.0f);
    }

    // ----- 更新 -----
    void WalkState::Update(const float& elapsedTime)
    {
        // ステートを切り替えるか判定
        int isChangeState = isInputMove();
        // 値が０の場合入力がないので、移動速度がない場合 IdleState へ遷移
        if (isChangeState == 0)
        {
            // if( moveSpeed <= 0.0f ) owner_->ChangeState(Player::State::Idle); return;

        }
        // 値が２の場合 走り なので RunState へ遷移
        if (isChangeState == 2)
        {
            owner_->ChangeState(Player::STATE::Run);
            return;
        }

        // 弱攻撃
        if (owner_->GetLightAttackKeyDown())
        {
            owner_->ChangeState(Player::STATE::LightAttack0);
            return;
        }

        GamePad& gamePad = Input::Instance().GetGamePad();
        float aLX = gamePad.GetAxisLX();
        float aLY = gamePad.GetAxisLY();

        // 回転
        DirectX::XMFLOAT2 input = { fabs(gamePad.GetAxisLX()), fabs(gamePad.GetAxisLY()) };
        DirectX::XMFLOAT3 cameraFront = Camera::Instance().CalcForward();
        DirectX::XMFLOAT3 cameraRight = Camera::Instance().CalcRight();

        DirectX::XMFLOAT3 direction =
        {
            aLY * cameraFront.x + aLX * cameraRight.x,
            0,
            aLY * cameraFront.z + aLX * cameraRight.z,
        };
        direction = XMFloat3Normalize(direction);
        if (input.x > 0.0f || input.y > 0.0f)
        {


            // カメラの前方向とプレイヤーの前方向を取得
#if 0
            DirectX::XMFLOAT2 cameraForward = { Camera::Instance().CalcForward().x ,Camera::Instance().CalcForward().z };
            cameraForward = XMFloat2Normalize(cameraForward);
#else 
            DirectX::XMFLOAT2 cameraForward = { direction.x, direction.z };
            cameraForward = XMFloat2Normalize(cameraForward);
#endif

            DirectX::XMFLOAT2 playerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
            playerForward = XMFloat2Normalize(playerForward);

            // 外積をしてどちらに回転するのかを判定する
            float forwardCorss = XMFloat2Cross(cameraForward, playerForward);

            // 内積で回転幅を算出
            float forwardDot = XMFloat2Dot(cameraForward, playerForward) - 1.0f;

            if (forwardCorss > 0)
            {
                owner_->GetTransform()->AddRotationY(forwardDot);
            }
            else
            {
                owner_->GetTransform()->AddRotationY(-forwardDot);
            }
        }

        // 速度計算
        DirectX::XMFLOAT3 velocity = owner_->GetVelocity();

        velocity.x = std::min(owner_->GetMaxSpeed(), velocity.x * elapsedTime);
        velocity.z = std::min(owner_->GetMaxSpeed(), velocity.z * elapsedTime);

        owner_->SetVelocity(velocity);
        owner_->GetTransform()->AddPosition(direction * elapsedTime * 2.0f);

        aLX = fabs(gamePad.GetAxisLX());
        aLY = fabs(gamePad.GetAxisLY());

        if (aLX <= 0 && aLY <= 0)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

    }

    // ----- 終了化 -----
    void WalkState::Finalize()
    {
    }

    // ----- 入力値に応じて返す値を変える -----
    int WalkState::isInputMove()
    {
        GamePad gamePad = Input::Instance().GetGamePad();

        float aLX = fabs(gamePad.GetAxisLX());
        float aLY = fabs(gamePad.GetAxisLY());

        // 入力値が 0.5以上だったら走りの状態なので ２ を返す
        if (aLX > 0.5f || aLY > 0.5f) return 2;

        // 入力値が 0 以上だったら歩きの状態なので １ を返す
        if (aLX > 0.0f || aLY > 0.0f) return 1;

        // 入力値なし
        return 0;
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
        owner_->SetWeight(0.0f);

        DirectX::XMFLOAT3 velocity = owner_->GetVelocity();

        velocity.x = 1.5f;
        velocity.z = 1.5f;

        owner_->SetVelocity(velocity);
    }

    // ----- 更新 -----
    void RunState::Update(const float& elapsedTime)
    {
        //owner_->AddWeight(elapsedTime);

        // 弱攻撃
        if (owner_->GetLightAttackKeyDown())
        {
            owner_->ChangeState(Player::STATE::LightAttack0);
            return;
        }

        // 強攻撃
        if (owner_->GetStrongAttackKeyDown())
        {
            owner_->ChangeState(Player::STATE::StrongAttack0);
            return;
        }

        GamePad& gamePad = Input::Instance().GetGamePad();
        float aLX = gamePad.GetAxisLX();
        float aLY = gamePad.GetAxisLY();

        // 回転
        DirectX::XMFLOAT2 input = { fabs(gamePad.GetAxisLX()), fabs(gamePad.GetAxisLY()) };
        DirectX::XMFLOAT3 cameraFront = Camera::Instance().CalcForward();
        DirectX::XMFLOAT3 cameraRight = Camera::Instance().CalcRight();

        DirectX::XMFLOAT3 direction =
        {
            aLY * cameraFront.x + aLX * cameraRight.x,
            0,
            aLY * cameraFront.z + aLX * cameraRight.z,
        };
        direction = XMFloat3Normalize(direction);
        if (input.x > 0.0f || input.y > 0.0f)
        {


            // カメラの前方向とプレイヤーの前方向を取得
#if 0
            DirectX::XMFLOAT2 cameraForward = { Camera::Instance().CalcForward().x ,Camera::Instance().CalcForward().z };
            cameraForward = XMFloat2Normalize(cameraForward);
#else 
            DirectX::XMFLOAT2 cameraForward = { direction.x, direction.z };
            cameraForward = XMFloat2Normalize(cameraForward);
#endif

            DirectX::XMFLOAT2 playerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
            playerForward = XMFloat2Normalize(playerForward);

            // 外積をしてどちらに回転するのかを判定する
            float forwardCorss = XMFloat2Cross(cameraForward, playerForward);

            // 内積で回転幅を算出
            float forwardDot = XMFloat2Dot(cameraForward, playerForward) - 1.0f;

            if (forwardCorss > 0)
            {
                owner_->GetTransform()->AddRotationY(forwardDot);
            }
            else
            {
                owner_->GetTransform()->AddRotationY(-forwardDot);
            }
        }

        // 速度計算
        DirectX::XMFLOAT3 velocity = owner_->GetVelocity();
        const float speed = owner_->GetSpeed() * elapsedTime;
        const float maxSpeed = owner_->GetMaxSpeed();

        float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
        const float weight = std::min(1.0f, length / maxSpeed);
        owner_->SetWeight(weight);

        velocity.x += direction.x * speed;
        velocity.z += direction.z * speed;
        if (fabs(velocity.x) > maxSpeed)
        {
            velocity.x = (velocity.x > 0) ? maxSpeed : -maxSpeed;
        }
        if (fabs(velocity.z) > maxSpeed)
        {
            velocity.z = (velocity.z > 0) ? maxSpeed : -maxSpeed;
        }


        owner_->SetVelocity(velocity);
        owner_->GetTransform()->AddPosition(velocity);

        aLX = fabs(gamePad.GetAxisLX());
        aLY = fabs(gamePad.GetAxisLY());

        if (aLX <= 0 && aLY <= 0)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void RunState::Finalize()
    {
    }
}

#endif

#pragma endregion 移動

// ---------- 回避 ----------
#pragma region 回避

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
        return;
        // アニメーション設定
        //owner_->PlayAnimation(Player::Animation::Protect, false);
    }

    // ----- 更新 -----
    void CounterState::Update(const float& elapsedTime)
    {
        owner_->ChangeState(Player::STATE::Idle);
        return;
        // アニメーション再生終了
        if (!owner_->IsPlayAnimation())
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

        // カウンター成功
        // TODO:ここつくる。カウンター
        if (GetAsyncKeyState('B') & 1)
        {
            owner_->ChangeState(Player::STATE::CounterAttack);
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
    void CounterAttackState::Initialize()
    {
        owner_->PlayAnimation(Player::Animation::StrongAttack0, false);
    }

    // ----- 更新 -----
    void CounterAttackState::Update(const float& elapsedTime)
    {
        // アニメーション終了
        if (!owner_->IsPlayAnimation())
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void CounterAttackState::Finalize()
    {
    }
}
#pragma endregion 回避

// ---------- 弱攻撃 ----------
#pragma region 弱攻撃

// ----- 弱攻撃０ -----
namespace PlayerState
{
    // ----- 初期化 -----
    void LightAttack0State::Initialize()
    {
        owner_->PlayBlendAnimation(Player::Animation::LightAttack0, false);
        owner_->SetWeight(1.0f);

        // 移動速度をリセットする
        owner_->SetVelocity({});

        // 攻撃判定有効化
        owner_->SetAttackFlag();

        // フラグをリセットする
        owner_->ResetFlags();
    }

    // ----- 更新 -----
    void LightAttack0State::Update(const float& elapsedTime)
    {
        // 先行入力受付
        owner_->CheckAttackButton(Player::NextInput::LightAttack);

        // 先行入力がある場合、現在の攻撃フレームが終わった時にステートを切り替える
        if (owner_->GetBlendAnimationSeconds() > comboAttackFrame_)
        {
            // 攻撃判定を無効化する
            if (owner_->GetIsActiveAttackFlag() == true)
            {
                owner_->SetAttackFlag(false);
            }

            if (owner_->GetNextInput() == static_cast<int>(Player::NextInput::LightAttack))
            {
                owner_->ChangeState(Player::STATE::LightAttack1);
                return;
            }
            if (owner_->GetNextInput() == static_cast<int>(Player::NextInput::StrongAttack))
            {
                owner_->ChangeState(Player::STATE::StrongAttack0);
                return;
            }
        }

        // アニメーションが終了したら待機ステートに切り替える
        if (owner_->IsPlayAnimation() == false)
        //if(owner_->GetBlendAnimationSeconds() > animationEndFrame_)
        {
            owner_->ChangeState(Player::STATE::Idle);
        }
    }

    // ----- 終了化 -----
    void LightAttack0State::Finalize()
    {
    }
}

// ----- 弱攻撃１ -----
namespace PlayerState
{
    // ----- 初期化 -----
    void LightAttack1State::Initialize()
    {
        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::LightAttack1, false);
        owner_->SetWeight(1.0f);

        // 攻撃判定有効化
        owner_->SetAttackFlag();

        // フラグをリセットする
        owner_->ResetFlags();
    }

    // ----- 更新 -----
    void LightAttack1State::Update(const float& elapsedTime)
    {
        // 先行入力受付
        owner_->CheckAttackButton(Player::NextInput::LightAttack);

        // 先行入力がある場合、現在の攻撃フレームが終わった時にステートを切り替える
        if (owner_->GetBlendAnimationSeconds() > comboAttackFrame_)
        {
            // 攻撃判定を無効化する
            if (owner_->GetIsActiveAttackFlag() == true)
            {
                owner_->SetAttackFlag(false);
            }

            if (owner_->GetNextInput() == static_cast<int>(Player::NextInput::LightAttack))
            {
                owner_->ChangeState(Player::STATE::LightAttack2);
                return;
            }
            if (owner_->GetNextInput() == static_cast<int>(Player::NextInput::StrongAttack))
            {
                owner_->ChangeState(Player::STATE::StrongAttack0);
                return;
            }
        }

        // アニメーションが終了したら待機ステートに切り替える
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
        }
    }

    // ----- 終了化 -----
    void LightAttack1State::Finalize()
    {
    }
}

// ----- 弱攻撃２ -----
namespace PlayerState
{
    // ----- 初期化 -----
    void LightAttack2State::Initialize()
    {
        owner_->PlayBlendAnimation(Player::Animation::LightAttack2, false);
        owner_->SetWeight(1.0f);

        // 攻撃判定有効化
        owner_->SetAttackFlag();

        // フラグをリセットする
        owner_->ResetFlags();
    }

    // ----- 更新 -----
    void LightAttack2State::Update(const float& elapsedTime)
    {
        // アニメーションが終了したら待機ステートに切り替える
        if (owner_->IsPlayAnimation() == false)
        {
            // 攻撃判定を無効化する
            if (owner_->GetIsActiveAttackFlag() == true)
            {
                owner_->SetAttackFlag(false);
            }

            owner_->ChangeState(Player::STATE::Idle);
        }
    }

    // ----- 終了化 -----
    void LightAttack2State::Finalize()
    {
    }
}

#pragma endregion 弱攻撃

// ---------- 強攻撃 ----------
#pragma region 強攻撃

// ----- 強攻撃０ -----
namespace PlayerState
{
    // ----- 初期化 -----
    void StrongAttack0State::Initialize()
    {
        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::StrongAttack0, false);
        owner_->SetWeight(1.0f);

        // 移動速度をリセットする
        owner_->SetVelocity({});

        // 攻撃判定有効化
        owner_->SetAttackFlag();

        // フラグをリセットする
        owner_->ResetFlags();
    }

    // ----- 更新 -----
    void StrongAttack0State::Update(const float& elapsedTime)
    {
        // 先行入力受付
        owner_->CheckAttackButton(Player::NextInput::StrongAttack);

        // 先行入力がある場合、現在の攻撃フレームが終わった時にステートを切り替える
        if (owner_->GetNextInput() == static_cast<int>(Player::NextInput::StrongAttack) &&
            owner_->GetBlendAnimationSeconds() > comboAttackFrame_)
        {
            // 攻撃判定を無効化する
            if (owner_->GetIsActiveAttackFlag() == true)
            {
                owner_->SetAttackFlag(false);
            }

            owner_->ChangeState(Player::STATE::StrongAttack1);
            return;
        }

        // アニメーションが終了したら待機ステートに切り替える
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
        }
    }

    // ----- 終了化 -----
    void StrongAttack0State::Finalize()
    {
    }
}

// ----- 強攻撃１ -----
namespace PlayerState
{
    // ----- 初期化 -----
    void StrongAttack1State::Initialize()
    {
        // アニメーション設定
        owner_->PlayBlendAnimation(Player::Animation::StrongAttack1, false);
        owner_->SetWeight(1.0f);

        // 攻撃判定有効化
        owner_->SetAttackFlag();

        // フラグをリセットする
        owner_->ResetFlags();
    }

    // ----- 更新 -----
    void StrongAttack1State::Update(const float& elapsedTime)
    {
        // アニメーションが終了したら待機ステートに切り替える
        if (owner_->IsPlayAnimation() == false)
        {
            // 攻撃判定を無効化する
            if (owner_->GetIsActiveAttackFlag() == true)
            {
                owner_->SetAttackFlag(false);
            }

            owner_->ChangeState(Player::STATE::Idle);
        }
    }

    // ----- 終了化 -----
    void StrongAttack1State::Finalize()
    {
    }
}

#pragma endregion 強攻撃

// ----- ダメージ -----
namespace PlayerState
{
    // ----- 初期化 -----
    void DamageState::Initialize()
    {
        // アニメーション再生 
        owner_->PlayAnimation(Player::Animation::Damage1, false);
        //owner_->PlayAnimation(Player::Animation::Damage0, false);
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