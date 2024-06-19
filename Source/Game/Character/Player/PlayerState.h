#pragma once
#include "StateMachine/State.h"
#include "Player.h"

namespace PlayerState
{
    // ----- �ҋ@ -----
    class IdleState : public State<Player>
    {
    public:
        IdleState(Player* player) : State(player, "IdleState") {}
        ~IdleState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        void SetAnimation();
    };

    // ----- �ړ� -----
    class MoveState : public State<Player>
    {
    public:
        MoveState(Player* player) : State(player, "MoveState") {}
        ~MoveState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

    // ----- �_���[�W -----
    class DamageState : public State<Player>
    {
    public:
        DamageState(Player* player) : State(player, "DamageState") {}
        ~DamageState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

    // ----- ���S -----
    class DeathState : public State<Player>
    {
    public:
        DeathState(Player* player) : State(player, "DeathState") {}
        ~DeathState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

    // ----- ��� -----
    class AvoidanceState : public State<Player>
    {
    public:
        AvoidanceState(Player* player) : State(player, "AvoidanceState") {}
        ~AvoidanceState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        void SetAnimation();
    };

    // ----- �J�E���^�[ -----
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

    // ----- �J�E���^�[�U�� -----
    class CounterComboState : public State<Player>
    {
    public:
        CounterComboState(Player* player) : State(player, "CounterComboState") {}
        ~CounterComboState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        float addForceFrame_ = 0.0f;
        bool isAddForce_ = false;
    };

    // ----- �R���{0_0 -----
    class ComboAttack0_0 : public State<Player>
    {
    public:
        ComboAttack0_0(Player* player) : State(player, "ComboAttack0_0") {}
        ~ComboAttack0_0() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        // ----- ��s���͗p -----
        float nextInputStartFrame_  = 0.0f;
        float nextInputEndFrame_    = 0.0f;

        // ----- �A�j���[�V��������p -----
        float slowAnimationStartFrame_  = 0.0f;
        bool  isSlowAnimation_          = false;
        const float normalAnimationSpeed_   = 1.5f;
        const float slowAnimationSpeed_     = 0.5f;
    };

    // ----- �R���{0_1 -----
    class ComboAttack0_1 : public State<Player>
    {
    public:
        ComboAttack0_1(Player* player) : State(player, "ComboAttack0_1") {}
        ~ComboAttack0_1() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        // ----- ��s���͗p -----
        float nextInputStartFrame_ = 0.0f;
        float nextInputEndFrame_   = 0.0f;
    };

    // ----- �R���{0_2 -----
    class ComboAttack0_2 : public State<Player>
    {
    public:
        ComboAttack0_2(Player* player) : State(player, "ComboAttack0_2") {}
        ~ComboAttack0_2() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        // ----- ��s���͗p -----
        float nextInputStartFrame_ = 0.0f;
        float nextInputEndFrame_ = 0.0f;
    };

    // ----- �R���{0_3 -----
    class ComboAttack0_3 : public State<Player>
    {
    public:
        ComboAttack0_3(Player* player) : State(player, "ComboAttack0_3") {}
        ~ComboAttack0_3() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

    // ----- �R���{1_0 -----
    class ComboAttack1_0 : public State<Player>
    {
    public:
        ComboAttack1_0(Player* player) : State(player, "ComboAttack1_0") {}
        ~ComboAttack1_0() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

    // ----- �R���{1_1 -----
    class ComboAttack1_1 : public State<Player>
    {
    public:
        ComboAttack1_1(Player* player) : State(player, "ComboAttack1_1") {}
        ~ComboAttack1_1() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

    // ----- �R���{1_2 -----
    class ComboAttack1_2 : public State<Player>
    {
    public:
        ComboAttack1_2(Player* player) : State(player, "ComboAttack1_2") {}
        ~ComboAttack1_2() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };
}