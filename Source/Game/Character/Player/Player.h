#pragma once
#include <memory>
#include "../Character.h"
#include "../../../AI/StateMachine/StateMachine.h"
#include "../../../Input/Input.h"
#include "../Graphics/Graphics.h"

class Player : public Character
{
public:// --- �萔 ---
#pragma region �萔 
    enum class STATE
    {
        Idle,               // �ҋ@
        Walk,               // ����
        Run,                // ����

        Avoidance,          // ���
        Counter,            // �J�E���^�[
        CounterAttack,      // �J�E���^�[�U�� 

        LightAttack0,       // ��U��0
        LightAttack1,       // ��U��1
        LightAttack2,       // ��U��2

        StrongAttack0,      // ���U��0
        StrongAttack1,      // ���U��1

        Damage,

        Move,
    };

    enum class Animation
    {
        Idle,       // �ҋ@
        Run,        // ����   
        LightAttack0,
        LightAttack1,
        LightAttack2,
        Avoidance,  // ���

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
        None,           // ��s���͂Ȃ�
        LightAttack,    // ��U��
        StrongAttack,   // ���U��
    };

#pragma endregion �萔

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
    void ResetFlags(); // �t���O�����Z�b�g����


    void PlayAnimation(const Animation& index, const bool& loop, const float& speed = 1.0f)
    {
        Object::PlayAnimation(static_cast<int>(index), loop, speed);
    }

    // ----- ������΂����� -----
    void UpdateForce(const float& elapsedTime);
    void AddForce(const DirectX::XMFLOAT3& direction, const float& power);

public:// --- �擾�E�ݒ� ---
#pragma region [Get, Set] Function
    // ---------- �X�e�[�g�}�V�� --------------------
    StateMachine<State<Player>>* GetStateMachine() { return stateMachine_.get(); }
    void ChangeState(const STATE& state) { stateMachine_.get()->ChangeState(static_cast<int>(state)); }

    // ---------- �s�� -------------------------------------------------------
    [[nodiscard]] const int GetNextInput() const { return nextInput_; }
    void SetNextInput(const NextInput& nextInput) { nextInput_ = static_cast<int>(nextInput); }
    [[nodiscard]] const bool GetIsAvoidance() const { return isAvoidance_; }
    void SetIsAvoidance(const bool& isAvoidance) { isAvoidance_ = isAvoidance; }

    // ---------- �L�[���� ----------
    [[nodiscard]] bool GetLightAttackKeyUp() { return Input::Instance().GetGamePad().GetButtonUp() & GamePad::BTN_X; }
    [[nodiscard]] bool GetLightAttackKeyDown() { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_X; }
    [[nodiscard]] bool GetStrongAttackKeyUp() { return Input::Instance().GetGamePad().GetButtonUp() & GamePad::BTN_Y; }
    [[nodiscard]] bool GetStrongAttackKeyDown() { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y; }

#pragma endregion [Get, Set] Function

private:
    // ---------- �X�e�[�g�}�V�� --------------------
    std::unique_ptr<StateMachine<State<Player>>> stateMachine_;

    // ---------- �s�� ------------------------------
    int nextInput_ = false; // ��s����
    bool isAvoidance_ = false; // ���

    int animationIndex_ = 0;
    float speed_ = 1.0f;

    // ---------- ������� --------------------
    DirectX::XMFLOAT3 blowDirection_ = {};
    float blowPower_ = 0.0f;

    // ---------- Debug�p --------------------
    bool isCollisionSphere_ = true;
    bool isDamageSphere_ = true;
    bool isAttackSphere_ = true;

    DirectX::XMFLOAT3 offset_ = {};
};
