#pragma once
#include <memory>
#include "../Character.h"
#include "StateMachine/StateMachine.h"
#include "Input.h"
#include "Graphics.h"
#include "Effect/SwordTrail/SwordTrail.h"

class Player : public Character
{
public:// --- 定数 ---
#pragma region 定数 
    enum class STATE
    {
        Idle,           // 待機
        Move,           // 移動
        Damage,         // ダメージ
        Death,          // 死亡
        Avoidance,      // 回避
        Counter,        // カウンター
        CounterCombo,   // カウンターコンボ
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
        Idle,               // 待機        
        Walk,               // 歩き
        Run,                // 走り
        GetHit,             // ダメージ食らい
        Down,               // ダウン
        Death,              // 死亡
        GetUp,              // 起き上がり
        StepFront,          // 回避前
        StepBack,           // 回避後ろ
        StepRight,          // 回避右
        StepLeft,           // 回避左

        Attack0,
        Attack1,
        Attack2,

        Counter,

        Attack3,

        ComboAttack0_1,
        ComboAttack0_2,
        ComboAttack0_3,
        ComboAttack0_4,

        ComboAttack1_0,
        ComboAttack1_1,
        ComboAttack1_2,
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

    void Initialize();
    void Finalize();
    void Update(const float& elapsedTime)   override;
    void Render(ID3D11PixelShader* psShader)override;
    void RenderTrail();
    void DrawDebug()                        override;  
    void DebugRender(DebugRenderer* debugRenderer);

    void Turn(const float& elapsedTime);
    void Move(const float& elapsedTime);

    bool CheckNextInput(const Player::NextInput& nextInput);

    void ResetFlags(); // フラグをリセットする


    void PlayAnimation(const Animation& index, const bool& loop, const float& speed = 1.0f)
    {
        Object::PlayAnimation(static_cast<int>(index), loop, speed);
        //swordModel_.PlayAnimation(static_cast<int>(index), loop, speed);
    }
    void SetAnimationSpeed(const float& speed)
    {
        Object::SetAnimationSpeed(speed);
        //swordModel_.SetAnimationSpeed(speed);
    }
    float slowAnimationSpeed_ = 0.15f;
    //float slowAnimationSpeed_ = 0.25f;

    void PlayBlendAnimation(const Animation& index1, const Animation& index2, const bool& loop, const float& speed = 1.0f);
    void PlayBlendAnimation(const Animation& index, const bool& loop, const float& speed = 1.0f);

    void UpdateCollisions(const float& elapsedTime, const float& scaleFactor) override;

    void SetWeight(const float& weight) override;
    void AddWeight(const float& weight) override;

    // 攻撃判定有効フラグ設定
    void SetAttackFlag(const bool& activeFlag = true);
    bool GetIsActiveAttackFlag();

public:// --- 取得・設定 ---
#pragma region [Get, Set] Function
    // ---------- ステートマシン --------------------
    StateMachine<State<Player>>* GetStateMachine() { return stateMachine_.get(); }
    void ChangeState(const STATE& state) { stateMachine_.get()->ChangeState(static_cast<int>(state)); }

    // ---------- 行動 -------------------------------------------------------
    [[nodiscard]] const NextInput GetNextInput() const { return nextInput_; }
    [[nodiscard]] const bool GetIsAvoidance() const { return isAvoidance_; }
    void SetIsAvoidance(const bool& isAvoidance) { isAvoidance_ = isAvoidance; }

    // ---------- キー入力 ----------
    [[nodiscard]] bool GetComboAttack0KeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_B; }
    [[nodiscard]] bool GetComboAttack1KeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y; }
    [[nodiscard]] bool GetAvoidanceKeyDown()    const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_A; }
    [[nodiscard]] bool GetCounterStanceKey()    const { return Input::Instance().GetGamePad().GetButton() & GamePad::BTN_B && Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_TRIGGER; }

#pragma endregion [Get, Set] Function

private:
    // ---------- ステートマシン --------------------
    std::unique_ptr<StateMachine<State<Player>>> stateMachine_;

    // ----- 移動 -----
    DirectX::XMFLOAT3 moveDirection_ = {};

    // ---------- 行動 ------------------------------
    NextInput nextInput_ = NextInput::None; // 先行入力
    bool isAvoidance_ = false; // 回避

    // ---------- Debug用 --------------------
    bool isCollisionSphere_ = true;
    bool isDamageSphere_ = true;
    bool isAttackSphere_ = true;

    DirectX::XMFLOAT3 offset_ = {};

    SwordTrail swordTrail_;
};
