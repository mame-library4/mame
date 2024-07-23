#include "ActionDragon.h"
#include "PostProcess/PostProcess.h"
#include "Easing.h"
#include "Input.h"
#include "Camera.h"
#include "../Player/PlayerManager.h"

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

// ----- 死亡行動 -----
namespace ActionDragon
{
    const ActionBase::State DeathAction::Run(const float& elapsedTime)
    {
#ifdef _DEBUG // TODO: 消す
        if (owner_->GetHealth() > 0)
        {
            Camera::Instance().SetTargetOffset({ 0,-1,0 });
            Camera::Instance().SetCameraOffset({ 0.0f,2.5f,0.0f });
            PlayerManager::Instance().SetUseCollisionDetection(true);

            owner_->SetStep(0);
            return ActionBase::State::Failed;
        }
#endif

        // Stateによってターゲット位置を設定する
        if (Camera::Instance().GetUseDeathCamera())
        {
            std::string nodeName = "";

            if (owner_->GetStep() >= 3)
                nodeName = "root";
            else
                nodeName = "Dragon15_spine2";

            const DirectX::XMFLOAT3 neckPos = owner_->GetJointPosition(nodeName, owner_->GetScaleFactor());
            Camera::Instance().SetTarget(neckPos);
        }

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// 初期化
        {
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Death, false);

            // 押し出し判定を無効化する
            PlayerManager::Instance().SetUseCollisionDetection(false);

            // カメラの各種項目を設定する
            Camera::Instance().SetUseDeathCamera(true);
            Camera::Instance().SetTargetOffset({ 0.0f, -7.0f, 0.0f });
            Camera::Instance().SetCameraOffset({ 0.0f, 7.0f, 0.0f });
            Camera::Instance().SetLength(10.0f);
            DirectX::XMFLOAT3 cameraRotate = {};
            cameraRotate.y = DirectX::XMConvertToRadians(rand() % 360);
            Camera::Instance().GetTransform()->SetRotation(cameraRotate);
            oldCameraRotate_ = cameraRotate;

            // 変数初期化
            easingTimer_ = 0.0f;
            timer_ = 0.0f;
            isAddTimer_ = false;

            // ステート変更
            SetState(STATE::FirstCamera);
        }
            break;
        case STATE::FirstCamera:// １つ目のカメラ
        {
            const float totalFrame = 2.0f;
            easingTimer_ += elapsedTime;
            easingTimer_ = std::min(easingTimer_, totalFrame);

            DirectX::XMFLOAT3 cameraRotate = {};
            cameraRotate.x = DirectX::XMConvertToRadians(cameraRotate.x);
            cameraRotate.y = Easing::InSine(easingTimer_, totalFrame, -40.0f, 0.0f);
            cameraRotate.y = oldCameraRotate_.y + DirectX::XMConvertToRadians(cameraRotate.y);

            Camera::Instance().GetTransform()->SetRotation(cameraRotate);

            // 指定のフレームを超えたら次に進む
            const float changeStateFrame = 2.0f;
            if (owner_->GetAnimationSeconds() > changeStateFrame)
            {
                DirectX::XMFLOAT3 cameraRotate = {};
                cameraRotate.x = DirectX::XMConvertToRadians(20.0f);
                cameraRotate.y = DirectX::XMConvertToRadians(rand() % 360);
                Camera::Instance().GetTransform()->SetRotation(cameraRotate);
                oldCameraRotate_ = cameraRotate;

                easingTimer_ = 0.0f;

                // ステート変更
                SetState(STATE::SecondCamera);
            }
        }
            break;
        case STATE::SecondCamera:// ２つ目のカメラ
        {
            const float totalFrame = 2.2f;
            easingTimer_ += elapsedTime;
            easingTimer_ = std::min(easingTimer_, totalFrame);

            DirectX::XMFLOAT3 cameraRotate = {};
            cameraRotate.x = Easing::InSine(easingTimer_, totalFrame, 30.0f, 20.0f);
            cameraRotate.y = Easing::InSine(easingTimer_, totalFrame, 30.0f, 0.0f);
            cameraRotate.x = DirectX::XMConvertToRadians(cameraRotate.x);
            cameraRotate.y = oldCameraRotate_.y + DirectX::XMConvertToRadians(cameraRotate.y);

            Camera::Instance().GetTransform()->SetRotation(cameraRotate);

            // 指定のフレームを超えたら次に進む
            const float changeStateFrame = 4.4f;
            if (owner_->GetAnimationSeconds() > changeStateFrame)
            {
                easingTimer_ = 0.0f;

                // ステート変更
                SetState(STATE::ThirdCamera);
            }
        }
            break;
        case STATE::ThirdCamera:// ３つ目のカメラ
        {
            const float totalFrame = 2.5f;
            easingTimer_ += elapsedTime;
            easingTimer_ = std::min(easingTimer_, totalFrame);


            //const float length = Easing::InSine(easingTimer_, totalFrame, 12.0f, 5.0f);
            const float length = 6.5f;
            Camera::Instance().SetLength(length);

            // rotate
            DirectX::XMFLOAT3 cameraRotate = {};
            cameraRotate.x = DirectX::XMConvertToRadians(-30.0f);
            cameraRotate.y = Easing::OutSine(easingTimer_, totalFrame, 220.0f, 250.0f);
            cameraRotate.y = DirectX::XMConvertToRadians(cameraRotate.y);
            Camera::Instance().GetTransform()->SetRotation(cameraRotate);

            // fov
            const float fov = Easing::InSine(easingTimer_, totalFrame, 50.0f, 45.0f);
            Camera::Instance().SetFov(fov);

            // focusOffset
            DirectX::XMFLOAT3 offset = {};
            offset.x = Easing::InSine(easingTimer_, totalFrame, 1.0f, 0.0f);
            offset.y = Easing::InSine(easingTimer_, totalFrame, -5.0f, -7.0f);
            offset.z = Easing::InSine(easingTimer_, totalFrame, 1.0f, 0.0f);
            Camera::Instance().SetTargetOffset(offset);

            if (owner_->GetAnimationIndex() == static_cast<int>(Enemy::DragonAnimation::Death) &&
                owner_->IsPlayAnimation() == false)
            {
                // 死亡ループモーションを流す
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::DeathLoop, false);

                // タイマー加算
                isAddTimer_ = true;
            }
            // タイマー加算
            if (isAddTimer_)
            {
                timer_ += elapsedTime;
                const float maxTime = 1.0f;
                if (timer_ > maxTime)
                {
                    if (Camera::Instance().GetUseDeathCamera() == false)
                    {
                        Camera::Instance().GetTransform()->SetRotation({ 0, 0, 0 });
                        Camera::Instance().SetLength(6.0f);
                        Camera::Instance().SetTargetOffset({ 0,-1,0 });
                        Camera::Instance().SetCameraOffset({ 0.0f,2.5f,0.0f });
                        Camera::Instance().SetFov(45.0f);

                        // ステート変更
                        SetState(STATE::Death);
                    }

                    // カメラを元に戻す
                    Camera::Instance().SetUseDeathCamera(false);
                }
            }
        }
            break;
        case STATE::Death:// 死亡ループ
            // 死亡しているので何もしない
            break;
        }

        return ActionBase::State::Run;
    }
}

// ----- 怯み行動 -----
namespace ActionDragon
{
    const ActionBase::State FlinchAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        const float animationSpeed = 1.5f;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// 初期化
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::CriticalStart, false);
            owner_->SetTransitionTime(0.1f);

            // 変数初期化
            addForceData_.Initialize(0.01, 0.2f, 1.5f);
            loopCounter_ = 0;
            maxLoopNum_ = 4;

            // ステート変更
            SetState(STATE::FlinchStart);

            break;
        case STATE::FlinchStart:// 怯みスタート( 倒れこみ )
        {
            // 移動処理
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                DirectX::XMFLOAT3 ownerRight = owner_->GetTransform()->CalcRight() * -1;

                owner_->AddForce(ownerRight, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            // 指定のフレームを超えたら次に進む
            const float animationEndFrame = 1.0f;
            if (owner_->GetAnimationSeconds() > animationEndFrame)
            {
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::CriticalLoop, false, animationSpeed);
                owner_->SetTransitionTime(0.1f);
                // ステート変更
                SetState(STATE::FlinchLoop);
            }
        }

            break;
        case STATE::FlinchLoop:// 怯みループ

            if (owner_->IsPlayAnimation() == false)
            {
                if (loopCounter_ > maxLoopNum_)
                {
                    owner_->PlayAnimation(Enemy::DragonAnimation::CriticalEnd, false, animationSpeed);
                    // ステート変更
                    SetState(STATE::FlinchEnd);
                }
                else
                {
                    owner_->PlayAnimation(Enemy::DragonAnimation::CriticalLoop, false, animationSpeed);
                }

                ++loopCounter_;                
            }

            break;
        case STATE::FlinchEnd:// 怯み終わり            

            // アニメーション速度設定
            SetAnimationSpeed();

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetIsFlinch(false);
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }

    // ----- アニメーション速度設定 -----
    void FlinchAction::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > 1.5f)
        {
            owner_->SetAnimationSpeed(1.0f);
        }
    }
}

// ----- 飛行時怯み行動 -----
namespace ActionDragon
{
    const ActionBase::State FlyFlinchAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        const float animationSpeed = 1.5f;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// 初期化
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::CriticalStart, false);
            owner_->SetTransitionTime(0.35f);

            // ルートモーションの移動値をなくす
            owner_->SetUseRootMotionMovement(false);

            // 変数初期化
            easingTimer_ = 0.0f;
            oldPositionY_ = owner_->GetTransform()->GetPositionY() + 0.5f;
            loopCounter_ = 0;
            maxLoopNum_ = 4;

            // ステート変更
            SetState(STATE::FlinchStart);

            break;
        case STATE::FlinchStart:// 怯み開始
        {
            const float totalFrame = 0.6f;
            easingTimer_ += elapsedTime;
            easingTimer_ = std::min(easingTimer_, totalFrame);
            const float posY = Easing::InSine(easingTimer_, totalFrame, 0.7f, oldPositionY_);
            owner_->GetTransform()->SetPositionY(posY);
        }

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetUseRootMotionMovement(true);

                owner_->PlayAnimation(Enemy::DragonAnimation::CriticalLoop, false, animationSpeed);
                               
                // ステート変更
                SetState(STATE::FlinchLoop);
            }

            break;
        case STATE::FlinchLoop:// 怯みループ
            // Y値を0.0fで固定する
            owner_->GetTransform()->SetPositionY(0.0f);

            if (owner_->IsPlayAnimation() == false)
            {
                if (loopCounter_ > maxLoopNum_)
                {
                    owner_->PlayAnimation(Enemy::DragonAnimation::CriticalEnd, false, animationSpeed);

                    // ステート変更
                    SetState(STATE::FlinchEnd);
                }
                else
                {
                    owner_->PlayAnimation(Enemy::DragonAnimation::CriticalLoop, false, animationSpeed);
                }

                ++loopCounter_;
            }

            break;
        case STATE::FlinchEnd:// 怯み終わり
            // アニメーション速度設定
            SetAnimationSpeed();

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetIsFlinch(false);
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
            break;
        }

        return ActionBase::State::Run;
    }

    // ----- アニメーションの速度設定 -----
    void FlyFlinchAction::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > 1.5f)
        {
            owner_->SetAnimationSpeed(1.0f);
        }
    }
}

// ----- 非戦闘時待機 -----
namespace ActionDragon
{
    const ActionBase::State NonBattleIdleAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// 初期化
            // アニメーション設定
            SetAnimation();

            timer_ = 0.0f;

            owner_->SetStep(1);
            break;
        case 1:

            timer_ += elapsedTime;
            if (timer_ > 3.0f)
            {
                owner_->SetStep(0);
                return ActionBase::State::Failed;
            }

            break;
        }

        return ActionBase::State();
    }

    // ----- アニメーション設定 -----
    void NonBattleIdleAction::SetAnimation()
    {
        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());

        if (animationIndex == Enemy::DragonAnimation::AttackTurn0)
        {
            //owner_->SetTransitionTime(0.3f);
            owner_->SetTransitionTime(0.15f);
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true);
            //owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true, 1.0f, 0.45f);
            //owner_->PlayAnimation(Enemy::DragonAnimation::Idle0, true);
        }
        else if (animationIndex == Enemy::DragonAnimation::AttackTackle3)
        {
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true);
            owner_->SetTransitionTime(0.15f);
        }
        else
        {
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true);
        }
    }
}

// ----- 非戦闘時歩き -----
namespace ActionDragon
{
    const ActionBase::State NonBattleWalkAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// 初期化
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Walk, true);

            owner_->SetStep(1);
            break;
        case 1:
            break;
        }

        return ActionBase::State();
    }
}

// ----- 咆哮 -----
namespace ActionDragon
{
    const ActionBase::State RoarAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// 初期化
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Roar, false);

            // ラジアルブラーのピクセルシェーダ使用
            PostProcess::Instance().SetUseRadialBlur();

            // 咆哮した。
            owner_->SetIsRoar(true);

            // 変数初期化
            gamePadVibration_[0].Initialize(1.0f, 0.4f, 0.5f);
            gamePadVibration_[1].Initialize(1.55f, 0.9f, 0.7f);
            gamePadVibration_[2].Initialize(3.0f, 2.0f, 1.0f);

            blurStartFrame_ = 3.0f;
            blurEndFrame_ = 5.0f;
            maxBlurPower_ = 0.01f;
            maxBlurTime_ = 0.2f;
            blurTimer_ = 0.0f;

            isPlayerFilnch_ = false;

            owner_->SetStep(1);
            break;
        case 1:
            // コントローラー振動更新
            for (int i = 0; i < vibrationNum_; ++i)
            {
                gamePadVibration_[i].Update(owner_->GetAnimationSeconds());
            }

            if (owner_->GetAnimationSeconds() > 4.1f && isPlayerFilnch_ == false)
            {
                PlayerManager::Instance().GetPlayer()->ChangeState(Player::STATE::Flinch);

                isPlayerFilnch_ = true;
            }

            // ラジアルブラー更新
            UpdateBlur(elapsedTime);

            // アニメーションが再生しきったら終了
            if (owner_->IsPlayAnimation() == false)
            {
                // フラグリセット
                PostProcess::Instance().SetUseRadialBlur(false);

                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

        break;
        }

        return ActionBase::State::Run;
    }

    // ----- ラジアルブラー更新 -----
    void RoarAction::UpdateBlur(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // ブラー終了フレームを過ぎたら、ブラーを緩める
        if (animationSeconds > blurEndFrame_)
        {
            PostProcess::Instance().GetConstants()->GetData()->blurPower_ =
                Easing::InSine(blurTimer_, maxBlurTime_, maxBlurPower_, 0.0f);

            blurTimer_ -= elapsedTime;
            blurTimer_ = std::max(blurTimer_, 0.0f);
        }
        // ブラー開始フレームを過ぎたら、ブラーをかける
        else if (animationSeconds > blurStartFrame_)
        {
            PostProcess::Instance().GetConstants()->GetData()->blurPower_ =
                Easing::InQuint(blurTimer_, maxBlurTime_, maxBlurPower_, 0.0f);

            blurTimer_ += elapsedTime;
            blurTimer_ = std::min(blurTimer_, maxBlurTime_);
        }

        // アニメーションのタイミングに合わせてブラーをもう１段階強くする
        if (animationSeconds > 4.1f)
        {
            maxBlurPower_ = 0.03f;
        }
    }
}

// ----- バックステップ咆哮 -----
namespace ActionDragon
{
    const ActionBase::State BackStepRoarAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// 初期化
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::BackStepRoar, false);

            // ラジアルブラーのピクセルシェーダ使用
            owner_->SetIsRoar(true);

            // 変数初期化
            {
                gamePadVibration_.Initialize(1.35f, 1.0f, 1.0f);

                blurStartFrame_ = 1.35f;
                blurEndFrame_ = 2.35f;
                maxBlurPower_ = 0.03f;
                maxBlurTime_ = 0.2f;
                blurTimer_ = 0.0f;
            }

            owner_->SetStep(1);

            break;
        case 1:
        {
            // コントローラー振動更新
            gamePadVibration_.Update(owner_->GetAnimationSeconds());

            // ラジアルブラー更新
            UpdateBlur(elapsedTime);

            // アニメーションが再生しきったら終了
            if (owner_->IsPlayAnimation() == false)
            {
                // フラグリセット
                owner_->SetIsRoar(false);

                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
        }
        break;
        }

        return ActionBase::State::Run;
    }

    // ----- ラジアルブラー更新 -----
    void BackStepRoarAction::UpdateBlur(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // ブラー終了フレームを過ぎたら、ブラーを緩める
        if (animationSeconds > blurEndFrame_)
        {
            PostProcess::Instance().GetConstants()->GetData()->blurPower_ =
                Easing::InSine(blurTimer_, maxBlurTime_, maxBlurPower_, 0.0f);

            blurTimer_ -= elapsedTime;
            blurTimer_ = std::max(blurTimer_, 0.0f);
        }
        // ブラー開始フレームを過ぎたら、ブラーをかける
        else if (animationSeconds > blurStartFrame_)
        {
            PostProcess::Instance().GetConstants()->GetData()->blurPower_ =
                Easing::InQuint(blurTimer_, maxBlurTime_, maxBlurPower_, 0.0f);

            blurTimer_ += elapsedTime;
            blurTimer_ = std::min(blurTimer_, maxBlurTime_);
        }
    }
}

// ----- バックステップ攻撃 -----
namespace ActionDragon
{
    const ActionBase::State BackStepAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:
            //owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackFly0, false);
            //owner_->PlayBlendAnimation(Enemy::DragonAnimation::BackStep, false);
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::CriticalLoop, false);

            owner_->SetStep(1);
            break;
        case 1:

            if (owner_->IsPlayAnimation() == false)
            {
                //owner_->SetStep(0);
                //
                //return ActionBase::State::Failed;
            }

            break;
        }

        return ActionBase::State();
    }
}

// ----- 空中からたたきつけ攻撃 -----
namespace ActionDragon
{
    const ActionBase::State FlyAttackAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// 初期化

            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackFly0, false);

            // ルートモーションは使わない
            owner_->SetUseRootMotion(false);

            // 変数初期化
            {
                addForceData_[MoveDirection::UpBack].Initialize(0.35f, 0.4f, 1.0f);
                //addForceData_[MoveDirection::DownForward].Initialize(0.0f, 0.7f, 1.0f);
                addForceData_[MoveDirection::DownForward].Initialize(0.0f, 0.65f, 1.0f);

                // SlowAnimation
                slowAnimationSpeed_ = 0.7f;
                slowAnimationEndFrame_ = 0.12f;

                // 予備動作用
                easingTimer_ = 0.0f;
                isDown_ = false;
                isRise_ = false;

                isAttackActive_ = false;
            }

            // ステート変更
            SetStep(STATE::FlyStart);

            break;
        case STATE::FlyStart:// 飛び始め
        {
            // 回転処理
            const float turnStartFrame = 0.6f;
            if (owner_->GetAnimationSeconds() > turnStartFrame)
            {
                owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());
            }

            // 後ろ斜め上方向に移動させる
            if (addForceData_[static_cast<int>(MoveDirection::UpBack)].Update(owner_->GetAnimationSeconds()))
            {
                DirectX::XMFLOAT3 direction = XMFloat3Normalize(owner_->GetTransform()->CalcForward() * -1 - owner_->GetTransform()->CalcUp() * -1);
                owner_->AddForce(direction, addForceData_[static_cast<int>(MoveDirection::UpBack)].GetForce(), addForceData_[static_cast<int>(MoveDirection::UpBack)].GetDecelerationForce());
            }

            // アニメーション再生しきったら
            if (owner_->IsPlayAnimation() == false)
            {
                // 次のアニメーションを設定する
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackFly1, false);

                // 現在の位置Yを保存する
                savePositionY_ = owner_->GetTransform()->GetPositionY();

                // ステート変更
                SetStep(STATE::PreAction);
                break;
            }
        }
            break;
        case STATE::PreAction:// 予備動作
        {
            // 回転処理
            const float turnEndFrame = 1.3f;
            if (owner_->GetAnimationSeconds() < turnEndFrame)
            {
                owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());
            }

            // 予備動作として下にすこし下がる
            if (isDown_ == false)
            {
                const float totalTime = 0.3f;
                addPositionY_ = Easing::InSine(easingTimer_, totalTime, -2.5f, 0.0f);
                easingTimer_ += elapsedTime;

                owner_->SetAnimationSpeed(0.25f);

                if (easingTimer_ > totalTime)
                {
                    easingTimer_ = 0.0f;
                    isDown_ = true;
                }
            }
            // 下に下がったら、上昇する
            else if (isRise_ == false)
            {
                const float totalTime = 0.4f;
                addPositionY_ = Easing::InCubic(easingTimer_, totalTime, 4.0f, -2.5f);
                easingTimer_ += elapsedTime;

                owner_->SetAnimationSpeed(1.0f);
                if (easingTimer_ > totalTime)
                {
                    easingTimer_ = 0.0f;
                    isRise_ = true;
                }
            }

            // 移動値Yの設定
            owner_->GetTransform()->SetPositionY(savePositionY_ + addPositionY_);

            // 上昇終了していたら次にすすむ
            if (isRise_)
            {
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackFly2, false, slowAnimationSpeed_);

                // ステート変更
                SetStep(STATE::FlyAttack);
                break;
            }
        }
            break;
        case STATE::FlyAttack:// 攻撃

            // 攻撃判定処理
            if (owner_->GetAnimationSeconds() > 0.4f)
            {
                if (isAttackActive_) owner_->SetFlyAttackActiveFlag(false);

                isAttackActive_ = false;
            }
            else if (owner_->GetAnimationSeconds() > 0.1f)
            {
                if (isAttackActive_ == false) owner_->SetFlyAttackActiveFlag();

                isAttackActive_ = true;
            }

            // アニメーションが指定のフレームを超えたら
            if (owner_->GetAnimationSeconds() > slowAnimationEndFrame_)
            {
                // アニメーション速度設定
                owner_->SetAnimationSpeed(1.0f);
            }
            
            // 前方斜め下に移動する
            if (addForceData_[static_cast<int>(MoveDirection::DownForward)].Update(owner_->GetAnimationSeconds()))
            {
                DirectX::XMFLOAT3 direction = XMFloat3Normalize(owner_->GetTransform()->CalcForward() - owner_->GetTransform()->CalcUp());
                owner_->AddForce(direction, addForceData_[static_cast<int>(MoveDirection::DownForward)].GetForce(), addForceData_[static_cast<int>(MoveDirection::DownForward)].GetDecelerationForce());
            }

            // 移動値Yの制御 (0.0fより下に行かないようにする)
            float posY = std::max(0.0f, owner_->GetTransform()->GetPositionY());
            owner_->GetTransform()->SetPositionY(posY);

            // アニメーションが再生しきったら終了
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->GetTransform()->SetPositionY(0.0f);
                
                // ステートリセット
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
            break;
        }
        return ActionBase::State::Run;
    }
}

// ----- 吹き飛ばし攻撃 -----
namespace ActionDragon
{
    const ActionBase::State KnockBackAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// 初期化
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackKnockBackStart, false);

            // ルートモーションを使用する
            owner_->SetUseRootMotion(true);

            // 変数初期化
            {
                loopMax_ = rand() % 3;
                //loopMax_ = rand() % 3;
                loopCounter_ = 0;
            }

            // ステート変更
            SetState(STATE::Guard);

            break;
        case STATE::Guard:// ガード
            
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackKnockBackLoop, false);
                
                // ステート変更
                SetState(STATE::Loop);
                break;
            }

            break;
        case STATE::Loop:// ループ

            // アニメーション再生終了
            if (owner_->IsPlayAnimation() == false)
            {
                // まだループする回数が残っている
                if (loopCounter_ < loopMax_)
                {
                    // ステート変更
                    SetState(STATE::LoopInit);
                    break;
                }
                // ループ終了
                else
                {
                    owner_->PlayAnimation(Enemy::DragonAnimation::AttackKnockBackEnd0, false);
                    
                    // ステート変更
                    SetState(STATE::Attack);
                }
                
                break;
            }
            break;
        case STATE::LoopInit:// ループ初期化
            // アニメーション設定
            owner_->PlayAnimation(Enemy::DragonAnimation::AttackKnockBackLoop, false);

            ++loopCounter_;

            // ステート変更
            SetState(STATE::Loop);

            break;
        case STATE::Attack:// 攻撃成功
            if (owner_->IsPlayAnimation() == false)
            {
                // ステートリセット
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        case STATE::Failed:// 攻撃失敗
            break;
        }

        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State SlamAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State FrontAttackAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        return ActionBase::State();
    }
}

// ----- コンボたたきつけ攻撃 -----
namespace ActionDragon
{
    const ActionBase::State ComboSlamAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// 初期化
            // アニメーション再生
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackComboSlam0, false);

            // ルートモーションを使用する
            owner_->SetUseRootMotion (true);

            owner_->SetStep(1);

            break;
        case 1:

            if (owner_->IsPlayAnimation() == false)
            {
                if (rand() % 2)
                {
                    owner_->PlayAnimation(Enemy::DragonAnimation::AttackComboSlam1, false);

                    owner_->SetStep(2);
                }
                else
                {
                    owner_->PlayAnimation(Enemy::DragonAnimation::AttackComboSlamEnd, false);

                    owner_->SetStep(3);
                }                
            }

            break;
        case 2:

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackComboSlamEnd, false);
                owner_->SetStep(3);
            }

            break;
        case 3:

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State ComboChargeAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        return ActionBase::State();
    }
}

// ----- 回転攻撃 -----
namespace ActionDragon
{
    const ActionBase::State TurnAttackAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// 初期化
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackTurnStart, false);

            // ルートモーションを使用する
            owner_->SetUseRootMotion(true);

            // ステート変更
            SetState(STATE::PreAction);

            // 変数初期化
            isAttackActive_ = false;
            addForceData_.Initialize(1.5f, 0.3f, 0.5f);

            break;
        case STATE::PreAction:// 予備動作

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackTurn0, false);
                SetState(STATE::Attack);
            }

            break;
        case STATE::Attack:// 攻撃

            // 移動処理
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            if (owner_->GetAnimationSeconds() > 2.2)
            {
                if(isAttackActive_) owner_->SetTurnAttackActiveFlag(false);
                
                isAttackActive_ = false;
            }
            else if (owner_->GetAnimationSeconds() > 1.2f)
            {
                if(isAttackActive_ == false) owner_->SetTurnAttackActiveFlag();

                isAttackActive_ = true;
            }

            if (owner_->GetAnimationSeconds() > 3.3f)
            //if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
            break;
        }

        return ActionBase::State::Run;
    }
}

// ----- タックル攻撃 -----
namespace ActionDragon
{
    const ActionBase::State TackleAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// 初期化
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackTackle0, false);

            // ルートモーション使用
            owner_->SetUseRootMotion(true);

            // ステート変更
            SetState(STATE::PreAction);

            // 変数初期化
            addForceData_.Initialize(0.15f, 0.6f, 0.7f);
            easingTimer_ = 0.0f;
            isAttackActive_ = false;

            break;
        case STATE::PreAction:// 予備動作
        {
            const float animationSeconds = owner_->GetAnimationSeconds();

            // 回転処理 ( プレイヤーの方向に向く )
            const float turnStateFrame = 0.3f;
            if (animationSeconds > turnStateFrame)
            {
                owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());
            }

            // アニメーション再生終了
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackTackle1, false);
                SetState(STATE::Tackle);
            }
        }
            break;
        case STATE::Tackle:// タックル
        {
            // 移動処理
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            // 攻撃判定有効化
            const float attackActiveStartFrame = 0.2f;
            if (owner_->GetAnimationSeconds() > attackActiveStartFrame && isAttackActive_ == false)
            {
                owner_->SetTackleAttackActiveFlag();
                isAttackActive_ = true;
            }

            // 前足が埋まってしまうので回転で無理やりしてる
            {
                const float maxAngle = -10.0f;
                if (owner_->GetAnimationSeconds() > 0.65f)
                {
                    const float totalFrame = 0.1f;
                    const float angle = Easing::InSine(easingTimer_, totalFrame, maxAngle, 0.0f);
                    easingTimer_ -= elapsedTime;
                    easingTimer_ = std::max(easingTimer_, 0.0f);
                    owner_->GetTransform()->SetRotationX(DirectX::XMConvertToRadians(angle));
                }
                else if (owner_->GetAnimationSeconds() > 0.45f)
                {
                    const float totalFrame = 0.1f;
                    const float angle = Easing::InSine(easingTimer_, totalFrame, maxAngle, 0.0f);
                    easingTimer_ += elapsedTime;
                    easingTimer_ = std::min(easingTimer_, totalFrame);
                    owner_->GetTransform()->SetRotationX(DirectX::XMConvertToRadians(angle));
                }
            }

            // アニメーション再生終了
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->GetTransform()->SetPositionY(0);

                addForceData_.Initialize(0.03f, 0.5f, 0.8f);

                owner_->PlayAnimation(Enemy::DragonAnimation::AttackTackle3, false);
                SetState(STATE::Recovery);
            }
        }
            break;
        case STATE::Recovery:// 後隙 ( 途中まで攻撃判定ある )
        {
            if (owner_->GetAnimationSeconds() > 0.6f)
            {
                if (isAttackActive_)
                {
                    owner_->SetTackleAttackActiveFlag(false);
                    isAttackActive_ = false;
                }
            }

            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            // アニメーション終了
            const float animationEndFrame = 1.9f;
            if (owner_->GetAnimationSeconds() > animationEndFrame)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
        }
            break;
        }

        return ActionBase::State::Run;
    }
}

// ----- 上昇攻撃 -----
namespace ActionDragon
{
    const ActionBase::State RiseAttackAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// 初期化
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackRise, false);



            // 変数初期化
            addForceData_.Initialize(1.9f, 1.5f, 2.0f);
            riseTimer_ = 0.0f;
            isCameraSet_ = false;
            isCameraReset_ = false;

            owner_->GetTransform()->SetPositionY(0);

            owner_->SetStep(1);

            break;
        case 1:// 予備動作

            if(owner_->GetAnimationSeconds() > 1.85f && isCameraSet_ == false)
            {
                // カメラ設定
                Camera::Instance().SetRiseAttackState(0);
                isCameraSet_ = true;
            }

            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                owner_->AddForce({ 0,1,0 }, addForceData_.GetForce(), addForceData_.GetDecelerationForce());


            }

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackRiseLoop, true);
                owner_->SetStep(2);
            }

            break;
        case 2:// 上昇
        {
            if (riseTimer_ < 1.0f)
            {
                owner_->SetIsStageCollisionJudgement(true);
                DirectX::XMFLOAT3 playerPos = PlayerManager::Instance().GetTransform()->GetPosition();
                owner_->GetTransform()->SetPositionX(playerPos.x);
                owner_->GetTransform()->SetPositionZ(playerPos.z);
            }

            riseTimer_ += elapsedTime;
            if (riseTimer_ > 2.0f)
            {
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackRiseEnd, false);
                owner_->SetStep(3);
                owner_->SetIsStageCollisionJudgement(false);
            }
        }
            break;
        case 3:
        {

            // アニメーション速度設定
            /*if (owner_->GetAnimationSeconds() < 1.0f)
            {
                owner_->SetAnimationSpeed(0.6f);

            }
            else if (owner_->GetAnimationSeconds() < 1.6f)            
            {
                owner_->SetAnimationSpeed(0.8f);
            }            
            else
            {
                owner_->SetAnimationSpeed(1.0f);
            }*/

            float ownerPosY = owner_->GetTransform()->GetPositionY();
            const float moveSpeed = 100.0f * elapsedTime;
            ownerPosY -= moveSpeed;
            ownerPosY = std::max(ownerPosY, 0.0f);
            owner_->GetTransform()->SetPositionY(ownerPosY);

            if (owner_->GetAnimationSeconds() > 0.6f && isCameraReset_ == false)
            {
                // カメラ設定
                Camera::Instance().SetRiseAttackState(-1);
                isCameraReset_ = true;
            }

            if (owner_->IsPlayAnimation() == false)
            {


                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
        }
            break;
        }

        return ActionBase::State::Run;
    }
}

namespace ActionDragon
{
    const ActionBase::State MoveTurnAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State MoveAttackAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        return ActionBase::State();
    }
}
