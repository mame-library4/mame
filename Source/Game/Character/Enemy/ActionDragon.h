#pragma once
#include "BehaviorTree/ActionBase.h"
#include "Enemy.h"

#include "Particle/SuperNovaParticle.h"
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
        int         intenseBlurFrame_       = 3;
        int         intenseBlurFrameCount_  = 0;
        float       preRadialBlurTimer_     = 0.0f;
        float       radialBlurTimer_        = 0.0f;
        const int   maxSampleCount_         = 5.0f;
    };

    // 死亡行動
    class DeathAction : public ActionBase
    {
    public:
        DeathAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;
    };

    // ひるみ行動
    class FlinchAction : public ActionBase
    {
    public:
        FlinchAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        enum class STATE
        {
            Initialize,
            FlinchStart,
            FlinchLoop,
            FlinchEnd,
        };

        void SetAnimation();
        void SetAnimationSpeed();
        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        AddForceData addForceData_;

        int loopCounter_ = 0; // ループした回数を数える
        int maxLoopNum_  = 0; // 最大ループ数
    };

    // 飛行中怯み行動
    class FlyFlinchAction : public ActionBase
    {
    public:
        FlyFlinchAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        enum class STATE
        {
            Initialize,
            FlinchStart,
            FlinchLoop,
            FlinchEnd,
        };

        void SetAnimationSpeed();
        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        float oldPositionY_ = 0.0f;
        float easingTimer_  = 0.0f; // Easing用

        int loopCounter_    = 0; // ループした回数を数える
        int maxLoopNum_     = 0; // 最大ループ数
    };

    // 部位破壊時ダッシュした時の怯み
    class PartDestructionFlinchAction : public ActionBase
    {
    public:
        PartDestructionFlinchAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;
    };

    // 非戦闘待機行動
    class NonBattleIdleAction : public ActionBase
    {
    public:
        NonBattleIdleAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

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
        void DrawDebug()                                      override;
    };

    // 咆哮行動
    class RoarAction : public ActionBase
    {
    public:
        RoarAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        void UpdateBlur(const float& elapsedTime);

    private:
        GamePadVibration gamePadVibration_;
        float blurTimer_ = 0.0f;
    };

    // 咆哮行動(長いやつ)
    class RoarLongAction : public ActionBase
    {
    public:
        RoarLongAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        void UpdateBlur(const float& elapsedTime);

    private:
        static const int vibrationNum_ = 3;
        GamePadVibration gamePadVibration_[vibrationNum_];

        // ----- ラジアルブラー -----
        int         intenseBlurFrame_       = 3;
        int         intenseBlurFrameCount_  = 0;
        float       maxStrength_            = 1.0f;
        float       radialBlurTimer_        = 0.0f;
        float       radialBlurSpeed_        = 1.0f;
        const int   maxSampleCount_         = 5.0f;

        bool isPlayerFilnch_ = false;
    };

    // バックステップ行動
    class BackStepAction : public ActionBase
    {
    public:
        BackStepAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;
    };

    // 飛び攻撃行動
    class FlyAttackAction : public ActionBase
    {
    public:
        FlyAttackAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

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
        
        void SetAnimation();
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
    };

    // ノックバック行動
    class KnockBackAction : public ActionBase
    {
    public:
        KnockBackAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

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

        float oldHealth_ = 0.0f;
    };

    // たたきつけ行動
    class SlamAction : public ActionBase
    {
    public:
        SlamAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;
    };

    // ブレス
    class FireBreath : public ActionBase
    {
    public:
        FireBreath(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        DirectX::XMFLOAT3 targetPosition_ = {};
        bool isCreateFireball_ = false;
    };

    // ブレス ( ３連撃 )
    class FireBreathCombo : public ActionBase
    {
    public:
        FireBreathCombo(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        enum class STATE
        {
            Initialize,     // 初期化
            FirstAttack,    // 一発目
            SecondAttack,   // 二発目
            ThirdAttack,    // 三発目
        };

        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }
        void Launch(const float& launchFrame);

    private:
        DirectX::XMFLOAT3 targetPosition_ = {};
        bool isCreateFireball_ = false;
    };

    // コンボたたきつけ行動
    class ComboSlamAction : public ActionBase
    {
    public:
        ComboSlamAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        enum class STATE
        {
            Initialize, // 初期化
            Attack0,    // 攻撃一発目
            Attack1,    // 攻撃二発目
            Recovery,   // 後隙
        };

        void SetAnimation();
        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        AddForceData addForceData_;
    };

    // コンボたたきつけ攻撃(軸合わせしてくる)
    class ComboFlySlamAction : public ActionBase
    {
    public:
        ComboFlySlamAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        enum class STATE
        {
            Initialize, // 初期化
            Attack,     // たたきつけ攻撃
            ComboJudge, // コンボ攻撃するか判定
            Recovery,   // 後隙
        };

        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        AddForceData    addForceData_;
        int             comboNum_       = 0;
        const int       maxComboNum_    = 2; // 最大コンボ数
    };

    // コンボチャージ行動
    class ComboChargeAction : public ActionBase
    {
    public:
        ComboChargeAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;
    };

    // 回転攻撃行動   
    class TurnAttackAction : public ActionBase
    {
    public:
        TurnAttackAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        enum class STATE
        {
            Initialize, // 初期化
            Attack,     // 攻撃
        };

        void SetAnimation();
        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        AddForceData addForceData_;
    };

    // タックル行動
    class TackleAction : public ActionBase
    {
    public:
        TackleAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

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
    };

    class ComboTackleAction : public ActionBase
    {
    public:
        ComboTackleAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;
    };

    // 上昇攻撃行動
    class RiseAttackAction : public ActionBase
    {
    public:
        RiseAttackAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

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
        void DrawDebug()                                      override;
    };

    class MoveAction : public ActionBase
    {
    public:
        MoveAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
        void DrawDebug()                                      override;

    private:
        DirectX::XMFLOAT3 targetPosition_ = {};
    };
}