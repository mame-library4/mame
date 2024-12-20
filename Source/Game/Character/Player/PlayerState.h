#pragma once
#include "StateMachine/State.h"
#include "Player.h"

namespace PlayerState
{
    struct AddForceData
    {
    public:
        void Initialize(const float& addForceFrame, const float& force, const float& decelerationForce = 2.0f);
        bool Update(const float& animationFrame);

        [[nodiscard]] const float GetForce() const { return force_; }
        [[nodiscard]] const float GetDecelerationForce() const { return decelerationForce_; }
        [[nodiscard]] const bool GetIsAddForce() const { return isAddforce_; }

    private:
        float   addForceFrame_      = 0.0f;
        float   force_              = 0.0f;
        float   decelerationForce_  = 0.0f;
        bool    isAddforce_         = false;
    };

    struct AttackData
    {
    public:
        void Initialize(const float& startFrame, const float& endFrame);
        bool Update(const float& animationFrame, const bool& flag);

    private:
        float attackStartFrame_ = 0.0f;     // 攻撃判定有効スタートフレーム
        float attackEndFrame_   = 0.0f;     // 攻撃判定有効エンドフレーム
    };

    struct GamePadVibration
    {
    public:
        void Initialize(const float& startFrame, const float& time, const float& power);
        const bool Update(const float& animationFrame);

    private:
        float startFrame_ = 0.0f;
        float time_ = 0.0f;
        float power_ = 0.0f;
        bool  isVibraion_ = false;
    };
}

// ---------- SwordMan ----------
namespace PlayerState
{
    // ----- 待機 -----
    class IdleState : public State<Player>
    {
    public:
        IdleState(Player* player) : State(player, "IdleState") {}
        ~IdleState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void SetAnimation();
        [[nodiscard]] const bool CheckNextInput();
    };

    // ----- 走り -----
    class RunState : public State<Player>
    {
    public:
        RunState(Player* player) : State(player, "RunState") {}
        ~RunState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void SetAnimation();
        [[nodiscard]] const bool CheckNextInput();

        void UpdateDash(const float& elapsedTime);

    private:
        int dashSENum_ = 0;
        float changeStateTimer_ = 0.0f;
    };
    
    // ----- ガードカウンター -----
    class GuardCounterState : public State<Player>
    {
    public:
        GuardCounterState(Player* player) : State(player, "GuardCounterState") {}
        ~GuardCounterState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void UpdateEffect(const float& elapsedTime);    // エフェクト更新
        [[nodiscard]] const bool CheckGuardCounterSuccessful();

    private:
        Effekseer::Handle guardEffect_ = {};
        const float guardStartAnimationSpeed_   = 3.0f; // ガード構えアニメーション再生速度
        float       guardEffectStartSize_       = 2.8f; // エフェクトの最大サイズ
        float       guardEffectEndSize_         = 1.8f; // エフェクトの最小サイズ
        float       guardEffectLerpTimer_       = 0.0f; // EffectLerp用
        float       guardEffectLerpSpeed_       = 0.4f; // EffectLerp速度調整用
        float       guardCounterStartRadius_    = 1.4f;
        float       guardCounterEndRadius_      = 0.9f;

        // ----- ジャストガード用 -----
        float   justGuardTimer_         = 0.0f;  // ジャストガード管理タイマー
        float   justGuardFrame_         = 0.0f;  // ジャストガード受付フレーム
        bool    isJustGuardSuccessful_  = false; // ジャストガードが成功したか

        GamePadVibration gamePadVibration_;
    };

    // ----- ガードカウンター攻撃 -----
    class GuardCounterAttackState : public State<Player>
    {
    public:
        GuardCounterAttackState(Player* player) : State(player, "GuardCounterAttackState") {}
        ~GuardCounterAttackState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;
    };

    // ----- ガードブロック -----
    class GuardBlockState : public State<Player>
    {
    public:
        GuardBlockState(Player* player) : State(player, "GuardBlockState") {}
        ~GuardBlockState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        float moveInputFrame_       = 0.4f;
        float moveInputThreshold_   = 0.7f;
    };

    // ----- ガード破壊された -----
    class GuardBrokenState : public State<Player>
    {
    public:
        GuardBrokenState(Player* player) : State(player, "GuardBrokenState") {}
        ~GuardBrokenState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;
    };

    class LightFlinchState : public State<Player>
    {
    public:
        LightFlinchState(Player* player) : State(player, "LightFlinchState") {}
        ~LightFlinchState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;
    };

    class FlinchState : public State<Player>
    {
    public:
        FlinchState(Player* player) : State(player, "FlinchState") {}
        ~FlinchState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        int state_ = 0;
    };

    // ----- ダメージ -----
    class DamageState : public State<Player>
    {
    public:
        DamageState(Player* player) : State(player, "DamageState") {}
        ~DamageState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void SetVignette(); // ビネット設定

        void SetAnimationSpeed();
        void Turn();

    private:
        AddForceData addForceData_;

        bool isFirstAnimation_ = false; // 最初のアニメーションか

        DirectX::XMFLOAT3 addForceDirection_ = {}; // 吹っ飛び方向

        bool isCameraShakeActive_ = false;

        // ----- Vignette -----
        DirectX::XMFLOAT4 normalDamageColor_        = { 1.0f, 0.0f, 0.0f, 1.0f };
        DirectX::XMFLOAT4 highDamageColor_          = { 1.0f, 0.4f, 0.0f, 1.0f };
        float             vignetteTimer_            = 0.0f;
        float             vignetteFadeOutSpeed_     = 0.7f;
        float             normalDamageMaxIntensity_ = 0.7f;
        float             highDamageMaxIntensity_   = 1.5f;
        bool              isHighDamage_             = false;
    };

    // ----- 死亡 -----
    class DeathState : public State<Player>
    {
    public:
        DeathState(Player* player) : State(player, "DeathState") {}
        ~DeathState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        float deathTimer_ = 0.0f;
        bool isCreateFadeUi_ = false;
    };

    // ----- 回避 -----
    class DodgeState : public State<Player>
    {
    public:
        DodgeState(Player* player) : State(player, "DodgeState") {}
        ~DodgeState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void Turn(const float& elapsedTime);

        void ResetState(); // このステートをリセット(初期化)する

        [[nodiscard]] const bool CheckNextInput();
        void SetAnimationSpeed();

        void SetAnimation();        // アニメーション設定

    private:
        float currentAnimationFrame_    = 0.0f;     // 現在のフレーム
        float invincibleFrame_          = 0.45f;    // 無敵時間フレーム
        float justDodgeFrame_           = 0.3f;     // ジャスト回避フレーム
        float rootMotionMoveValue_      = 1.2f;     // ルートモーション移動値

        DirectX::XMFLOAT2 inputDirection_ = {};
        bool isFirstTime_   = true;     // このステートに入るのが初めてかどうか
        bool isRotating_    = false;    // 回転処理をするか
        bool isInputStick_  = false;    // スティック入力があるか

        // ----- Animation -----
        float animationStartFrame_ = 0.15f;
    };
      
    // ----- ジャスト回避 -----
    class JustDodgeState : public State<Player>
    {
    public:
        JustDodgeState(Player* player) : State(player, "JustDodgeState") {}
        ~JustDodgeState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        // ----- コントローラー振動 -----
        float vibrationTime_ = 0.5f;

        // ----- スロー用 -----
        float allSlowSpeed_     = 0.01f;
        float playerSlowSpeed_  = 0.4f;

        float slowStartFrame_ = 0.1f;
        float mostSlowStartFrame_ = 1.0f;
        float slowTimer_ = 0.0f;

        // ----- ポストエフェクト用 -----
        float lerpTimer_ = 0.0f;
        float lerpSpeed_ = 3.0f;
        float maxLerpStrength_ = 0.1f;

        // ----- ルートモーション用 -----
        float rootMotionMoveValue_ = 1.2f;

        // ----- SE -----
        int slowSENum_ = 0;
    };

    // ----- ジャスト回避キャンセル -----
    class JustDodgeCancelState : public State<Player>
    {
    public:
        JustDodgeCancelState(Player* player) : State(player, "JustDodgeCancelState") {}
        ~JustDodgeCancelState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;
    };

    // ----- ラッシュ攻撃 -----
    class RushAttackState : public State<Player>
    {
    public:
        RushAttackState(Player* player) : State(player, "RushAttackState") {}
        ~RushAttackState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void Turn(const float& elapsedTime); // 旋回処理

        void SetTargetPosition(); // ターゲットを設定

    private:
        AttackData      attackData_;

        // ----- 旋回 -----
        float rotationSpeed_ = 20.0f;

        float radius_ = 1.0f;

        DirectX::XMFLOAT3   targetPosition_     = {};
        std::string         targetJointName_    = "";

        bool isNextInput_ = false;

        int currentAttackNum_ = 0;

        // ----- ラジアルブラー -----
        float startRadialBlurStrength_ = 0.0f;
        float radialBlurLerpTimer_ = 0.0f;
        float radialBlurLerpSpeed_ = 3.0f;

        // ----- RootMotion -----
        float dashRootMotionValue_ = 4.0f;
    };

    // ----- カウンター -----
    class CounterState : public State<Player>
    {
    public:
        CounterState(Player* player) : State(player, "Counter") {}
        ~CounterState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void SetAnimation();

        void Move();                            // 移動
        void Turn(const float& elapsedTime);    // 旋回
        
        void SetAnimationSpeed();

    private:
        AttackData          attackData_;
        GamePadVibration    gamePadVibration_;

        bool isNextInput_ = false; // カウンター攻撃の先行入力用

        // ----- Transition -----
        float transitionIdle_   = 0.1f;
        float transitionRun_    = 0.1f;

        // ----- Movement -----
        AddForceData        addForceFront_; // 前方向
        AddForceData        addForceBack_;  // 後ろ方向
        DirectX::XMFLOAT2   addForceDirection_ = {};

        // ----- Rotation -----
        bool isRotating_ = false;

        // ----- Counter -----
        float counterStartFrame_    = 0.1f;
        float counterEndFrame_      = 0.6f;
        bool isCounterReaction      = false; // カウンターが成功したときの演出

        // ----- Effect用 -----
        Effekseer::Handle   mikiriEffectHandle_     = 0;
        Effekseer::Handle   counterEffectHandle_    = 0;
        DirectX::XMFLOAT3   effectOffsetVec_        = {};
        float               effectLength_           = 0.0f;
        DirectX::XMFLOAT3   mikiriEffectAddPosition_ = {};

        // ----- Vignette -----
        DirectX::XMFLOAT4 vignetteColor_        = { 0.4f, 0.4f, 1.0f, 1.0f };
        float             vignetteMaxIntensity_ = 1.0f;
        float             vignetteTimer_        = 0.0f;
        float             vignetteFadeOutSpeed_ = 1.0f;
        bool              isVignetteActive_     = false;
    };

    // ----- カウンター攻撃 -----
    class CounterComboState : public State<Player>
    {
    public:
        CounterComboState(Player* player) : State(player, "CounterComboState") {}
        ~CounterComboState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void UpdateAnimationSpeed();
        void Turn(const float& elapsedTime); // 旋回処理

    private:
        AttackData          attackData_;

        // ----- Roation -----
        float rotationEndFrame_ = 0.45f;

        // ----- CameraVibration -----
        float cameraVibrationFrame_     = 0.6f;
        float cameraVibrationVolume_    = 0.08f;
        float cameraVibrationTime_      = 0.6f;
        bool isPlayCameraVibration_     = false;

        // ----- Slow -----
        float slowEndFrame_       = 0.5f;
        float slowAnimationSpeed_ = 0.7f;

        // ----- SwordTrail -----
        float swordTrailStartFrame_ = 0.45f;
        float swordTrailEndFrame_   = 0.8f;

        // ----- Debug用 -----
        float currentAnimationFrame_ = 0.0f;
    };

    // ----- 走り攻撃 ----- 
    class RunAttackState : public State<Player>
    {
    public:
        RunAttackState(Player* player) : State(player, "RunAttackState") {}
        ~RunAttackState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        [[nodiscard]] const bool CheckNextInput();

    private:
        AddForceData    addForceData_;
        AttackData      attackData_;
    };

    // ----- コンボ0_0 -----
    class ComboAttack0_0 : public State<Player>
    {
    public:
        ComboAttack0_0(Player* player) : State(player, "ComboAttack0_0") {}
        ~ComboAttack0_0() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void SetAnimation();
        void SetAnimationSpeed();
        [[nodiscard]] const bool CheckNextInput();

    private:
        AttackData      attackData_;

        bool isAbleAttack_ = false;
    };

    // ----- コンボ0_1 -----
    class ComboAttack0_1 : public State<Player>
    {
    public:
        ComboAttack0_1(Player* player) : State(player, "ComboAttack0_1") {}
        ~ComboAttack0_1() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void SetAnimation();
        void SetAnimationSpeed();
        [[nodiscard]] const bool CheckNextInput();

    private:
        AttackData      attackData_;
    };

    // ----- コンボ0_2 -----
    class ComboAttack0_2 : public State<Player>
    {
    public:
        ComboAttack0_2(Player* player) : State(player, "ComboAttack0_2") {}
        ~ComboAttack0_2() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void SetAnimationSpeed();
        [[nodiscard]] const bool CheckNextInput();

    private:
        AttackData      attackData_;

        // ----- SE -----
        float   swordSlashSEPlayFrame_  = 0.7f;
        bool    isPlaySwordSlashSE_     = false;
    };

    // ----- コンボ0_3 -----
    class ComboAttack0_3 : public State<Player>
    {
    public:
        ComboAttack0_3(Player* player) : State(player, "ComboAttack0_3") {}
        ~ComboAttack0_3() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void SetAnimationSpeed();
        [[nodiscard]] const bool CheckNextInput();

        void UpdateRootMotionMovement();

        void Turn(const float& elapsedTime);

    private:
        AttackData      attackData_;
        bool isVibration_ = false;

        // ----- SE -----
        float   swordSlashSEPlayFrame_ = 0.65f;
        bool    isPlaySwordSlashSE_ = false;

        float rootMotionValue_ = 0.4f;
    };

    // ----- 樽設置 -----
    class PlacingBarrelState : public State<Player>
    {
    public:
        PlacingBarrelState(Player* player) : State(player, "PlacingBarrelState") {}
        ~PlacingBarrelState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        float firstAnimationStartFrame_         = 0.35f;
        float firstAnimationEndFrame_           = 0.5f;
        float firstAnimationSpeed_              = 1.0f;
        float firstAnimationTransitionTime_     = 0.1f;
        float secondAnimationStartFrame_        = 0.8f;
        float secondAnimationSpeed_             = 1.5f;
        float secondAniamtionTransitionTime_    = 0.2f;

        float generatePosition_ = 0.7f;
    };

    // ----- 兜割り -----
    class HelmbreakerState : public State<Player>
    {
    public:
        HelmbreakerState(Player* player) : State(player, "HelmbreakerState") {}
        ~HelmbreakerState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void UpdateAttackJudgment(); // 攻撃判定更新
        void UpdateAnimationSpeed(); // アニメーション速度調整

    private:
        float firstAnimationStartFrame_ = 0.2f;
        float firstAnimationEndFrame_   = 0.7f;
        float firstAnimationSpeed_      = 1.0f;

        float secondAnimationSpeed_     = 0.7f;

        float transitionTime_           = 0.15f;

        float rootMotionMoveValue_      = 2.0f;

        
        float startPositionY_ = 0.0f;
        float totalMoveFrame_ = 0.11f;

        float moveValueY_ = 5.0f;

        int hitCounter_ = 0;
        int maxHitNum_ = 5;
    };

    // ----- チャージ攻撃 -----
    class ChargeAttackState : public State<Player>
    {
    public:
        ChargeAttackState(Player* player) : State(player, "ChargeAttackState") {}
        ~ChargeAttackState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void PlayAnimation(); // アニメーション再生

    private:
        // ---------- チャージスタート ----------
        float chargeStartAnimationSpeed_        = 1.0f;
        float chargeStartAnimationStartFrame_   = 0.25f;

        float transitionIdle_ = 0.15f;

        

        float firstAttackAnimationSpeed_        = 1.0f;
        float firstAttackAnimationStartFrame_   = 0.18f;
        float firstAttackAnimationEndFrame_     = 0.3f;
        float transitionChargeLoop_             = 0.1f;

        float secondAttackAnimationSpeed_       = 1.0f;
        float secondAttackAnimationStartFrame_  = 0.45f;
        float secondAttackAnimationEndFrame_    = 0.75f;
        //float secondAttackAnimationStartFrame_  = 0.16f;
        //float secondAttackAnimationEndFrame_    = 0.6f;
        float transitionFirstAttack_            = 0.1f;

        float riseAnimationSpeed_       = 1.0f;
        float riseAnimationStartFrame_  = 0.4f;
        float riseAnimationEndFrame_    = 0.5f;
        float riseAnimationMoveValue_   = 1.0f;
        float transitionSecondAttack_   = 0.1f;

        float thirdAttackAnimationSpeed_        = 1.0f;
        float thirdAttackAnimationStartFrame_   = 0.0f;

        float transitionRise_                   = 0.1f;

        float startPositionY_ = 0.0f;        

        // ---------- チャージ ----------
        DirectX::XMFLOAT4 currentChargeColor_ = {};
        DirectX::XMFLOAT4 chargeColor_[3] =
        {
            { 1.0f, 1.0f, 1.0f, 1.0f }, // 白
            { 1.0f, 1.0f, 0.0f, 1.0f }, // 黄
            { 1.0f, 0.0f, 0.0f, 1.0f }, // 赤
        };
        int     currentCharge_  = 0;
        int     maxChargeNum_   = 3;
        float   chargeTimer_    = 0.0f;
        float   maxChargeTime_  = 1.0f;
        float   chargeSpeed_    = 1.2f;

        bool isOutlineActive_ = false;

        // ---------- Vibration ----------
        float               vibrationTime_      = 0.6f;
        DirectX::XMFLOAT2   vibrationVolume_[3] =
        {
            { 0.6f, 0.3f },
            { 0.8f, 0.2f },
            { 1.0f, 0.1f },
        };

        Effekseer::Handle chargeEffect_ = {};

        float               chargeEffectSize_   = 0.15f;
        float               chargeEffectSpeed_  = 1.2f;
    };
}

// ---------- Mage ----------
namespace PlayerState
{
    // ----- 待機 -----
    class MageIdleState : public State<Player>
    {
    public:
        MageIdleState(Player* player) : State(player, "MageIdleState") {}
        ~MageIdleState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        [[nodiscatd]] const bool CheckNextInput();

    };

    // ----- 走り -----
    class MageRunState : public State<Player>
    {
    public:
        MageRunState(Player* player) : State(player, "MageRunState") {}
        ~MageRunState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void PlayAnimation(); // アニメーション再生
        [[nodiscard]] const bool CheckNextInput();
        void UpdateDash(const float& elapsedTime);

    private:
        float transitionDodge_ = 0.2f;
    };

    // ----- 回避 -----
    class MageDodgeState : public State<Player>
    {
    public:
        MageDodgeState(Player* player) : State(player, "MageDodgeState") {}
        ~MageDodgeState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void PlayAnimation();                                // アニメーション再生
        void UpdateAnimationSpeed(const float& elapsedTime); // アニメーション再生速度更新
        void Turn(const float& elapsedTime);                 // 旋回処理
        void ResetState();                                   // このステートをリセット(初期化)する
        
        [[noidscard]] const bool CheckNextInput(); // 先行入力判定

    private:
        // ----- Animation -----
        float playAnimationSpeed_        = 1.4f; // 再生速度
        float changeAnimationSpeed_      = 1.0f; // 再生速度
        float animationSpeedChangeFrame_ = 0.6f;
        
        float animationStartFrame_  = 0.15f;    // 再生開始フレーム
        
        float transitionDodge_      = 0.05f;    // 回避モーションからの遷移

        float rootMotionMoveValue_  = 1.2f;     // ルートモーションの移動値

        float invincibleFrame_      = 0.45f;    // 無敵フレーム

        

        // ----- 先行入力 -----
        float nextInputStartFrame_      = 0.5f;
        float dodgeStateChangeFrame_    = 0.9f;
        float runStateChangeFrame_      = 0.8f;

        DirectX::XMFLOAT2   inputDirection_     = {};
        bool                isDodgeFirstTime_   = true;  // 連続で回避しているか
        bool                isRotating_         = false; // 旋回処理を行うか
        bool                isInputStick_       = false; // スティック入力があるか
    };

    // ----- ダメージ -----
    class MageDamageState : public State<Player>
    {
    public:
        MageDamageState(Player* player) : State(player, "MageDamageState") {}
        ~MageDamageState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void Turn();        // 旋回処理
        void SetVignette(); // ビネット設定
        void UpdateAnimationSpeed();

    private:
        // ----- Animation -----
        float   playAnimationSpeed_ = 1.2f;
        float   transitionDamage_   = 0.1f;
        bool    isFirstAnimation_   = false;

        float getUpStartFrame_  = 1.2f;
        float getUpEndFrame_    = 1.8f;

        float dodgeStateChangeFrame_ = 1.25f;

        // ----- AddForceData -----
        AddForceData        addForceData_       = {};
        DirectX::XMFLOAT3   addForceDirection_  = {};

        // ----- CameraShake -----
        float   cameraShakeStartFrame_  = 0.15f;
        float   cameraShakePower_       = 0.1f;
        float   cameraShakeTime_        = 0.3f;
        bool    isCameraShakeActive_    = false;

        // ----- Vignette -----
        DirectX::XMFLOAT4 normalDamageColor_        = { 1.0f, 0.0f, 0.0f, 1.0f };
        DirectX::XMFLOAT4 highDamageColor_          = { 1.0f, 0.4f, 0.0f, 1.0f };
        float             vignetteTimer_            = 0.0f;
        float             vignetteFadeOutSpeed_     = 0.7f;
        float             normalDamageMaxIntensity_ = 0.7f;
        float             highDamageMaxIntensity_   = 1.5f;
        bool              isHighDamage_             = false;
    };

    // ----- 攻撃0_0 -----
    class MageAttack0_0 : public State<Player>
    {
    public:
        MageAttack0_0(Player* player) : State(player, "MageAttack0_0") {}
        ~MageAttack0_0() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void PlayAnimation(); // アニメーション再生
        [[nodiscard]] const bool CheckNextInput(); // 先行入力判定

    private:
        float playAnimationSpeed_   = 1.0f;
        float animationStartFrame_  = 0.65f;
        
        float attack0_1ChangeFrame_ = 0.9f;
    };

    // ----- 攻撃0_1 -----
    class MageAttack0_1 : public State<Player>
    {
    public:
        MageAttack0_1(Player* player) : State(player, "MageAttack0_1") {}
        ~MageAttack0_1() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void PlayAnimation(); // アニメーション再生
        [[nodiscard]] const bool CheckNextInput(); // 先行入力判定

    private:
        float playAnimationSpeed_ = 1.0f;
        float animationStartFrame_ = 0.0f;
        float transitionAttack0_0_ = 0.1f;

        float attack0_0ChangeFrame_ = 0.3f;
    };

    // ----- 攻撃1_0 -----
    class MageAttack1_0 : public State<Player>
    {
    public:
        MageAttack1_0(Player* player) : State(player, "MageAttack1_0") {}
        ~MageAttack1_0() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void PlayAnimation(); // アニメーション再生

        [[nodiscard]] const bool CheckNextInput(); // 先行入力判定

    private:
        float playAniamtionSpeed_   = 1.0f;
        float animationStartFrame_  = 0.0f;

        float   hailBoltLaunchFrame_    = 0.38f;
        float   hailBoltMoveSpeed_      = 30.0f;
        bool    isCreateHailBolt_       = false;

        float attack1_1ChangeFrame_ = 0.5f;
    };

    // ----- 攻撃1_1 -----
    class MageAttack1_1 : public State<Player>
    {
    public:
        MageAttack1_1(Player* player) : State(player, "MageAttack1_1") {}
        ~MageAttack1_1() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void PlayAnimation(); // アニメーション再生

        [[nodiscard]] const bool CheckNextInput(); // 先行入力判定

    private:
        float playAnimationSpeed_   = 1.0f;
        float animationStartFrame_  = 0.0f;

        float attack1_2ChangeFrame_ = 0.6f;
    };

    // ----- 攻撃1_2 -----
    class MageAttack1_2 : public State<Player>
    {
    public:
        MageAttack1_2(Player* player) : State(player, "MageAttack1_2") {}
        ~MageAttack1_2() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void PlayAnimation(); // アニメーション再生

        [[nodiscard]] const bool CheckNextInput(); // 先行入力判定

    private:
        float playAnimationSpeed_ = 1.0f;
        float animationStartFrame_ = 0.0f;

        float attack1_3ChangeFrame_ = 1.0f;
    };

    // ----- 攻撃1_0 -----
    class MageAttack1_3 : public State<Player>
    {
    public:
        MageAttack1_3(Player* player) : State(player, "MageAttack1_3") {}
        ~MageAttack1_3() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
        void DrawDebug()                        override;

    private:
        void PlayAnimation(); // アニメーション再生

        [[nodiscard]] const bool CheckNextInput(); // 先行入力判定

    private:
        float playAnimationSpeed_ = 1.0f;
        float animationStartFrame_ = 0.0f;
    };
}