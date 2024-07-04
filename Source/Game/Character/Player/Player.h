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
        //Move,           // 移動
        Walk,
        Run,
        Flinch,
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

    bool CheckNextInput(const Player::NextInput& nextInput, const float& nextAttackFrame = -1);

    void ResetFlags(); // フラグをリセットする


    // 剣の座標更新
    void UpdateSwordTransform();

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

    
    void PlayBlendAnimation(const Animation& index, const bool& loop, const float& speed = 1.0f, const float& blendAnimationFrame = 0.0f) { Object::PlayBlendAnimation(static_cast<int>(index), loop, speed, blendAnimationFrame); }

    void UpdateCollisions(const float& elapsedTime) override;


    void SetMoveDirection(const DirectX::XMFLOAT3 direction) { moveDirection_ = direction; }

    // ---------- Animation ----------
    [[nodiscard]] const bool GetUseBlendAnimation() const { return useBlendAnimation_; }
    void SetUseBlendAnimation(const bool& flag) { useBlendAnimation_ = flag; }

    // 攻撃判定有効フラグ設定
    void SetAttackFlag(const bool& activeFlag = true);
    bool GetIsActiveAttackFlag();

public:// --- 取得・設定 ---
#pragma region [Get, Set] Function
    // ---------- ステートマシン --------------------
    StateMachine<State<Player>>* GetStateMachine() { return stateMachine_.get(); }
    void ChangeState(const STATE& state);

    // ---------- 行動 -------------------------------------------------------
    [[nodiscard]] const NextInput GetNextInput() const { return nextInput_; }
    void SetNextInput(const NextInput& nextInput) { nextInput_ = nextInput; }

    [[nodiscard]] const bool GetIsAvoidance() const { return isAvoidance_; }
    void SetIsAvoidance(const bool& isAvoidance) { isAvoidance_ = isAvoidance; }

    // ---------- キー入力 ----------
    [[nodiscard]] bool GetComboAttack0KeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_B; }
    [[nodiscard]] bool GetComboAttack1KeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y; }
    [[nodiscard]] bool GetAvoidanceKeyDown()    const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_A; }
    [[nodiscard]] bool GetCounterStanceKey()    const { return Input::Instance().GetGamePad().GetButton() & GamePad::BTN_B && Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_TRIGGER; }

#pragma endregion [Get, Set] Function

private:
    // ---------- 武器 ----------
    GltfModel weapon_;

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

    // ---------- Animation ----------
    bool useBlendAnimation_ = false;

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
