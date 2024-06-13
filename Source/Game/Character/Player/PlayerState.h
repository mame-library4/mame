#pragma once
#include "StateMachine/State.h"
#include "Player.h"

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

    private:
        int isInputMove();  // 入力に応じて切り替えるステートを判断する
    };

    // ---- 移動 -----
#pragma region 移動
    class MoveState : public State<Player>
    {
    public:
        MoveState(Player* player) : State(player, "MoveState") {}
        ~MoveState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };


    // ----- 歩き -----
    class WalkState : public State<Player>
    {
    public:
        WalkState(Player* player) : State(player, "WalkState") {}
        ~WalkState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        int isInputMove();  // 入力値に応じて値を返す
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
    };

#pragma endregion 移動

    // ---- 回避 -----
#pragma region
    // ----- 回避 -----
    class AvoidanceState : public State<Player>
    {
    public:
        AvoidanceState(Player* player) : State(player, "AvoidanceState") {}
        ~AvoidanceState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
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

    private:
        float power_ = 0.0f;
        float addForceFrame_ = 0.0f;
        bool isAddForce_ = false;

        float animationSlowStartFrame_ = 0.0f;
        float animationSlowEndFrame_ = 0.0f;
    };

    // ----- カウンター攻撃 -----
    class CounterAttackState : public State<Player>
    {
    public:
        CounterAttackState(Player* player) : State(player, "Counter") {}
        ~CounterAttackState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        float addForceFrame_ = 0.0f;
        bool isAddForce_ = false;
    };

#pragma endregion

    // ----- 弱攻撃 -----
#pragma region 弱攻撃

    // ----- 弱攻撃０ -----
    class LightAttack0State : public State<Player>
    {
    public:
        LightAttack0State(Player* player) : State(player, "LightAttack0State") {}
        ~LightAttack0State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        float comboAttackFrame_ = 0.34f; // 次の攻撃に移るフレーム
        //float animationEndFrame_ = 0.68f;
    };

    // ----- 弱攻撃１ -----
    class LightAttack1State : public State<Player>
    {
    public:
        LightAttack1State(Player* player) : State(player, "LightAttack1State") {}
        ~LightAttack1State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        float comboAttackFrame_ = 0.4f; // 次の攻撃に移るフレーム
    };

    // ----- 弱攻撃２ -----
    class LightAttack2State : public State<Player>
    {
    public:
        LightAttack2State(Player* player) : State(player, "LightAttack2State") {}
        ~LightAttack2State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

#pragma endregion 弱攻撃

    // ----- 強攻撃 -----
#pragma region 強攻撃

    // ----- 強攻撃０ -----
    class StrongAttack0State : public State<Player>
    {
    public:
        StrongAttack0State(Player* player) : State(player, "StrongAttack0State") {}
        ~StrongAttack0State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        float comboAttackFrame_ = 0.7f; // 次の攻撃に移るフレーム
    };

    // ----- 強攻撃１ -----
    class StrongAttack1State : public State<Player>
    {
    public:
        StrongAttack1State(Player* player) : State(player, "StrongAttack1State") {}
        ~StrongAttack1State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

#pragma endregion 強攻撃

    // ----- ダメージ食らい -----
    class DamageState : public State<Player>
    {
    public:
        DamageState(Player* player) : State(player, "DamageState") {}
        ~DamageState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };
}