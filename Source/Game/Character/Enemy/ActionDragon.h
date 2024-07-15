#pragma once
#include "BehaviorTree/ActionBase.h"
#include "Enemy.h"

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

    private:
        float   addForceFrame_      = 0.0f;
        float   force_              = 0.0f;
        float   decelerationForce_  = 0.0f;
        bool    isAddforce_         = false;
    };
}

namespace ActionDragon
{
    // 死亡行動
    class DeathAction : public ActionBase
    {
    public:
        DeathAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        float timer_ = 0.0f;
    };

    // ひるみ行動
    class FlinchAction : public ActionBase
    {
    public:
        FlinchAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
    };

    // 非戦闘待機行動
    class NonBattleIdleAction : public ActionBase
    {
    public:
        NonBattleIdleAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        void SetAnimation();

    private:
        float timer_ = 0.0f;
    };

    // 非戦闘歩き行動
    class NonBattleWalkAction : public ActionBase
    {
    public:
        NonBattleWalkAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
    };

    // 咆哮行動
    class RoarAction : public ActionBase
    {
    public:
        RoarAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        void UpdateBlur(const float& elapsedTime);

    private:
        static const int vibrationNum_ = 3;
        GamePadVibration gamePadVibration_[vibrationNum_];

        // ----- Blur -----
        float   blurStartFrame_ = 0.0f; // ブラー開始フレーム
        float   blurEndFrame_   = 0.0f; // ブラー終了フレーム
        float   maxBlurPower_   = 0.0f; // ブラーの強さ
        float   blurTimer_      = 0.0f; // ブラーのEasing用タイマー
        float   maxBlurTime_    = 0.0f; // ブラー用

        bool isPlayerFilnch_ = false;
    };

    // バックステップ咆哮行動
    class BackStepRoarAction : public ActionBase
    {
    public:
        BackStepRoarAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        void UpdateBlur(const float& elapsedTime);

    private:
        GamePadVibration gamePadVibration_;

        // ----- Blur -----
        float   blurStartFrame_ = 0.0f; // ブラー開始フレーム
        float   blurEndFrame_   = 0.0f; // ブラー終了フレーム
        float   maxBlurPower_   = 0.0f; // ブラーの強さ
        float   blurTimer_      = 0.0f; // ブラーのEasing用タイマー
        float   maxBlurTime_    = 0.0f; // ブラー用
    };

    // バックステップ行動
    class BackStepAction : public ActionBase
    {
    public:
        BackStepAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
    };

    // 飛び攻撃行動
    class FlyAttackAction : public ActionBase
    {
    public:
        FlyAttackAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        enum MoveDirection
        {
            UpBack,
            DownForward,
            Max,
        };

        enum class STATE
        {
            Initialize, // 初期化
            FlyStart,   // 飛び始め
            PreAction,  // 予備動作
            FlyAttack,  // 攻撃
        };
        
        void SetStep(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        AddForceData addForceData_[MoveDirection::Max];

        float slowAnimationSpeed_       = 0.0f;
        float slowAnimationEndFrame_    = 0.0f;

        // ----- 予備動作用 -----
        float savePositionY_ = 0;
        float addPositionY_ = 0;
        float easingTimer_ = 0.0f;
        bool isDown_ = false;
        bool isRise_ = false;

        bool isAttackActive_ = false;
    };

    // ノックバック行動
    class KnockBackAction : public ActionBase
    {
    public:
        KnockBackAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        enum class STATE
        {
            Initialize, // 初期化
            Guard,      // ガード
            Loop,       // ガードループ
            LoopInit,   // ループ初期化
            Attack,     // 攻撃成功
            Failed,     // 攻撃失敗
        };

        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        int loopMax_ = 0;
        int loopCounter_ = 0;
    };

    // たたきつけ行動
    class SlamAction : public ActionBase
    {
    public:
        SlamAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
    };

    // 前方向攻撃行動
    class FrontAttackAction : public ActionBase
    {
    public:
        FrontAttackAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
    };

    // コンボたたきつけ行動
    class ComboSlamAction : public ActionBase
    {
    public:
        ComboSlamAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
    };

    // コンボチャージ行動
    class ComboChargeAction : public ActionBase
    {
    public:
        ComboChargeAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
    };

    // 回転攻撃行動   
    class TurnAttackAction : public ActionBase
    {
    public:
        TurnAttackAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        enum class STATE
        {
            Initialize, // 初期化
            PreAction,  // 予備動作
            Attack,     // 攻撃
        };

        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        AddForceData addForceData_;
        bool isAttackActive_ = false;
    };

    // タックル行動
    class TackleAction : public ActionBase
    {
    public:
        TackleAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        enum class STATE
        {
            Initialize, // 初期化
            PreAction,  // 予備動作
            Tackle,     // タックル
            Recovery,   // 後隙
        };

        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        AddForceData addForceData_;
        float easingTimer_ = 0.0f;

        bool isAttackActive_ = false;
    };

    // 上昇攻撃行動
    class RiseAttackAction : public ActionBase
    {
    public:
        RiseAttackAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        float riseTimer_ = 0.0f;
        bool isCameraSet_ = false; // カメラリセット用
        bool isCameraReset_ = false; // カメラリセット用

        AddForceData addForceData_;
    };

    // 移動回転行動
    class MoveTurnAction : public ActionBase
    {
    public:
        MoveTurnAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
    };

    // 移動攻撃行動
    class MoveAttackAction : public ActionBase
    {
    public:
        MoveAttackAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
    };
}