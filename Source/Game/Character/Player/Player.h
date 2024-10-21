#pragma once
#include <memory>
#include "../Character.h"
#include "StateMachine/StateMachine.h"
#include "Input.h"
#include "Graphics.h"
#include "Effect/SwordTrail/SwordTrail.h"

#include "Effect/Effect.h"

#include "Sword/Sword.h"

class Player : public Character
{
public:// --- 定数 ---
#pragma region ----- 定数 -----
    enum class STATE
    {
        Idle,           // 待機
        Run,

        GuardCounter,       // ガードカウンター
        GuardCounterAttack, // ガードカウンター攻撃

        LightFlinch,
        Flinch,
        Damage,         // ダメージ
        Death,          // 死亡
        Dodge,          // 回避

        Counter,        // カウンター
        CounterCombo,   // カウンターコンボ

        RunAttack,

        ComboAttack0_0,   // コンボ0
        ComboAttack0_1,   // コンボ0
        ComboAttack0_2,   // コンボ0
        ComboAttack0_3,   // コンボ0
        
        ComboAttack1_0,   // コンボ1
        ComboAttack1_1,   // コンボ1
        ComboAttack1_2,   // コンボ1

        
    };

    enum class Animation
    {
        Idle,           // 待機
        Walk,           // 歩き
        Run,            // 走り
        RollFront,      // 回避前
        RollBack,       // 回避後ろ
        RollRight,      // 回避右
        RollLeft,       // 回避左
        Attack0_0,      // 攻撃0_0
        Attack0_1,      // 攻撃0_1
        Attack0_2,      // 攻撃0_2
        Attack0_3,      // 攻撃0_3
        BlockStart,     // 防御開始
        BlockLoop,      // 防御ループ
        BlockEnd,       // 防御終わり        
        CounterAttack0, // カウンター攻撃0
        CounterAttack1, // カウンター攻撃0
        Damage,         // ダメージ
        GetUp,          // 起き上がり
        HitFront,       // 弱ダメージ前
        HitBack,        // 弱ダメージ後ろ
        HitRight,       // 弱ダメージ右
        HitLeft,        // 弱ダメージ左
        DownStart,      // ダウン開始
        DownLoop,       // ダウンループ
        DownEnd,        // ダウン終わり
        DownDeath,      // ダウン死亡
        Attack1_0,      // 攻撃1_0
        Attack1_1,      // 攻撃1_1
        Attack1_2,      // 攻撃1_2
        Attack1_3,      // 攻撃1_3
        RunAttack0,     // 走り攻撃0
        RunAttack1,     // 走り攻撃1
        Counter,        // カウンター
    };

    // 先行入力の種類
    enum class NextInput
    {
        None,           // 先行入力なし
        ComboAttack0,   // コンボ攻撃
        Dodge,      // 回避
        Counter,        // カウンター
    };

#pragma endregion ----- 定数 -----

public:
    Player();
    ~Player() override;

    void Initialize();                                  // 初期化
    void Finalize();                                    // 終了化
    void Update(const float& elapsedTime)    override;  // 更新
    void Render(ID3D11PixelShader* psShader) override;  // 描画
    void DrawDebug()                         override;  // ImGui用
    
    void RenderTrail();                                 // 剣の軌跡描画
    void DebugRender(DebugRenderer* debugRenderer);     // 判定用図形描画

    // ---------- Animation ----------
    void PlayAnimation(const Animation& index, const bool& loop, const float& speed = 1.0f) { Object::PlayAnimation(static_cast<int>(index), loop, speed); }
    void PlayBlendAnimation(const Animation& index, const bool& loop, const float& speed = 1.0f, const float& blendAnimationFrame = 0.0f) { Object::PlayBlendAnimation(static_cast<int>(index), loop, speed, blendAnimationFrame); }
    void SetAnimationSpeed(const float& speed) { Object::SetAnimationSpeed(speed); }

    // ---------- 移動＆回転 ----------
    void Turn(const float& elapsedTime); // 旋回処理
    void Move(const float& elapsedTime); // 移動処理

    void CalculateRotationAdjustment(); // 回転補正量設定
    void UpdateRotationAdjustment(const float& elapsedTime);    // 回転補正

    // ---------- スタミナ ----------
    void UpdateStaminaRecovery(const float& elapsedTime);

    // ---------- Collision ----------
    void UpdateCollisions(const float& elapsedTime) override;
    void UpdateCollisionDetectionData();    // 押し出し判定位置更新

    // ---------- 剣の座標更新 ----------
    void UpdateSwordTransform();

public:// --- 取得・設定 ---
#pragma region [Get, Set] Function
    // ---------- ステートマシン --------------------
    StateMachine<State<Player>>* GetStateMachine() { return stateMachine_.get(); }
    void ChangeState(const STATE& state);
    [[nodiscard]] const STATE GetCurrentState() const { return currentState_; }
    [[nodiscard]] const STATE GetOldState() const { return oldState_; }

    // ---------- 移動 ------------------------------
    void SetMoveDirection(const DirectX::XMFLOAT3 direction) { moveDirection_ = direction; } // 移動方向

    [[nodiscard]] const float GetDashSpeed() const { return dashSpeed_; }
    [[nodiscard]] const float GetDashAnimationSpeed() const { return dashAnimationSpeed_; }
    [[nodiscard]] const bool GetIsDash() const { return isDash_; }
    void SetIsDash(const bool& flag) { isDash_ = flag; }

    // ---------- スタミナ ----------
    [[nodiscard]] const float GetStamina() const { return stamina_; }
    [[nodiscard]] const float GetMaxStamina() const { return maxStamina_; }
    void SetStanima(const float& stamina) { stamina_ = stamina; }
    
    [[nodiscard]] const float GetDodgeStaminaCost() const { return dodgeStaminaCost_; }
    [[nodiscard]] const float GetDashStamiaCost() const { return dashStaminaCost_; }
    void UseDodgeStamina() { stamina_ -= dodgeStaminaCost_; }
    void UseDashStamina(const float& elapsedTime);
    
    [[nodiscard]] const float GetStaminaRecoverySpeed() const { return staminaRecoverySpeed_; }
    void SetStaminaRecoverySpeed(const float& speed) { staminaRecoverySpeed_ = speed; }

    [[nodiscard]] const bool GetIsStaminaDepleted() const { return isStaminaDepleted; }

    // ---------- 行動 -------------------------------------------------------
    // ----- フラグをリセット -----
    void ResetFlags();    
    // ----- 先行入力 -----
    void SetNextInputStartFrame(const float& dodge = 0.0f, const float& attack = 0.0f, const float& counter = 0.0f, const float& move = 0.0f);
    void SetNextInputEndFrame(const float& dodge = 10.0f, const float& attack = 10.0f, const float& counter = 10.0f);
    void SetNextInputTransitionFrame(const float& dodge = 0.0f, const float& attack = 0.0f, const float& counter = 0.0f);


    // ----- 攻撃キャンセル -----
    [[nodiscard]] const bool GetIsMoveAttackCancel() const { return isMoveAttackCancel_; }
    void SetIsMoveAttackCancel(const bool& flag) { isMoveAttackCancel_ = flag; }
    [[nodiscard]] const bool GetIsDodgeAttackCancel() const { return isDodgeAttackCancel_; }
    void SetIsDodgeAttackCancel(const bool& flag) { isDodgeAttackCancel_ = flag; }

    [[nodiscard]] const NextInput GetNextInput() const { return nextInput_; }
    void SetNextInput(const NextInput& nextInput) { nextInput_ = nextInput; }
    // ----- 回避 -----
    [[nodiscard]] const bool GetIsDodge() const { return isDodge_; }
    void SetIsDodge(const bool& flag) { isDodge_ = flag; }
    // ----- カウンター状態か -----
    [[nodiscard]] const bool GetIsCounter() const { return isCounter_; }
    void SetIsCounter(const bool& flag) { isCounter_ = flag; }
    // ----- カウンター(追加の)攻撃が可能か -----
    [[nodiscard]] const bool GetIsAbleCounterAttack() const { return isAbleCounterAttack_; }
    void SetIsAbleCounterAttack(const bool& flag) { isAbleCounterAttack_ = flag; }
    
    // ----- 攻撃判定 -----
    [[nodiscard]] const bool GetIsAttackValid() const { return isAttackValid_; }
    void SetIsAttackValid(const bool& flag) { isAttackValid_ = flag; }

    [[nodiscard]] const bool GetIsAttackHit() const { return isAttackHit_; }
    void SetIsAttackHit(const bool& flag) { isAttackHit_ = flag; }

    // ----- 無敵判定 -----
    [[nodiscard]] const bool GetIsInvincible() const { return isInvincible_; }
    void SetIsInvincible(const bool& flag) { isInvincible_ = flag; }

    // ----- カウンター有効範囲 -----
    [[nodiscard]] const float GetCounterActiveRadius() const { return counterActiveRadius_; }
    void SetCounterActiveRadius(const float& range) { counterActiveRadius_ = range; }

    // ---------- ガードカウンター　----------
    [[nodiscard]] const float GetGuardCounterRadius() const { return guardCounterRadius_; }
    void SetGuardCounterRadius(const float& radius) { guardCounterRadius_ = radius; }
    [[nodiscard]] const bool GetIsGuardCounterStance() const { return isGuardCounterStance_; }
    void SetIsGuardCounterStance(const bool& flag) { isGuardCounterStance_ = flag; }
    [[nodiscard]] const bool GetIsGuardCounterSuccessful() const { return isGuardCounterSuccessful_; }
    void SetIsGuardCounterSuccessful(const bool& flag) { isGuardCounterSuccessful_ = flag; }

    // ---------- キー入力 ----------
    [[nodiscard]] bool IsComboAttack0KeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_B; }
    [[nodiscard]] bool IsDodgeKeyDown()        const;
    [[nodiscard]] bool IsCounterStanceKey()    const;
    [[nodiscard]] bool IsGetUpKeyDown()        const;
    [[nodiscard]] bool IsDashKey()             const;

    [[nodiscard]] bool IsGuardCounterKeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_X; }

#pragma endregion [Get, Set] Function

    [[nodiscard]] const float GetMoveInputStartFrame()      const { return moveInputStartFrame_; }
    [[nodiscard]] const float GetDodgeInputStartFrame()     const { return dodgeInputStartFrame_; }
    [[nodiscard]] const float GetDodgeInputEndFrame()       const { return dodgeInputEndFrame_; }
    [[nodiscard]] const float GetDodgeTransitionFrame()     const { return dodgeTransitionFrame_; }
    [[nodiscard]] const float GetAttackInputStartFrame()    const { return attackInputStartFrame_; }
    [[nodiscard]] const float GetAttackInputEndFrame()      const { return attackInputEndFrame_; }
    [[nodiscard]] const float GetAttackTransitionFrame()    const { return attackTransitionFrame_; }
    [[nodiscard]] const float GetCounterInputStartFrame()   const { return counterInputStartFrame_; }
    [[nodiscard]] const float GetCounterInputEndFrame()     const { return counterInputEndFrame_; }
    [[nodiscard]] const float GetCounterTransitionFrame()   const { return counterTransitionFrame_; }

private:
    // ---------- Collision ----------
    void RegisterCollisionData();

private:
    // ---------- 武器 ----------
    GltfModel weapon_;

    // ---------- ステートマシン --------------------
    std::unique_ptr<StateMachine<State<Player>>> stateMachine_;
    STATE currentState_ = STATE::Idle;
    STATE oldState_     = STATE::Idle;

    // ---------- 移動 ----------
    DirectX::XMFLOAT3 moveDirection_        = {};
    float             dashSpeed_            = 8.0f;  // ダッシュ時の速度
    float             dashAnimationSpeed_   = 1.3f;  // ダッシュのアニメーション速度
    bool              isDash_               = false; // 現在ダッシュしているか

    // ---------- 回転 ----------
    float startAngle_               = 0.0f;     // 補間開始地点
    float endAngle_                 = 0.0f;     // 補間終了地点
    float rotationTimer_            = 0.0f;     // 補間制御用
    float rotateAngleThreshold_     = 0.52f;    // 回転角閾値
    float rotationAmount_           = 0.52f;    // 回転量
    float rotationSpeed_            = 3.0f;     // 回転速度
    bool  useRotationAdjustment_    = false;    // 更新処理をするか

    // ---------- 行動 ------------------------------
    NextInput   nextInput_                  = NextInput::None;  // 先行入力保存用
    float       dodgeInputStartFrame_       = 0.0f;             // 回避先行入力開始フレーム
    float       attackInputStartFrame_      = 0.0f;             // 攻撃先行入力開始フレーム
    float       counterInputStartFrame_     = 0.0f;             // カウンター先行入力開始フレーム
    float       moveInputStartFrame_        = 0.0f;
    float       dodgeInputEndFrame_         = 0.0f;
    float       attackInputEndFrame_        = 0.0f;
    float       counterInputEndFrame_       = 0.0f;
    float       dodgeTransitionFrame_       = 0.0f;             // 回避へ遷移可能フレーム
    float       attackTransitionFrame_      = 0.0f;             // 攻撃へ遷移可能フレーム
    float       counterTransitionFrame_     = 0.0f;             // カウンターへ遷移可能フレーム

    bool isDodge_           = false;            // 回避
    bool isCounter_             = false;            // カウンター状態か
    bool isAbleCounterAttack_   = false;            // カウンター攻撃が可能か( 追加の )
    
    bool isAttackValid_         = false;            // 攻撃が有効か
    bool isAttackHit_           = false;            // 攻撃が当たったか

    bool isMoveAttackCancel_ = false;  // 移動入力による攻撃後隙キャンセル
    bool isDodgeAttackCancel_ = false; // 回避による攻撃キャンセル

    // ---------- カウンター有効範囲 ----------
    float counterActiveRadius_   = 0.0f; // カウンター有効範囲

    // ---------- 無敵 ----------
    bool isInvincible_ = false;

    // ---------- Debug用 --------------------
    bool isCollisionSphere_ = true;
    bool isDamageSphere_ = true;
    bool isAttackSphere_ = true;

    // ---------- スタミナ ----------
    float stamina_      = 1.0f;
    float maxStamina_   = 1.0f;
    float staminaRecoverySpeed_ = 15.0f;

    float dodgeStaminaCost_ = 10.0f; // 回避に使うスタミナ量
    float dashStaminaCost_  = 3.0f; // ダッシュに使うスタミナ量

    bool isStaminaDepleted = false;


    SwordTrail swordTrail_;
    Sword sword_;
    bool isSwordPrimitiveDraw_ = false;
    //bool isSwordPrimitiveDraw_ = true;

    // ---------- 剣 ----------
    DirectX::XMFLOAT3 socketLocation_ = { 700, 10500, -7000 };
    DirectX::XMFLOAT3 socketRotation_ = { 145.0f, -16.0f, 0.0f };
    DirectX::XMFLOAT3 socketScale_ = { 1.0f, 1.0f, -1.0f };
    DirectX::XMFLOAT4X4 weaponWorld_;

    // ---------- ガードカウンター　----------
    float guardCounterRadius_       = 0.9f;
    bool  isGuardCounterStance_     = false; // ガードカウンター状態か
    bool  isGuardCounterSuccessful_ = false; // 成功したか
};
