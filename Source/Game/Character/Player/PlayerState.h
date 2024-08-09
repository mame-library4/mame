#pragma once
#include "StateMachine/State.h"
#include "Player.h"

namespace PlayerState
{
    struct AddForceData
    {
    public:
        void Initialize(const float& addForceFrame, const float& force, const float& decelerationForce = 2.0f);
        bool Update(const float& animationFrame);

        [[nodiscard]] const float GetForce() const { return force_; }
        [[nodiscard]] const float GetDecelerationForce() const { return decelerationForce_; }

    private:
        float   addForceFrame_      = 0.0f;
        float   force_              = 0.0f;
        float   decelerationForce_  = 0.0f;
        bool    isAddforce_         = false;
    };

    struct AttackData
    {
    public:
        void Initialize(const float& startFrame, const float& endFrame);
        bool Update(const float& animationFrame, const bool& flag);

    private:
        float attackStartFrame_ = 0.0f;     // 攻撃判定有効スタートフレーム
        float attackEndFrame_   = 0.0f;     // 攻撃判定有効エンドフレーム
        bool  isAttacked_       = false;    // 攻撃したか
        bool  isFirstTime_      = false;    // 攻撃判定有効フレームに入った一回目のフレームかどうか
    };

    struct GamePadVibration
    {
    public:
        void Initialize(const float& startFrame, const float& time, const float& power);
        void Update(const float& animationFrame);

    private:
        float startFrame_ = 0.0f;
        float time_ = 0.0f;
        float power_ = 0.0f;
        bool  isVibraion_ = false;
    };
}

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
        void SetAnimation();
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
        void SetAnimation();
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

    private:
        void SetAnimation();

    private:
        float stateChangeTimer_ = 0.0f;
    };

    class FlinchState : public State<Player>
    {
    public:
        FlinchState(Player* player) : State(player, "FlinchState") {}
        ~FlinchState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        int state_ = 0;
    };

    // ----- ダメージ -----
    class DamageState : public State<Player>
    {
    public:
        DamageState(Player* player) : State(player, "DamageState") {}
        ~DamageState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        void SetAnimationSpeed();
        void Turn();

    private:
        AddForceData addForceData_;

        bool isFirstAnimation_ = false; // 最初のアニメーションか

        DirectX::XMFLOAT3 addForceDirection_ = {}; // 吹っ飛び方向
    };

    // ----- 死亡 -----
    class DeathState : public State<Player>
    {
    public:
        DeathState(Player* player) : State(player, "DeathState") {}
        ~DeathState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;
    };

    // ----- 回避 -----
    class AvoidanceState : public State<Player>
    {
    public:
        AvoidanceState(Player* player) : State(player, "AvoidanceState") {}
        ~AvoidanceState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        void Turn(const float& elapsedTime);

        void ResetState(); // このステートをリセット(初期化)する

        [[nodiscard]] const bool CheckNextInput();
        void SetAnimationSpeed();

        void SetAnimation();        // アニメーション設定
        void CalcMoveDirection();   // 移動方向算出

    private:
        DirectX::XMFLOAT3   moveDirection_  = {};

        AddForceData addForceData_;

        DirectX::XMFLOAT2 inputDirection_ = {};
        bool isFirstTime_   = true;     // このステートに入るのが初めてかどうか
        bool isRotating_    = false;    // 回転処理をするか
        bool isInputStick_  = false;    // スティック入力があるか
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
        void SetAnimationSpeed();

    private:
        AddForceData addForceFront_;
        AddForceData addForceBack_;

        GamePadVibration gamePadVibration_;

        bool isNextInput_ = false; // カウンター攻撃の先行入力用
    };

    // ----- カウンター攻撃 -----
    class CounterComboState : public State<Player>
    {
    public:
        CounterComboState(Player* player) : State(player, "CounterComboState") {}
        ~CounterComboState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        AddForceData        addForceData_;
        AttackData          attackData_;
    };

    class RunAttackState : public State<Player>
    {
    public:
        RunAttackState(Player* player) : State(player, "RunAttackState") {}
        ~RunAttackState() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        [[nodiscard]] const bool CheckNextInput();

    private:
        AddForceData    addForceData_;
        AttackData      attackData_;
    };

    // ----- コンボ0_0 -----
    class ComboAttack0_0 : public State<Player>
    {
    public:
        ComboAttack0_0(Player* player) : State(player, "ComboAttack0_0") {}
        ~ComboAttack0_0() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        void SetAnimation();

        void SetAnimationSpeed();
        [[nodiscard]] const bool CheckNextInput();

    private:
        // ----- 先行入力用 -----
        float nextInputStartFrame_  = 0.0f;
        float nextInputEndFrame_    = 0.0f;
        float nextAttackFrame_      = 0.0f;

        AddForceData    addForceData_;
        AttackData      attackData_;
    };

    // ----- コンボ0_1 -----
    class ComboAttack0_1 : public State<Player>
    {
    public:
        ComboAttack0_1(Player* player) : State(player, "ComboAttack0_1") {}
        ~ComboAttack0_1() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        void SetAnimation();
        void SetAnimationSpeed();
        [[nodiscard]] const bool CheckNextInput();

    private:
        AddForceData    addForceData_;
        AttackData      attackData_;
    };

    // ----- コンボ0_2 -----
    class ComboAttack0_2 : public State<Player>
    {
    public:
        ComboAttack0_2(Player* player) : State(player, "ComboAttack0_2") {}
        ~ComboAttack0_2() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        void SetAnimationSpeed();
        [[nodiscard]] const bool CheckNextInput();

    private:
        AddForceData    addForceData_;
        AttackData      attackData_;
    };

    // ----- コンボ0_3 -----
    class ComboAttack0_3 : public State<Player>
    {
    public:
        ComboAttack0_3(Player* player) : State(player, "ComboAttack0_3") {}
        ~ComboAttack0_3() {}

        void Initialize()                       override;
        void Update(const float& elapsedTime)   override;
        void Finalize()                         override;

    private:
        void SetAnimationSpeed();

    private:
        AddForceData    addForceData_;
        AttackData      attackData_;

        bool isVibration_ = false;
    };
}