#pragma once
#include "BehaviorTree/ActionBase.h"
#include "Enemy.h"

#include "Particle/TailParticle.h"
#include "Particle/SuperNovaParticle.h"
#include "Particle/SlamAttackParticle.h"
#include "Particle/MeteorParticle.h"

#include "Effect/EffectManager.h"

namespace ActionDragon
{
    struct GamePadVibration
    {
    public:
        void Initialize(const float& startFrame, const float& time, const float& power);
        void Update(const float& animationFrame);

    private:
        float startFrame_   = 0.0f;
        float time_         = 0.0f;
        float power_        = 0.0f;
        bool  isVibraion_   = false;
    };

    struct AddForceData
    {
    public:
        void Initialize(const float& addForceFrame, const float& force, const float& decelerationForce = 2.0f);
        bool Update(const float& animationFrame);

        [[nodiscard]] const float GetForce() const { return force_; }
        [[nodiscard]] const float GetDecelerationForce() const { return decelerationForce_; }
        [[nodiscard]] const bool  GetIsAddForce() const { return isAddforce_; }

    private:
        float   addForceFrame_      = 0.0f;
        float   force_              = 0.0f;
        float   decelerationForce_  = 0.0f;
        bool    isAddforce_         = false;
    };
}

namespace ActionDragon
{
    // ----- DeathAction -----
    class DeathAction : public ActionBase
    {
    public:
        DeathAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;
    };

    // ----- DownAction -----
    class DownAction : public ActionBase
    {
    public:
        DownAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        void PlayAnimation();

    private:
        int loopCounter_ = 0;
        int maxLoopNum_ = 3;

    };

    // ----- KnockDownAction -----
    class KnockDownAction : public ActionBase
    {
    public:
        KnockDownAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        [[nodiscard]] const bool PlayAnimation();
        void UpdateAnimationSpeed();

    private:
        // ----- TransitionTime -----
        float transitionSlamAttack_ = 0.2f;

        // ----- AnimationSpeed -----
        float slowStartFrame_           = 1.4f;
        float slowEndFrame_             = 1.6f;
        float slowAnimationSpeed_       = 0.7f;
        float mostSlowAnimationSpeed_   = 0.6f;
    };

#pragma region ---------- 咆哮 ----------
    class RoarAction : public ActionBase
    {
    public:
        RoarAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;
    };

#pragma endregion ---------- 咆哮 ----------

#pragma region ---------- 攻撃 ----------
    // ----- SlamAttackAction -----
    class SlamAttackAction : public ActionBase
    {
    public:
        SlamAttackAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        void PlayAnimation();

        void Finalize();
        void UpdateAnimationSpeed(); // アニメーションの速度を調整する

        void UpdateJustDodgeStatus(); // ジャスト回避判定更新

        void PlayChargeEffect();

        void SetTargetPosition();            // 目標地点設定
        void Move(const float& elapsedTime); // 移動処理
        void Turn(const float& elapsedTime); // 旋回処理

    private:
        SlamAttackParticle* slamAttackParticle_ = nullptr;

        Effekseer::Handle powerEffectHandle_ = {};
        bool isCreateChargeEffect_ = false;
        bool isPlayExplosionParticle_ = false;

        float slowAnimationSpeed_   = 0.12f;
        float slowStartFrame_       = 0.88f;  // スロー開始フレーム
        float slowEndFrame_         = 0.93f; // スロー終了フレーム

        // ----- ジャスト回避 -----
        float justDodgeStartFrame_  = 0.93f;
        float justDodgeEndFrame_    = 1.05f;
        bool  isActiveJustDodge_    = false;

        // ----- TransitionFrame -----
        float transitionWalk_ = 0.2f;

        // ----- BlendFrame -----
        float blendFrameWalk_ = 0.3f;;

        // ---------- Movement & Rotation ----------
        DirectX::XMFLOAT3   startPosition_      = {};   // スタート地点
        DirectX::XMFLOAT3   targetPosition_     = {};   // 目標位置
        float               targetSetFrame_     = 0.5f; // 目標をセットするフレーム
        bool                isSetTarget_        = false;// 目標をセットしたか
        bool                isMovement_         = false;// 移動処理をするか
        float               moveLength_         = 0.0f; // 移動量
        float               minMoveLength_      = 5.0f; // 最小移動量
        float               maxMoveLength_      = 7.0f; // 最大移動量
        float               moveTimer_          = 0.0f; // 移動用
        float               moveSpeed_          = 2.0f; // 移動速度
        float               rotationEndFrame_   = 0.9f; // 回転終了フレーム

    };

    // ----- TurnAttackAction -----
    class TurnAttackAction : public ActionBase
    {
    public:
        TurnAttackAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:// ----- 定数 -----
        enum class STATE
        {
            Initialize, // 初期化
            Attack,     // 攻撃
        };

    private:
        void PlayAnimation();

        void Finalize(); // 終了化
        void UpdateAnimationSpeed(); // アニメーションの速度を調整する

        

        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }
    private:
        TailParticle* tailParticle_ = nullptr;

        float slowStartFrame_   = 0.5f;   // スロー開始フレーム
        float slowEndFrame_     = 0.8f; // スロー終了フレーム
        float slowSpeed_        = 0.3f;

        float recoveryFrame_ = 2.0f; // 攻撃の後隙
        float recoverySpeed_ = 1.0f;

        const float removeFrame_ = 2.2f;

        bool isPlayTailParticle_        = false;
        bool isPlayTailTrailParticle_   = false;
        bool isRemoveParticle_          = false;

        // ----- TransitionFrame -----
        float transitionWalk_ = 0.2f;

        // ----- BlendFrame -----
        float blendFrameWalk_ = 0.3f;

        // ----- Movement -----
        AddForceData addForceData_;
        bool         isAbleMove_ = true;
    };

    // ----- SuperNovaAction -----
    class SuperNovaAction : public ActionBase
    {
    public:
        SuperNovaAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        void GenerateChargeEffect(const float& elapsedTime);
        void GenarateMainEffect(const float& elapsedTime);

        void UpdateChargeEffect(const float& elapsedTime);
        void UpdateRadialBlur(const float& elapsedTime);

        void Finalize();

    private:
        SuperNovaParticle* superNovaParticle_ = nullptr;

        bool isCreateLavaCrawlerParticle_ = false;

        bool isCreateCoreBurst_ = false; // メインの爆発

        float scaleLerpTimer_ = 0.0f;

        Effekseer::Handle powerEffectHandle_ = {};

        // ----- ラジアルブラー -----
        int         intenseBlurFrame_ = 3;
        int         intenseBlurFrameCount_ = 0;
        float       preRadialBlurTimer_ = 0.0f;
        float       radialBlurTimer_ = 0.0f;
        const int   maxSampleCount_ = 5.0f;
    };

    // ----- WalkAction -----
    class WalkAction : public ActionBase
    {
    public:
        WalkAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        void PlayAnimation();

    private:
        DirectX::XMFLOAT3   initPosition_   = {};
        DirectX::XMFLOAT3   targetPosition_ = {};
        DirectX::XMFLOAT3   direction_      = {};
        float               minMoveLength_  = 5.0f;
        float               maxMoveLength_  = 20.0f;
        float               moveLength_     = 0.0f;
        float               targetLength_   = 0.0f;
        float               offsetLength_   = 8.0f;

        float               lerpTimer_      = 0.0f;
        float               maxLerpSpeed_   = 2.0f;
        float               minLerpSpeed_   = 1.0f;
        float               lerpSpeed_      = 1.0f;

        // ----- TransitionTime -----
        float transitionSlamAttack_ = 0.2f;
        float transitionTurnAttack_ = 0.2f;
    };

#pragma endregion ---------- 攻撃 ----------

    // ----- GuardAction -----
    class GuardAction : public ActionBase
    {
    public:
        GuardAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        float oldHealth_ = 0.0f;
        
        int loopCounter_ = 0;
        int loopMax_ = 2;
    };

#pragma region ---------- 未完成 ----------
    // ----- Meteor -----
    class MeteorAction : public ActionBase
    {
    public:
        MeteorAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        void PlayAnimation();
        void UpdateAnimationSpeed();
        void Finalize();

    private:
        MeteorParticle* meteorParticle_ = nullptr;

        float slowAnimationSpeed_ = 0.5f;
    };

    // ----- FireBreath -----
    class FireBreathAction : public ActionBase
    {
    public:
        FireBreathAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;
    };
#pragma endregion ---------- 未完成 ----------
}