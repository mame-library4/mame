#pragma once
#include "StateMachine/State.h"
#include "Player.h"

namespace PlayerState
{
    struct AddForceData
    {
    public:
        void Initialize(const float& addForceFrame, const float& force, const float& decelerationForce = 2.0f);
        bool IsAbleAddForce(const float& animationFrame);

        [[nodiscard]] const float GetForce() const { return force_; }
        [[nodiscard]] const float GetDecelerationForce() const { return decelerationForce_; }

    private:
        float   addForceFrame_      = 0.0f;
        float   force_              = 0.0f;
        float   decelerationForce_  = 0.0f;
        bool    isAddforce_         = false;
    };
}

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
        void SetAnimation();
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

    private:
        void SetAnimation();
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
        enum class Direction
        {
            Fornt,
            Back,
            Right,
            Left,
        };

        void SetAnimation();        // �A�j���[�V�����ݒ�
        void CalcMoveDirection();   // �ړ������Z�o

    private:
        DirectX::XMFLOAT3   moveDirection_  = {};
        Direction           direction_      = Direction::Back;
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
        enum Direction
        {
            Front,
            Back,
            Max,
        };

        void AddForceFront(const float& elapsedTime);
        void AddForceBack(const float& elapsedTime);

    private:
        float power_[Direction::Max]         = {};
        float addForceFrame_[Direction::Max] = {};
        bool  isAddForce_[Direction::Max]    = {};

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
        float nextAttackFrame_      = 0.0f;

        AddForceData addForceData_;
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
        AddForceData addForceData_;

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
        void SetAnimation();

    private:
        // ----- ��s���͗p -----
        float nextInputStartFrame_  = 0.0f;
        float nextInputEndFrame_    = 0.0f;
        float nextAttackFrame_      = 0.0f;

        // ----- �ړ� -----
        DirectX::XMFLOAT3   moveDirecion_ = {};
        float               addForceFrame_ = 0.0f;
        float               power_ = 0.0f;
        bool                isAddForce_ = false;
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

    private:
        void SetAnimation();

    private:
        // ----- �ړ� -----
        DirectX::XMFLOAT3   moveDirecion_ = {};
        float               addForceFrame_[2] = {};
        float               power_[2] = {};
        bool                isAddForce_[2];
    };
}