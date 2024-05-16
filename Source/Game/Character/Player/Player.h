#pragma once
#include <memory>
#include "../Character.h"
#include "../../../AI/StateMachine/StateMachine.h"
#include "../../../Input/Input.h"
#include "../Graphics/Graphics.h"

class Player : public Character
{
public:// --- 定数 ---
#pragma region 定数 
    enum class STATE
    {
        Idle,               // 待機
        Walk,               // 歩き
        Run,                // 走り

        Avoidance,          // 回避
        Counter,            // カウンター
        CounterAttack,      // カウンター攻撃 

        LightAttack0,       // 弱攻撃0
        LightAttack1,       // 弱攻撃1
        LightAttack2,       // 弱攻撃2

        StrongAttack0,      // 強攻撃0
        StrongAttack1,      // 強攻撃1

        Damage,

        Move,
    };

    enum class Animation
    {
        Idle,       // 待機
        Run,        // 走り   
        LightAttack0,
        LightAttack1,
        LightAttack2,
        Avoidance,  // 回避

        Protect,

        StrongAttack0,
        Damage0,
        Damage1,
        StrongAttack1,
        StrongAttack2,
        Walk,
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

    void UpdateAttackState(const Player::STATE& state);
    void ResetFlags(); // フラグをリセットする


    void PlayAnimation(const Animation& index, const bool& loop, const float& speed = 1.0f)
    {
        Object::PlayAnimation(static_cast<int>(index), loop, speed);
    }

    // ----- 吹っ飛ばす処理 -----
    void UpdateForce(const float& elapsedTime);
    void AddForce(const DirectX::XMFLOAT3& direction, const float& power);

public:// --- 取得・設定 ---
#pragma region [Get, Set] Function
    // ---------- ステートマシン --------------------
    StateMachine<State<Player>>* GetStateMachine() { return stateMachine_.get(); }
    void ChangeState(const STATE& state) { stateMachine_.get()->ChangeState(static_cast<int>(state)); }

    // ---------- 行動 -------------------------------------------------------
    [[nodiscard]] const int GetNextInput() const { return nextInput_; }
    void SetNextInput(const NextInput& nextInput) { nextInput_ = static_cast<int>(nextInput); }
    [[nodiscard]] const bool GetIsAvoidance() const { return isAvoidance_; }
    void SetIsAvoidance(const bool& isAvoidance) { isAvoidance_ = isAvoidance; }

    // ---------- キー入力 ----------
    [[nodiscard]] bool GetLightAttackKeyUp() { return Input::Instance().GetGamePad().GetButtonUp() & GamePad::BTN_X; }
    [[nodiscard]] bool GetLightAttackKeyDown() { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_X; }
    [[nodiscard]] bool GetStrongAttackKeyUp() { return Input::Instance().GetGamePad().GetButtonUp() & GamePad::BTN_Y; }
    [[nodiscard]] bool GetStrongAttackKeyDown() { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y; }

#pragma endregion [Get, Set] Function

private:
    // ---------- ステートマシン --------------------
    std::unique_ptr<StateMachine<State<Player>>> stateMachine_;

    // ---------- 行動 ------------------------------
    int nextInput_ = false; // 先行入力
    bool isAvoidance_ = false; // 回避

    int animationIndex_ = 0;
    float speed_ = 1.0f;

    // ---------- 吹っ飛び --------------------
    DirectX::XMFLOAT3 blowDirection_ = {};
    float blowPower_ = 0.0f;

    // ---------- Debug用 --------------------
    bool isCollisionSphere_ = true;
    bool isDamageSphere_ = true;
    bool isAttackSphere_ = true;

    DirectX::XMFLOAT3 offset_ = {};
};
