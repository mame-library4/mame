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
        Idle,               // 待機

        Move,
        //Walk,               // 歩き
        //Run,                // 走り

        Avoidance,          // 回避
        Counter,            // カウンター
        CounterAttack,      // カウンター攻撃 

        LightAttack0,       // 弱攻撃0
        LightAttack1,       // 弱攻撃1
        LightAttack2,       // 弱攻撃2

        StrongAttack0,      // 強攻撃0
        StrongAttack1,      // 強攻撃1

        Damage,
    };

    enum class Animation
    {
        Idle,       // 待機
        Walk,
        Run,        // 走り   
        
        LightAttack0,
        LightAttack1,
        LightAttack2,

        StrongAttack0,
        StrongAttack1,

        Damage0,
        Damage1,
    };

    enum class NextInput
    {
        None,           // 先行入力なし
        LightAttack,    // 弱攻撃
        StrongAttack,   // 強攻撃
    };

#pragma endregion 定数

public:
    Player();
    ~Player() override;

    void Initialize();
    void Finalize();
    void Update(const float& elapsedTime)   override;
    void Render()                           override;
    void DrawDebug()                        override;  
    void DebugRender(DebugRenderer* debugRenderer);

    void Turn(const float& elapsedTime);
    void Move(const float& elapsedTime);

    bool CheckAttackButton(const Player::NextInput& nextInput);

    void ResetFlags(); // フラグをリセットする


    void PlayAnimation(const Animation& index, const bool& loop, const float& speed = 1.0f)
    {
        Object::PlayAnimation(static_cast<int>(index), loop, speed);
        swordModel_.PlayAnimation(static_cast<int>(index), loop, speed);
    }


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
    [[nodiscard]] const int GetNextInput() const { return nextInput_; }
    [[nodiscard]] const bool GetIsAvoidance() const { return isAvoidance_; }
    void SetIsAvoidance(const bool& isAvoidance) { isAvoidance_ = isAvoidance; }

    // ---------- キー入力 ----------
    [[nodiscard]] bool GetLightAttackKeyUp() { return Input::Instance().GetGamePad().GetButtonUp() & GamePad::BTN_X; }
    [[nodiscard]] bool GetLightAttackKeyDown() { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_X; }
    [[nodiscard]] bool GetStrongAttackKeyUp() { return Input::Instance().GetGamePad().GetButtonUp() & GamePad::BTN_Y; }
    [[nodiscard]] bool GetStrongAttackKeyDown() { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y; }

#pragma endregion [Get, Set] Function

private:
    // ---------- 剣 ----------
    GltfModel swordModel_;

    // ---------- ステートマシン --------------------
    std::unique_ptr<StateMachine<State<Player>>> stateMachine_;

    // ----- 移動 -----
    DirectX::XMFLOAT3 moveDirection_ = {};

    // ---------- 行動 ------------------------------
    int nextInput_ = false; // 先行入力
    bool isAvoidance_ = false; // 回避

    // ---------- Debug用 --------------------
    bool isCollisionSphere_ = true;
    bool isDamageSphere_ = true;
    bool isAttackSphere_ = true;

    DirectX::XMFLOAT3 offset_ = {};

    SwordTrail swordTrail_;
};
