#pragma once
#include "../../../AI/StateMachine/State.h"
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
        int isInputMove();  // ���͂ɉ����Đ؂�ւ���X�e�[�g�𔻒f����
    };

    // ---- �ړ� -----
#pragma region �ړ�

    // ----- ���� -----
    class WalkState : public State<Player>
    {
    public:
        WalkState(Player* player) : State(player, "WalkState") {}
        ~WalkState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        int isInputMove();  // ���͒l�ɉ����Ēl��Ԃ�
    };

    // ----- ���� -----
    class RunState : public State<Player>
    {
    public:
        RunState(Player* player) : State(player, "RunState") {}
        ~RunState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

#pragma endregion �ړ�

    // ---- ��� -----
#pragma region
    // ----- ��� -----
    class AvoidanceState : public State<Player>
    {
    public:
        AvoidanceState(Player* player) : State(player, "AvoidanceState") {}
        ~AvoidanceState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
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
    };

    // ----- �J�E���^�[�U�� -----
    class CounterAttackState : public State<Player>
    {
    public:
        CounterAttackState(Player* player) : State(player, "Counter") {}
        ~CounterAttackState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

#pragma endregion

    // ----- ��U�� -----
#pragma region ��U��

    // ----- ��U���O -----
    class LightAttack0State : public State<Player>
    {
    public:
        LightAttack0State(Player* player) : State(player, "LightAttack0State") {}
        ~LightAttack0State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

    // ----- ��U���P -----
    class LightAttack1State : public State<Player>
    {
    public:
        LightAttack1State(Player* player) : State(player, "LightAttack1State") {}
        ~LightAttack1State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

    // ----- ��U���Q -----
    class LightAttack2State : public State<Player>
    {
    public:
        LightAttack2State(Player* player) : State(player, "LightAttack2State") {}
        ~LightAttack2State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

#pragma endregion ��U��

    // ----- ���U�� -----
#pragma region ���U��

    // ----- ���U���O -----
    class StrongAttack0State : public State<Player>
    {
    public:
        StrongAttack0State(Player* player) : State(player, "StrongAttack0State") {}
        ~StrongAttack0State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

    // ----- ���U���P -----
    class StrongAttack1State : public State<Player>
    {
    public:
        StrongAttack1State(Player* player) : State(player, "StrongAttack1State") {}
        ~StrongAttack1State() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

#pragma endregion ���U��

    // ----- �_���[�W�H�炢 -----
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