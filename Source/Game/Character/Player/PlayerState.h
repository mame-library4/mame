#pragma once
#include "StateMachine/State.h"
#include "Player.h"

namespace PlayerState
{
    // ----- ‘Ò‹@ -----
    class IdleState : public State<Player>
    {
    public:
        IdleState(Player* player) : State(player, "IdleState") {}
        ~IdleState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        int isInputMove();  // “ü—Í‚É‰‚¶‚ÄØ‚è‘Ö‚¦‚éƒXƒe[ƒg‚ğ”»’f‚·‚é
    };

    // ---- ˆÚ“® -----
#pragma region ˆÚ“®
    class MoveState : public State<Player>
    {
    public:
        MoveState(Player* player) : State(player, "MoveState") {}
        ~MoveState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };


    // ----- •à‚« -----
    class WalkState : public State<Player>
    {
    public:
        WalkState(Player* player) : State(player, "WalkState") {}
        ~WalkState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        int isInputMove();  // “ü—Í’l‚É‰‚¶‚Ä’l‚ğ•Ô‚·
    };

    // ----- ‘–‚è -----
    class RunState : public State<Player>
    {
    public:
        RunState(Player* player) : State(player, "RunState") {}
        ~RunState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

#pragma endregion ˆÚ“®

    // ---- ‰ñ”ğ -----
#pragma region
    // ----- ‰ñ”ğ -----
    class AvoidanceState : public State<Player>
    {
    public:
        AvoidanceState(Player* player) : State(player, "AvoidanceState") {}
        ~AvoidanceState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

    // ----- ƒJƒEƒ“ƒ^[ -----
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
    };

    // ----- ƒJƒEƒ“ƒ^[UŒ‚ -----
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

    // ----- ãUŒ‚ -----
#pragma region ãUŒ‚

    // ----- ãUŒ‚‚O -----
    class LightAttack0State : public State<Player>
    {
    public:
        LightAttack0State(Player* player) : State(player, "LightAttack0State") {}
        ~LightAttack0State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        float comboAttackFrame_ = 0.34f; // Ÿ‚ÌUŒ‚‚ÉˆÚ‚éƒtƒŒ[ƒ€
        //float animationEndFrame_ = 0.68f;
    };

    // ----- ãUŒ‚‚P -----
    class LightAttack1State : public State<Player>
    {
    public:
        LightAttack1State(Player* player) : State(player, "LightAttack1State") {}
        ~LightAttack1State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        float comboAttackFrame_ = 0.4f; // Ÿ‚ÌUŒ‚‚ÉˆÚ‚éƒtƒŒ[ƒ€
    };

    // ----- ãUŒ‚‚Q -----
    class LightAttack2State : public State<Player>
    {
    public:
        LightAttack2State(Player* player) : State(player, "LightAttack2State") {}
        ~LightAttack2State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

#pragma endregion ãUŒ‚

    // ----- ‹­UŒ‚ -----
#pragma region ‹­UŒ‚

    // ----- ‹­UŒ‚‚O -----
    class StrongAttack0State : public State<Player>
    {
    public:
        StrongAttack0State(Player* player) : State(player, "StrongAttack0State") {}
        ~StrongAttack0State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        float comboAttackFrame_ = 0.7f; // Ÿ‚ÌUŒ‚‚ÉˆÚ‚éƒtƒŒ[ƒ€
    };

    // ----- ‹­UŒ‚‚P -----
    class StrongAttack1State : public State<Player>
    {
    public:
        StrongAttack1State(Player* player) : State(player, "StrongAttack1State") {}
        ~StrongAttack1State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

#pragma endregion ‹­UŒ‚

    // ----- ƒ_ƒ[ƒWH‚ç‚¢ -----
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