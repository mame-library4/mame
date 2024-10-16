#include "ActionDragon.h"
#include "PostProcess/PostProcess.h"
#include "Easing.h"
#include "Input.h"
#include "Camera.h"
#include "../Player/PlayerManager.h"

#include "Projectile/ProjectileManager.h"
#include "Projectile/Fireball.h"

#include "Particle/ParticleManager.h"
#include "Common.h"

#include "sprite.h"
#include "Application.h"

#include "UI/UIFlashOut.h"

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

// ----- SuperNova -----
namespace ActionDragon
{
    const ActionBase::State SuperNovaAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange())
        {
            Finalize();

            return ActionBase::State::Failed;
        }

        switch (owner_->GetStep())
        {
        case 0:
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Nova1, false);

            // 変数初期化
            superNovaParticle_ = new SuperNovaParticle();
            
            isCreateLavaCrawlerParticle_ = false;
            isCreateCoreBurst_           = false;

            scaleLerpTimer_ = 0.0f;

            radialBlurTimer_ = 0.0f;
            preRadialBlurTimer_ = 0.0f;
            intenseBlurFrameCount_ = 0;

            owner_->SetStep(1);

            break;
        case 1:

            // チャージエフェクト生成
            GenerateChargeEffect(elapsedTime);

            // チャージエフェクト更新
            UpdateChargeEffect(elapsedTime);

            // メインエフェクト生成
            GenarateMainEffect(elapsedTime);

            // ラジアルブラー更新
            UpdateRadialBlur(elapsedTime);


            if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetStep(0);

                Finalize();

                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }

    void SuperNovaAction::DrawDebug()
    {
    }

    // ----- チャージエフェクト生成 -----
    void SuperNovaAction::GenerateChargeEffect(const float& elapsedTime)
    {
        // もう既に生成しているためここで終了
        if (isCreateLavaCrawlerParticle_) return;

        // 生成フレームに達していない
        if (owner_->GetAnimationSeconds() <= 0.65f) return;

        // チャージエフェクトとパーティクルを再生する
        DirectX::XMFLOAT3 emitterPosition = owner_->GetJointPosition("Dragon15_neck_1");
        powerEffectHandle_ = EffectManager::Instance().GetEffect("Power")->Play(emitterPosition, 0.1f, 1.0f);
        
        emitterPosition.y = 0.3f;
        superNovaParticle_->PlayLavaCrawlerParticle(elapsedTime, emitterPosition);

        isCreateLavaCrawlerParticle_ = true;
    }

    // ----- メインエフェクト生成 -----
    void SuperNovaAction::GenarateMainEffect(const float& elapsedTime)
    {
        // もう既にメインエフェクトが生成されている
        if (isCreateCoreBurst_) return;
        // 生成フレームに達していない
        if (owner_->GetAnimationSeconds() <= 3.9f) return;
        
        // チャージエフェクトを停止する
        EffectManager::Instance().GetEffect("Power")->Stop(powerEffectHandle_);

        const DirectX::XMFLOAT3 emitterPosition = owner_->GetJointPosition("Dragon15_neck_1");

        // 爆発パーティクル再生
        superNovaParticle_->PlayCoreBurstParticle(elapsedTime, emitterPosition);
        // 爆発エフェクト再生
        Effect* superNovaEffect = EffectManager::Instance().GetEffect("SuperNova");
        superNovaEffect->Play(emitterPosition, 1.3f, 1.0f);

        // 白飛び画像を生成
        UIFlashOut* uiFlashOut = new UIFlashOut();

        // カメラシェイク
        Camera::Instance().ScreenVibrate(0.3f, 1.5f);

        // 地面を這うパーティクルの速度を上げる
        superNovaParticle_->SetLavaCrawlerParticleSpeed(30.0f);

        isCreateCoreBurst_ = true;
    }

    // ----- チャージエフェクト更新 -----
    void SuperNovaAction::UpdateChargeEffect(const float& elapsedTime)
    {
        // まだチャージエフェクトが生成されていない
        if (isCreateLavaCrawlerParticle_ == false) return;
        // メインの爆発エフェクトが生成されているので処理しない
        if (isCreateCoreBurst_) return;

        PostProcess::Instance().SetUseRadialBlur();
        preRadialBlurTimer_ += elapsedTime;
        preRadialBlurTimer_ = std::min(preRadialBlurTimer_, 1.0f);

        // offsetを使って振動を表現する
        const float strength = XMFloatLerp(0.0f, 0.1f, preRadialBlurTimer_);
        const float offset = rand() % 10 * 0.01f;

        PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = strength + offset;
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = maxSampleCount_;

        // ブラーの開始中心点を決める
        const DirectX::XMFLOAT3 dragonNeckPosition = owner_->GetJointPosition("Dragon15_neck_1");
        DirectX::XMFLOAT2 centerPosition = Sprite::ConvertToScreenPos(dragonNeckPosition);
        centerPosition.x /= SCREEN_WIDTH;
        centerPosition.y /= SCREEN_HEIGHT;

        // 画面外にいるときは発生地点を画面中央にする
        if (centerPosition.x > 1.0f || centerPosition.y > 1.0f ||
            centerPosition.x < 0.0f || centerPosition.y < 0.0f)
        {
            centerPosition = { 0.5f, 0.5f };
        }
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->uvOffset_ = centerPosition;

        // エフェクトのサイズを徐々に大きくする
        const float speed = 0.4f;
        scaleLerpTimer_ += speed * elapsedTime;
        scaleLerpTimer_ = std::min(scaleLerpTimer_, 1.0f);

        const float scale = XMFloatLerp(0.1f, 7.0f, scaleLerpTimer_);
        EffectManager::Instance().GetEffect("Power")->SetScale(powerEffectHandle_, scale);
    }

    // ----- ラジアルブラー更新 -----
    void SuperNovaAction::UpdateRadialBlur(const float& elapsedTime)
    {
        // メインの爆発パーティクルが生成されていないので更新しない
        if (isCreateCoreBurst_ == false) return;

        // ラジアルブラーを使用する サンプリング回数は５回
        PostProcess::Instance().SetUseRadialBlur();
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = maxSampleCount_;

        // ブラーの開始中心点を決める
        const DirectX::XMFLOAT3 dragonNeckPosition = owner_->GetJointPosition("Dragon15_neck_1");
        DirectX::XMFLOAT2 centerPosition = Sprite::ConvertToScreenPos(dragonNeckPosition);
        centerPosition.x /= SCREEN_WIDTH;
        centerPosition.y /= SCREEN_HEIGHT;

        // ブラーの開始点が 0.0 ~ 1.0 を超えていた場合真ん中に補正する
        if (centerPosition.x > 1.0f || centerPosition.y > 1.0f ||
            centerPosition.x < 0.0f || centerPosition.y < 0.0f)
        {
            centerPosition = { 0.5f, 0.5f };
        }
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->uvOffset_ = centerPosition;

        // ブラー開始の数フレームを最大強度のブラーをかける
        if (intenseBlurFrameCount_ < intenseBlurFrame_)
        {
            PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = 1.5f;
            ++intenseBlurFrameCount_;
            return;
        }

        // ラジアルブラーの強度を徐々に下げる
        if (radialBlurTimer_ < 2.0f)
        {
            radialBlurTimer_ += elapsedTime;
            radialBlurTimer_ = std::min(radialBlurTimer_, 1.0f);

            const float strength = XMFloatLerp(1.0f, 0.0f, radialBlurTimer_);

            PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = strength;

            return;
        }

        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 1;
    }

    // ----- 終了処理 -----
    void SuperNovaAction::Finalize()
    {
        if (superNovaParticle_ != nullptr)
        {
            ParticleManager::Instance().Remove(superNovaParticle_);
            superNovaParticle_ = nullptr;
        }
        
        EffectManager::Instance().GetEffect("Power")->Stop(powerEffectHandle_);
        
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 1;
    }
}

// ----- 死亡行動 -----
namespace ActionDragon
{
    const ActionBase::State DeathAction::Run(const float& elapsedTime)
    {
        if (owner_->GetStep() == 0)
        {

            owner_->SetStep(1);
        }

        return ActionBase::State::Run;
    }
    void DeathAction::DrawDebug()
    {
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
        //const float animationSpeed = 1.0f;
        //const float animationSpeed = 0.8f;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// 初期化
            // アニメーション設定
            SetAnimation();

            // 怯み時押し出し判定設定
            owner_->SetDownCollisionActiveFlag();

            // 変数初期化
            addForceData_.Initialize(0.01, 0.2f, 1.5f);
            loopCounter_ = 0;
            //maxLoopNum_ = 2;
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
                // フラグをリセット
                owner_->SetIsFlinch(false);
                owner_->SetDownCollisionActiveFlag(false);

                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }

    void FlinchAction::DrawDebug()
    {
    }

    // ----- アニメーション設定 -----
    void FlinchAction::SetAnimation()
    {
#if 0
        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());

        if (animationIndex == Enemy::DragonAnimation::AttackTackle3)
        {
            owner_->SetTransitionTime(0.2f);
        }
        else
        {
            owner_->SetTransitionTime(0.1f);
        }
#endif
        owner_->SetTransitionTime(0.1f);

        owner_->PlayBlendAnimation(Enemy::DragonAnimation::CriticalStart, false);
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

            // 怯み時の押し出し判定
            owner_->SetDownCollisionActiveFlag();

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
                // フラグをリセット
                owner_->SetIsFlinch(false);
                owner_->SetDownCollisionActiveFlag(false);

                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
            break;
        }

        return ActionBase::State::Run;
    }

    void FlyFlinchAction::DrawDebug()
    {
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

namespace ActionDragon
{
    const ActionBase::State PartDestructionFlinchAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// 初期設定
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::GetHitStart, false, 1.0f, 0.1f);
            owner_->SetTransitionTime(0.15f);

            owner_->SetUseRootMotion(false);

            owner_->SetStep(1);

            break;
        case 1:
            if (owner_->GetUseRootMotionMovement() == false)
            {
                if (owner_->GetIsBlendAnimation() == false) owner_->SetUseRootMotion(true);
            }

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::GetHitLoop, false);

                owner_->SetUseRootMotion(false);

                owner_->SetStep(2);
            }

            break;
        case 2:
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::GetHitEnd, false);

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

        return ActionBase::State::Run;
    }
    void PartDestructionFlinchAction::DrawDebug()
    {
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

            // ルートモーションを使用しない
            owner_->SetUseRootMotion(false);

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

    void NonBattleIdleAction::DrawDebug()
    {
    }

    // ----- アニメーション設定 -----
    void NonBattleIdleAction::SetAnimation()
    {
        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());

        // 通常アニメーションを再生する
        if (animationIndex == Enemy::DragonAnimation::AttackTackle3         || // 突進攻撃
            animationIndex == Enemy::DragonAnimation::AttackKnockBackEnd0   || // 吹き飛ばし攻撃
            animationIndex == Enemy::DragonAnimation::AttackFly2            || // 空中からたたきつけ攻撃
            animationIndex == Enemy::DragonAnimation::FireBreathFront       || // ブレス 
            animationIndex == Enemy::DragonAnimation::AttackRiseEnd            // 上昇攻撃
            )
        {
            owner_->PlayAnimation(Enemy::DragonAnimation::Idle0, true);
            return;
        }

        // 回転攻撃
        if (animationIndex == Enemy::DragonAnimation::AttackTurn)
        {
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true, 1.0f, 0.2f);
            owner_->SetTransitionTime(0.2f);
            return;
        }

        // たたきつけ攻撃
        if (animationIndex == Enemy::DragonAnimation::AttackComboSlamEnd)
        {
            owner_->SetTransitionTime(0.1f);
        }

        owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true);
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
    void NonBattleWalkAction::DrawDebug()
    {
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
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::BackStepRoar, false);

            // ラジアルブラーのピクセルシェーダ使用
            PostProcess::Instance().SetUseRadialBlur();

            // 咆哮した。
            owner_->SetIsRoar(true);

            // 変数初期化
            gamePadVibration_.Initialize(1.3f, 1.0f, 1.0f);
            blurTimer_ = 0.0f;

            owner_->SetStep(1);

            break;
        case 1:
            // コントローラー振動
            gamePadVibration_.Update(owner_->GetAnimationSeconds());

            // ブラー更新
            UpdateBlur(elapsedTime);

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

    void RoarAction::DrawDebug()
    {
    }

    // ----- ラジアルブラー更新 -----
    void RoarAction::UpdateBlur(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();
        const float blurStartFrame = 1.3f;
        const float blurEndFrame = 2.3f;
        const float maxBlurTime = 0.2f;
        const float maxBlurPower = 0.03f;

        // ブラー終了フレームを過ぎたら、ブラーを緩める
        if (animationSeconds > blurEndFrame)
        {
            PostProcess::Instance().GetConstants()->GetData()->dummy_ =
                Easing::InSine(blurTimer_, maxBlurTime, maxBlurPower, 0.0f);

            blurTimer_ -= elapsedTime;
            blurTimer_ = std::max(blurTimer_, 0.0f);
        }
        // ブラー開始フレームを過ぎたら、ブラーをかける
        else if (animationSeconds > blurStartFrame)
        {
            PostProcess::Instance().GetConstants()->GetData()->dummy_ =
                Easing::InQuint(blurTimer_, maxBlurTime, maxBlurPower, 0.0f);

            blurTimer_ += elapsedTime;
            blurTimer_ = std::min(blurTimer_, maxBlurTime);
        }
    }
}

// ----- 咆哮(長いやつ) -----
namespace ActionDragon
{
    const ActionBase::State RoarLongAction::Run(const float& elapsedTime)
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
            PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 5;
            PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = 0.0f;

            // 咆哮した。
            owner_->SetIsRoar(true);

            // 変数初期化
            gamePadVibration_[0].Initialize(1.0f, 0.4f, 0.5f);
            gamePadVibration_[1].Initialize(1.55f, 0.9f, 0.7f);
            gamePadVibration_[2].Initialize(3.0f, 2.0f, 1.0f);

            intenseBlurFrameCount_  = 0;
            radialBlurTimer_        = 0.0f;

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
                PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 1;

                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

        break;
        }

        return ActionBase::State::Run;
    }

    void RoarLongAction::DrawDebug()
    {
        ImGui::DragFloat("RadialBlurTimer", &radialBlurTimer_);
        ImGui::DragFloat("RadialBlurSpeed", &radialBlurSpeed_);
        ImGui::DragInt("IntenseBlurFrame", &intenseBlurFrame_);
        ImGui::DragInt("IntenseBlurFrameCounter", &intenseBlurFrameCount_);
        ImGui::DragFloat("MaxStrength", &maxStrength_);
    }

    // ----- ラジアルブラー更新 -----
    void RoarLongAction::UpdateBlur(const float& elapsedTime)
    {
        // ブラーの開始中心点を決める
        const DirectX::XMFLOAT3 dragonNeckPosition = owner_->GetJointPosition("Dragon15_neck_1");
        DirectX::XMFLOAT2 centerPosition = Sprite::ConvertToScreenPos(dragonNeckPosition);
        centerPosition.x /= SCREEN_WIDTH;
        centerPosition.y /= SCREEN_HEIGHT;

        // ブラーの開始点が 0.0 ~ 1.0 を超えていた場合真ん中に補正する
        if (centerPosition.x > 1.0f || centerPosition.y > 1.0f ||
            centerPosition.x < 0.0f || centerPosition.y < 0.0f)
        {
            centerPosition = { 0.5f, 0.5f };
        }
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->uvOffset_ = centerPosition;



        if (owner_->GetAnimationSeconds() > 4.1f)
        {
            // ブラー開始の数フレームを最大強度のブラーをかける
            if (intenseBlurFrameCount_ < intenseBlurFrame_)
            {
                PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = maxStrength_;
                ++intenseBlurFrameCount_;
                return;
            }
            else
            {
                radialBlurTimer_ += radialBlurSpeed_ * elapsedTime;
                radialBlurTimer_ = std::min(radialBlurTimer_, 1.0f);
                const float strength = XMFloatLerp(maxStrength_, 0.0f, radialBlurTimer_);

                PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = strength;
            }
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
    void BackStepAction::DrawDebug()
    {
    }
}

// ----- FlyAttackAction -----
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
            SetAnimation();

            // ダメージ設定
            owner_->SetAttackDamage(50.0f);

            // ルートモーションを使用しない
            owner_->SetUseRootMotion(false);

            // カウンター有効範囲を設定する
            PlayerManager::Instance().GetPlayer()->SetCounterActiveRadius(6.0f);

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
                if(owner_->GetIsAttackActive()) 
                    owner_->SetFlyAttackActiveFlag(false);
            }
            else if (owner_->GetAnimationSeconds() > 0.1f)
            {
                if(owner_->GetIsAttackActive() == false) 
                    owner_->SetFlyAttackActiveFlag();
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


            if(owner_->GetAnimationSeconds() > 2.55f)
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

    void FlyAttackAction::DrawDebug()
    {
    }

    // ----- アニメーション設定 -----
    void FlyAttackAction::SetAnimation()
    {
        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());

        if (animationIndex == Enemy::DragonAnimation::AttackKnockBackEnd1)
        {
            owner_->SetTransitionTime(0.2f);
        }
        else
        {
            owner_->SetTransitionTime(0.1f);
        }

        owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackFly0, false);
    }
}

// ----- KnockBackAction -----
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

            // ルートモーションを使用しない
            owner_->SetUseRootMotion(false);

            // ダメージ設定
            owner_->SetAttackDamage(50.0f);

            // カウンター有効範囲を設定する
            PlayerManager::Instance().GetPlayer()->SetCounterActiveRadius(8.0f);

            // 変数初期化
            loopMax_ = 2;
            loopCounter_ = 0;

            // ステート変更
            SetState(STATE::Guard);

            break;
        case STATE::Guard:// ガード
            
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackKnockBackLoop, false);
                
                // 現在の体力を保存する
                oldHealth_ = owner_->GetHealth();

                // ステート変更
                SetState(STATE::Loop);
                break;
            }

            break;
        case STATE::Loop:// ループ

            // 攻撃をくらったか判定
            if (owner_->GetHealth() != oldHealth_)
            {
                // 攻撃を受けたのでAttackに移行
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackKnockBackEnd0, false);

                // ステート変更
                SetState(STATE::Attack);
            }

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
                    // 攻撃をくらわなかった
                    owner_->PlayAnimation(Enemy::DragonAnimation::AttackKnockBackEnd1, false);
                    
                    // ステート変更
                    SetState(STATE::Failed);
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
            // 攻撃判定処理
            if (owner_->GetAnimationSeconds() > 1.0f)
            {
                if (owner_->GetIsAttackActive())
                    owner_->SetKnockBackAttackActiveFalg(false);
            }
            else if (owner_->GetAnimationSeconds() > 0.36f)
            {
                if (owner_->GetIsAttackActive() == false)
                    owner_->SetKnockBackAttackActiveFalg();
            }

            if(owner_->GetAnimationSeconds() > 1.75f)
            {
                // ステートリセット
                owner_->SetStep(0);

                owner_->SetUseRootMotion(false);

                return ActionBase::State::Complete;
            }

            break;
        case STATE::Failed:// 攻撃失敗

            if (owner_->GetAnimationSeconds() > 1.15f)
            {
                // ステートリセット
                owner_->SetStep(0);

                owner_->SetUseRootMotion(false);

                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }
    void KnockBackAction::DrawDebug()
    {
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
    void SlamAction::DrawDebug()
    {
    }
}

// ----- FireBreath -----
namespace ActionDragon
{
    const ActionBase::State FireBreath::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// 初期化
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::FireBreathFront, false);

            // ダメージ設定
            owner_->SetAttackDamage(50.0f);

            // ルートモーションを使用しない
            owner_->SetUseRootMotion(false);
            
            // 変数初期化
            targetPosition_ = PlayerManager::Instance().GetTransform()->GetPosition();
            isCreateFireball_ = false;

            owner_->SetStep(1);

            break;
        case 1:

            // 回転処理
            if (owner_->GetAnimationSeconds() > 0.25f)
            {
                owner_->Turn(elapsedTime, targetPosition_);
            }


            if (owner_->GetAnimationSeconds() > 0.95 && isCreateFireball_ == false)
            {
                Fireball* fireball = new Fireball();
                
                DirectX::XMFLOAT3 mouthPosition = owner_->GetJointPosition("Dragon15_tongue4");
                DirectX::XMFLOAT3 playerPosition = PlayerManager::Instance().GetTransform()->GetPosition();
                DirectX::XMFLOAT3 direction = XMFloat3Normalize(playerPosition - mouthPosition);

                fireball->Launch(elapsedTime, mouthPosition, direction);

                isCreateFireball_ = true;
            }

            if(owner_->GetAnimationSeconds() > 2.4f)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }
    void FireBreath::DrawDebug()
    {
    }
}

// ----- FireBreathCombo -----
namespace ActionDragon
{
    const ActionBase::State FireBreathCombo::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// 初期化
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::FireBreathLeft, false, 1.0f, 0.14f);
            owner_->SetTransitionTime(0.3f);

            // ダメージ設定
            owner_->SetAttackDamage(50.0f);

            // ルートモーションを使用しない
            owner_->SetUseRootMotion(false);

            // 変数初期化
            targetPosition_ = PlayerManager::Instance().GetTransform()->GetPosition();
            isCreateFireball_ = false;

            // ステート変更
            SetState(STATE::FirstAttack);

            break;
        case STATE::FirstAttack:
        {
            // 回転処理
            if (owner_->GetAnimationSeconds() > 0.25f)
            {
                owner_->Turn(elapsedTime, targetPosition_);
            }

            // 火球発射
            Launch(elapsedTime, 0.95f);

            //if (owner_->IsPlayAnimation() == false)
            if (owner_->GetAnimationSeconds() > 1.7f)
            {
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::FireBreathRight, false, 1.0f, 0.14f);
                owner_->SetTransitionTime(0.25f);

                isCreateFireball_ = false;

                // ステート変更
                SetState(STATE::SecondAttack);
            }
        }
            break;
        case STATE::SecondAttack:
        {
            // 火球発射
            Launch(elapsedTime, 0.95f);

            //if (owner_->IsPlayAnimation() == false)
            if (owner_->GetAnimationSeconds() > 1.7f)
            {
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::FireBreathFront, false, 1.0f, 0.2f);

                isCreateFireball_ = false;

                // ステート変更
                SetState(STATE::ThirdAttack);
            }
        }
            break;
        case STATE::ThirdAttack:
        {
            // 火球発射
            Launch(elapsedTime, 0.95f);

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

    void FireBreathCombo::DrawDebug()
    {
    }

    // ----- 火球発射 -----
    void FireBreathCombo::Launch(const float& elapsedTime, const float& launchFrame)
    {
        // 既に発射している
        if (isCreateFireball_) return;

        // 指定された発射フレームに達していない
        if (owner_->GetAnimationSeconds() < launchFrame) return;


        Fireball* fireball = new Fireball();
        DirectX::XMFLOAT3 tongue3Position = owner_->GetJointPosition("Dragon15_tongue3");
        DirectX::XMFLOAT3 tongue4Position = owner_->GetJointPosition("Dragon15_tongue4");

        DirectX::XMFLOAT3 direction = tongue4Position - tongue3Position;
        direction.y = 0;
        direction = XMFloat3Normalize(direction);

        fireball->Launch(elapsedTime, tongue4Position, direction);

        isCreateFireball_ = true;
    }
}

// ----- ComboSlamAction -----
namespace ActionDragon
{
    const ActionBase::State ComboSlamAction::Run(const float& elapsedTime)
    {
        // 実行中ノードを中断するか
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// 初期化
            // アニメーション再生
            SetAnimation();

            // ダメージ設定
            owner_->SetAttackDamage(50.0f);

            // ルートモーションを使用するが初期化時点では使用しない
            owner_->SetUseRootMotion(false);

            // カウンター有効範囲を設定する
            PlayerManager::Instance().GetPlayer()->SetCounterActiveRadius(6.0f);

            // 変数初期化
            addForceData_.Initialize(1.0f, 0.4f, 1.0f);

            // ステート変更
            SetState(STATE::Attack0);

            break;
        case STATE::Attack0:// 攻撃一発目

            // ルートモーションを使用する
            if (owner_->GetUseRootMotionMovement() == false)
            {
                if (owner_->GetIsBlendAnimation() == false) owner_->SetUseRootMotion(true);
            }

            // 移動処理
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                DirectX::XMFLOAT3 vec = owner_->CalcDirectionToPlayer();
                vec = XMFloat3Normalize({ vec.x, 0.0f, vec.z });
                owner_->AddForce(vec, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            // 回転処理
            if (owner_->GetAnimationSeconds() > 0.7f &&
                owner_->GetAnimationSeconds() < 1.3f)
            {
                owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());
            }

            // 攻撃判定処理
            if (owner_->GetAnimationSeconds() > 1.55f)
            {
                if (owner_->GetIsAttackActive())
                    owner_->SetComboSlamAttackActiveFlag(false);
            }
            else if (owner_->GetAnimationSeconds() > 1.4f)
            {
                if (owner_->GetIsAttackActive() == false)
                    owner_->SetComboSlamAttackActiveFlag();
            }

            // アニメーション再生終了したらステート変更
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackComboSlam1, false);

                // ルートモーションリセット
                owner_->SetUseRootMotion(true);

                // ステート変更
                SetState(STATE::Attack1);
            }

            break;
        case STATE::Attack1:// 攻撃二発目

            // 攻撃判定処理
            if (owner_->GetAnimationSeconds() > 0.75f)
            {
                if (owner_->GetIsAttackActive())
                    owner_->SetComboSlamAttackActiveFlag(false);
            }
            else if (owner_->GetAnimationSeconds() > 0.6f)
            {
                if (owner_->GetIsAttackActive() == false)
                    owner_->SetComboSlamAttackActiveFlag();
            }

            // アニメーション再生終了したらステート変更
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackComboSlamEnd, false);
                
                // ステート変更
                SetState(STATE::Recovery);
            }

            break;
        case STATE::Recovery:// 後隙
        {
            // 指定したフレームを超えたら終了
            const float animationEndFrame = 1.1f;
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

    void ComboSlamAction::DrawDebug()
    {
    }

    // ----- アニメーション設定 -----
    void ComboSlamAction::SetAnimation()
    {
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackComboSlam0, false);
    }
}

// ----- コンボたたきつけ攻撃(軸合わせしてくる) -----
namespace ActionDragon
{
    const ActionBase::State ComboFlySlamAction::Run(const float& elapsedTime)
    {
        switch(static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// 初期化
            // アニメーション再生
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackComboSlam0, false);

            // ダメージ設定
            owner_->SetAttackDamage(50.0f);

            // 最初の段階ではルートモーションを使用しない
            owner_->SetUseRootMotion(false);

            // 変数初期化
            addForceData_.Initialize(1.0f, 0.4f, 1.0f);
            comboNum_ = 0;

            // ステート変更
            SetState(STATE::Attack);

            break;
        case STATE::Attack:

            if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == true)
            {
                owner_->SetUseRootMotion(true);
            }

            // 移動処理
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                DirectX::XMFLOAT3 vec = owner_->CalcDirectionToPlayer();
                vec = XMFloat3Normalize({ vec.x, 0.0f, vec.z });
                owner_->AddForce(vec, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            // 回転処理
            if (owner_->GetAnimationSeconds() > 0.7f &&
                owner_->GetAnimationSeconds() < 1.3f)
            {
                owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());
            }

            // 攻撃判定処理
            if (owner_->GetAnimationSeconds() > 1.55f)
            {
                if (owner_->GetIsAttackActive())
                    owner_->SetComboSlamAttackActiveFlag(false);
            }
            else if (owner_->GetAnimationSeconds() > 1.4f)
            {
                if (owner_->GetIsAttackActive() == false)
                    owner_->SetComboSlamAttackActiveFlag();
            }

            // アニメーション再生終了したらステート変更
            if (comboNum_ < maxComboNum_)
            {
                if(owner_->GetAnimationSeconds() > 1.9f)
                {
                    // ステート変更
                    SetState(STATE::ComboJudge);
                }
            }
            else
            {
                if (owner_->IsPlayAnimation() == false)
                {
                    // ステート変更
                    SetState(STATE::ComboJudge);
                }
            }
            

            break;
        case STATE::ComboJudge:

            if (comboNum_ < maxComboNum_)
            {
                ++comboNum_;

                owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackComboSlam0, false, 1.0f, 0.75f);
                owner_->SetTransitionTime(0.25f);

                owner_->SetUseRootMotion(false);

                addForceData_.Initialize(1.0f, 0.4f, 1.0f);

                // ステート変更
                SetState(STATE::Attack);
            }
            else
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackComboSlamEnd, false);
                owner_->SetTransitionTime(0.1f);

                // ステート変更
                SetState(STATE::Recovery);
            }

            break;
        case STATE::Recovery:
        {
            // 指定したフレームを超えたら終了
            const float animationEndFrame = 1.1f;
            if (owner_->GetAnimationSeconds() > animationEndFrame)
            {
                owner_->SetStep(0);

                owner_->SetUseRootMotion(false);

                return ActionBase::State::Complete;
            }
        }
            break;
        }

        return ActionBase::State::Run;
    }
    void ComboFlySlamAction::DrawDebug()
    {
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
    void ComboChargeAction::DrawDebug()
    {
    }
}

// ----- TurnAttackAction -----
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
            SetAnimation();

            // ダメージ設定
            owner_->SetAttackDamage(50.0f);

            // ルートモーションを使用するが初期化時点では使用しない
            owner_->SetUseRootMotion(false);
             
            // カウンター有効範囲を設定する
            PlayerManager::Instance().GetPlayer()->SetCounterActiveRadius(6.0f);

            // 変数初期化
            addForceData_.Initialize(1.5f, 0.3f, 0.5f);
 
            // ステート変更
            SetState(STATE::Attack);

            break;
        case STATE::Attack:// 攻撃

            // ルートモーションを使用する
            if (owner_->GetUseRootMotionMovement() == false)
            {
                if (owner_->GetIsBlendAnimation() == false) owner_->SetUseRootMotion(true);
            }

            // 移動処理
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }


            if (owner_->GetAnimationSeconds() > 2.2f)
            {
                if (owner_->GetIsAttackActive())
                    owner_->SetTurnAttackActiveFlag(false);
            }
            else if (owner_->GetAnimationSeconds() > 1.2f)
            {
                if (owner_->GetIsAttackActive() == false) 
                    owner_->SetTurnAttackActiveFlag();
            }

            if(owner_->GetAnimationSeconds() >= 3.25f)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
            break;
        }

        return ActionBase::State::Run;
    }

    void TurnAttackAction::DrawDebug()
    {
    }

    // ----- アニメーションを設定する -----
    void TurnAttackAction::SetAnimation()
    {
        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());

        if(animationIndex == Enemy::DragonAnimation::AttackTurn)
        {
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackTurn, false, 1.0f, 0.3f);
            return;
        }
        
        if (animationIndex == Enemy::DragonAnimation::AttackKnockBackEnd1)
        {
            owner_->SetTransitionTime(0.2f);
        }
        else
        {
            owner_->SetTransitionTime(0.1f);
        }
        
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackTurn, false);
    }
}

// ----- TackleAction -----
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

            // ダメージ設定
            owner_->SetAttackDamage(50.0f);

            // ルートモーションを使用するが初期化時点では使用しない
            owner_->SetUseRootMotion(false);

            // カウンター有効範囲を設定する
            PlayerManager::Instance().GetPlayer()->SetCounterActiveRadius(6.0f);

            // 変数初期化
            addForceData_.Initialize(0.15f, 0.6f, 0.6f);
            //addForceData_.Initialize(0.15f, 0.8f, 0.6f);
            easingTimer_ = 0.0f;

            // ステート変更
            SetState(STATE::PreAction);

            break;
        case STATE::PreAction:// 予備動作
        {
            // ルートモーションを使用する
            if (owner_->GetUseRootMotionMovement() == false)
            {
                if (owner_->GetIsBlendAnimation() == false) owner_->SetUseRootMotion(true);
            }

            // 回転処理 ( プレイヤーの方向に向く )
            const float turnStateFrame = 0.3f;
            if (owner_->GetAnimationSeconds() > turnStateFrame)
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
                const DirectX::XMFLOAT3 vec = XMFloat3Normalize(owner_->CalcDirectionToPlayer());

                owner_->AddForce(vec, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }
            // 回転処理
            if (addForceData_.GetIsAddForce() == false)
            {
                owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());
            }

            // 攻撃判定有効化
            if (owner_->GetIsAttackActive() == false)
            {
                const float attackActiveStartFrame = 0.2f;
                if (owner_->GetAnimationSeconds() > attackActiveStartFrame)
                {
                    owner_->SetTackleAttackActiveFlag();
                }
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
                // 地面に埋まらないように０地点にリセットする
                owner_->GetTransform()->SetPositionY(0);

                addForceData_.Initialize(0.03f, 0.5f, 0.6f);
                //addForceData_.Initialize(0.03f, 0.6f, 0.8f);

                owner_->PlayAnimation(Enemy::DragonAnimation::AttackTackle3, false);

                // ルートモーションリセット
                owner_->SetUseRootMotion(true);

                SetState(STATE::Recovery);
            }
        }
            break;
        case STATE::Recovery:// 後隙 ( 途中まで攻撃判定ある )
        {
            // 脚が部位破壊されている場合は途中で終了し、怯みに移行する
            if (owner_->GetIsPartDestruction(Enemy::PartName::Leg))
            {
                if (owner_->GetAnimationSeconds() >= 0.35f)
                {
                    // 攻撃判定無効化
                    owner_->SetTackleAttackActiveFlag(false);

                    // 位置Yを０にしてあげる
                    owner_->GetTransform()->SetPositionY(0.0f);

                    owner_->SetStep(0);
                    return ActionBase::State::Complete;
                }
            }

            // 攻撃判定無効化
            if (owner_->GetIsAttackActive())
            {
                if (owner_->GetAnimationSeconds() > 0.6f)
                {
                    owner_->SetTackleAttackActiveFlag(false);
                }
            }

            // 移動処理
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            // アニメーション終了
            if(owner_->GetAnimationSeconds() > 1.9f)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
        }
            break;
        }

        return ActionBase::State::Run;
    }
    void TackleAction::DrawDebug()
    {
    }
}

namespace ActionDragon
{
    const ActionBase::State ComboTackleAction::Run(const float& elapsedTime)
    {
        return ActionBase::State();
    }
    void ComboTackleAction::DrawDebug()
    {
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

            // ダメージ設定
            owner_->SetAttackDamage(50.0f);

            // ルートモーションを使用しない
            owner_->SetUseRootMotion(false);

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
                //Camera::Instance().SetRiseAttackState(0);
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
                //Camera::Instance().SetRiseAttackState(-1);
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
    void RiseAttackAction::DrawDebug()
    {
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
    void MoveTurnAction::DrawDebug()
    {
    }
}

namespace ActionDragon
{
    const ActionBase::State MoveAction::Run(const float& elapsedTime)
    {        
        switch (owner_->GetStep())
        {
        case 0:// 初期化
            // アニメーション設定
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Walk, true);
            //owner_->PlayBlendAnimation(Enemy::DragonAnimation::Run, true);
            owner_->SetTransitionTime(0.1f);

            // 移動目的地を設定
            targetPosition_ = PlayerManager::Instance().GetTransform()->GetPosition();

            owner_->SetStep(1);

            break;
        case 1:
        {
            // 回転処理
            owner_->Turn(elapsedTime, targetPosition_);

            // 移動処理
            const DirectX::XMFLOAT3 vec = XMFloat3Normalize(owner_->CalcDirectionToPlayer());
            const float speed = owner_->GetWalkSpeed() * elapsedTime;

            owner_->GetTransform()->AddPosition(vec * speed);

            const float distance = owner_->CalcDistanceToPlayer();
            if (distance < 10.0f)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }


        }
            break;
        }

        return ActionBase::State::Run;
    }
    void MoveAction::DrawDebug()
    {
    }
}