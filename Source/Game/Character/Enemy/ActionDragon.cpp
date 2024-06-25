#include "ActionDragon.h"
#include "PostProcess/PostProcess.h"
#include "Easing.h"
#include "Input.h"

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

// ----- 死亡行動 -----
namespace ActionDragon
{
    const ActionBase::State DeathAction::Run(const float& elapsedTime)
    {
        switch (owner_->GetStep())
        {
        case 0:// 初期化
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Death, false);

            owner_->SetStep(1);
            break;
        case 1:
            // アニメーションが最後まで再生されたら
            if (owner_->IsPlayAnimation() == false)
            {
                // 死亡ループモーションを流す
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::DeathLoop, false);

                owner_->SetStep(2);
                break;
            }

            break;
        case 2:
            // 死亡しているのでここで放置
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
        return ActionBase::State();
    }
}

// ----- 非戦闘時待機 -----
namespace ActionDragon
{
    const ActionBase::State NonBattleIdleAction::Run(const float& elapsedTime)
    {
        switch (owner_->GetStep())
        {
        case 0:// 初期化
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true);
            //owner_->SetTransitionTime(0.1f);

            timer_ = 0.0f;

            owner_->SetStep(1);
            break;
        case 1:

            timer_ += elapsedTime;
            if (timer_ > 2.0f)
            {
                owner_->SetStep(0);
                return ActionBase::State::Failed;
            }

            break;
        }

        return ActionBase::State();
    }
}

// ----- 非戦闘時歩き -----
namespace ActionDragon
{
    const ActionBase::State NonBattleWalkAction::Run(const float& elapsedTime)
    {
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
        switch (owner_->GetStep())
        {
        case 0:// 初期化
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Roar, false);

            // ラジアルブラーのピクセルシェーダ使用
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

            owner_->SetStep(1);
            break;
        case 1:
            // コントローラー振動更新
            for (int i = 0; i < vibrationNum_; ++i)
            {
                gamePadVibration_[i].Update(owner_->GetAnimationSeconds());
            }

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
        switch (owner_->GetStep())
        {
        case 0:
            //owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackFly0, false);
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::BackStep, false);

            owner_->SetStep(1);
            break;
        case 1:

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetStep(0);

                return ActionBase::State::Failed;
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
        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// 初期化

            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackFly0, false);

            // 変数初期化
            {
                addForceData_[MoveDirection::UpBack].Initialize(0.35f, 0.4f, 1.0f);
                //addForceData_[MoveDirection::DownForward].Initialize(0.0f, 0.7f, 1.0f);
                addForceData_[MoveDirection::DownForward].Initialize(0.0f, 0.65f, 1.0f);

                slowAnimationSpeed_ = 0.7f;
                slowAnimationEndFrame_ = 0.12f;

                // 予備動作用
                easingTimer_ = 0.0f;
                isDonw_ = false;
                isUp_ = false;
            }

            owner_->SetStep(1);
            break;
        case STATE::FlyStart:// 飛び始め
            
            // AddForce
            if (addForceData_[static_cast<int>(MoveDirection::UpBack)].IsAbleAddForce(owner_->GetAnimationSeconds()))
            {
                DirectX::XMFLOAT3 direction = XMFloat3Normalize(owner_->GetTransform()->CalcForward() * -1 - owner_->GetTransform()->CalcUp() * -1);
                owner_->AddForce(direction, addForceData_[static_cast<int>(MoveDirection::UpBack)].GetForce(), addForceData_[static_cast<int>(MoveDirection::UpBack)].GetDecelerationForce());
            }

            //if (owner_->GetAnimationSeconds() > firstAnimationEndFrame_)
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackFly1, false);
                
                // 現在の位置Yを保存する
                savePositionY_ = owner_->GetTransform()->GetPositionY();
                
                owner_->SetStep(2);
                break;
            }

            break;
        case STATE::PreAction:// 予備動作

        {
            const float animatioSeconds = owner_->GetAnimationSeconds();

            if (isDonw_ == false)
            {
                const float totalTime = 0.3f;
                addPositionY_ = Easing::InSine(easingTimer_, totalTime, -2.5f, 0.0f);
                easingTimer_ += elapsedTime;

                owner_->SetAnimationSpeed(0.25f);

                if (easingTimer_ > totalTime)
                {
                    easingTimer_ = 0.0f;
                    isDonw_ = true;
                }
            }
            else if (isUp_ == false)
            {
                const float totalTime = 0.4f;
                addPositionY_ = Easing::InCubic(easingTimer_, totalTime, 4.0f, -2.5f);
                easingTimer_ += elapsedTime;
                
                //owner_->SetAnimationSpeed(0.7f);
                owner_->SetAnimationSpeed(1.0f);
                if (easingTimer_ > totalTime)
                {
                    easingTimer_ = 0.0f;
                    isUp_ = true;
                    //owner_->SetAnimationSpeed(1.0f);
                }
            }

            owner_->GetTransform()->SetPositionY(savePositionY_ + addPositionY_);
        }
            

            //if (owner_->IsPlayAnimation() == false)
            //if (owner_->GetAnimationSeconds() > secondAnimationEndFrame_)
            if(isUp_)
            {
                //owner_->PlayAnimation(Enemy::DragonAnimation::AttackFly2, false, slowAnimationSpeed_);
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackFly2, false, slowAnimationSpeed_);
                //owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackFly2, false, 1.0f);
                        //owner_->SetTransitionTime(2.0f);
                owner_->SetStep(3);
                break;
            }
            

            break;
        case STATE::FlyAttack:
            // AddForce
            if (addForceData_[static_cast<int>(MoveDirection::DownForward)].IsAbleAddForce(owner_->GetAnimationSeconds()))
            {
                DirectX::XMFLOAT3 direction = XMFloat3Normalize(owner_->GetTransform()->CalcForward() - owner_->GetTransform()->CalcUp());
                owner_->AddForce(direction, addForceData_[static_cast<int>(MoveDirection::DownForward)].GetForce(), addForceData_[static_cast<int>(MoveDirection::DownForward)].GetDecelerationForce());
            }

            float posY = std::max(0.0f, owner_->GetTransform()->GetPositionY());
            owner_->GetTransform()->SetPositionY(posY);


            if (owner_->GetAnimationSeconds() > slowAnimationEndFrame_)
            {
                owner_->SetAnimationSpeed(1.0f);
            }

            if (owner_->IsPlayAnimation() == false)
            {
                //owner_->GetTransform()->SetPositionY(0.0f);
                owner_->GetTransform()->SetPosition({});
                
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;

        }

        return ActionBase::State::Run;
    }
}

namespace ActionDragon
{
    const ActionBase::State KnockBackAction::Run(const float& elapsedTime)
    {
        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State SlamAction::Run(const float& elapsedTime)
    {
        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State FrontAttackAction::Run(const float& elapsedTime)
    {
        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State ComboSlamAction::Run(const float& elapsedTime)
    {
        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State ComboChargeAction::Run(const float& elapsedTime)
    {
        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State TurnAttackAction::Run(const float& elapsedTime)
    {
        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State TackleAction::Run(const float& elapsedTime)
    {
        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State RiseAttackAction::Run(const float& elapsedTime)
    {
        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State MoveTurnAction::Run(const float& elapsedTime)
    {
        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State MoveAttackAction::Run(const float& elapsedTime)
    {
        return ActionBase::State();
    }
}
