#pragma once
#include <memory>
#include "../Character.h"
#include "StateMachine/StateMachine.h"
#include "Input.h"
#include "Graphics.h"
#include "Effect/SwordTrail/SwordTrail.h"

#include "Effect/Effect.h"

class Player : public Character
{
public:// --- 定数 ---
#pragma region 定数 
    enum class STATE
    {
        Idle,           // 待機
        //Move,           // 移動
        Walk,
        Run,
        Flinch,
        Damage,         // ダメージ
        Death,          // 死亡
        Avoidance,      // 回避
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
        // 待機
        Idle,

        // 歩き
        WalkStart,
        Walk,
        WalkEnd,

        // 走り
        RunStart,
        Run,
        RunEnd,

        // 回避
        RollForward,
        RollBack,
        RollRight,
        RollLeft,

        // 攻撃
        ComboAttack0_0,
        ComboAttack0_1,
        ComboAttack0_2,
        ComboAttack0_3,

        // 防御
        BlockStart,
        BlockLoop,
        BlockEnd,
        
        // カウンター攻撃
        ParryCounterAttack0, // Right
        ParryCounterAttack1, // Left

        HitLarge,
        GetUp,
        HitForward,
        HitBack,
        HitRight,
        HitLeft,

        KnockDownStart,
        KnockDownLoop,
        KnockDownEnd,
        KnockDownDeath,

        ComboAttack1_0,
        ComboAttack1_1,
        ComboAttack1_2,
        ComboAttack1_3,

        RunAttack0,
        RunAttack1,

        Counter,
    };

    enum class NextInput
    {
        None,           // 先行入力なし
        ComboAttack0,   // コンボ攻撃0
        ComboAttack1,   // コンボ攻撃1
        Avoidance,      // 回避
    };

#pragma endregion 定数

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

    // ---------- Collision ----------
    void UpdateCollisions(const float& elapsedTime) override;
    void UpdateCollisionDetectionData();    // 押し出し判定位置更新

    // ---------- 剣の座標更新 ----------
    void UpdateSwordTransform();

    bool Player::CheckNextInput(const Player::NextInput& nextInput);

public:// --- 取得・設定 ---
#pragma region [Get, Set] Function
    // ---------- ステートマシン --------------------
    StateMachine<State<Player>>* GetStateMachine() { return stateMachine_.get(); }
    void ChangeState(const STATE& state);
    [[nodiscard]] const STATE GetCurrentState() const { return currentState_; }

    // ---------- 移動 ------------------------------
    void SetMoveDirection(const DirectX::XMFLOAT3 direction) { moveDirection_ = direction; } // 移動方向

    // ---------- 行動 -------------------------------------------------------
    // ----- フラグをリセット -----
    void ResetFlags();    
    // ----- 先行入力 -----
    [[nodiscard]] const NextInput GetNextInput() const { return nextInput_; }
    void SetNextInput(const NextInput& nextInput) { nextInput_ = nextInput; }
    // ----- 回避 -----
    [[nodiscard]] const bool GetIsAvoidance() const { return isAvoidance_; }
    void SetIsAvoidance(const bool& isAvoidance) { isAvoidance_ = isAvoidance; }
    // ----- カウンター状態か -----
    [[nodiscard]] const bool GetIsCounter() const { return isCounter_; }
    void SetIsCounter(const bool& flag) { isCounter_ = flag; }
    // ----- カウンター(追加の)攻撃が可能か -----
    [[nodiscard]] const bool GetIsAbleCounterAttack() const { return isAbleCounterAttack_; }
    void SetIsAbleCounterAttack(const bool& flag) { isAbleCounterAttack_ = flag; }
    // ----- 攻撃判定 -----
    [[nodiscard]] const bool GetIsAbleAttack() const { return isAbleAttack_; }
    void SetIsAbleAttack(const bool& flag) { isAbleAttack_ = flag; }

    // ----- カウンター有効範囲 -----
    [[nodiscard]] const float GetCounterActiveRadius() const { return counterActiveRadius_; }
    void SetCounterActiveRadius(const float& range) { counterActiveRadius_ = range; }

    // ---------- キー入力 ----------
    [[nodiscard]] bool GetComboAttack0KeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_B; }
    [[nodiscard]] bool GetComboAttack1KeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y; }
    [[nodiscard]] bool GetAvoidanceKeyDown()    const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_A; }
    [[nodiscard]] bool GetCounterStanceKey()    const { return Input::Instance().GetGamePad().GetButton() & GamePad::BTN_B && Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_TRIGGER; }

#pragma endregion [Get, Set] Function

private:
    // ---------- Collision ----------
    void RegisterCollisionData();

private:
    // ---------- 武器 ----------
    GltfModel weapon_;

    // ---------- ステートマシン --------------------
    std::unique_ptr<StateMachine<State<Player>>> stateMachine_;
    STATE currentState_ = STATE::Idle;

    // ---------- 移動 ----------
    DirectX::XMFLOAT3 moveDirection_ = {};

    // ---------- 行動 ------------------------------
    NextInput nextInput_        = NextInput::None;  // 先行入力
    bool isAvoidance_           = false;            // 回避
    bool isCounter_             = false;            // カウンター状態か
    bool isAbleCounterAttack_   = false;            // カウンター攻撃が可能か( 追加の )
    bool isAbleAttack_          = false;            // 攻撃可能か

    // ---------- カウンター有効範囲 ----------
    float counterActiveRadius_   = 0.0f; // カウンター有効範囲

    // ---------- Debug用 --------------------
    bool isCollisionSphere_ = true;
    bool isDamageSphere_ = true;
    bool isAttackSphere_ = true;


    SwordTrail swordTrail_;

#if 1
    DirectX::XMFLOAT3 socketLocation_ = { 700, 10500, -7000 };
    DirectX::XMFLOAT3 socketRotation_ = { 145.0f, -16.0f, 0.0f };
    DirectX::XMFLOAT3 socketScale_ = { 1.0f, 1.0f, -1.0f };
#else   
    DirectX::XMFLOAT3 socketLocation_ = { 770, 12000, 2500 };
    DirectX::XMFLOAT3 socketRotation_ = { -100.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 socketScale_ = { 1.0f, 1.0f, 1.0f };
#endif  
    DirectX::XMFLOAT4X4 weaponWorld_;

};
