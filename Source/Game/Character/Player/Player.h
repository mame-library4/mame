#pragma once
#include <memory>
#include "../Character.h"
#include "StateMachine/StateMachine.h"
#include "Input.h"
#include "Graphics.h"
#include "Effect/SwordTrail/SwordTrail.h"

class Player : public Character
{
public:// --- �萔 ---
#pragma region �萔 
    enum class STATE
    {
        Idle,               // �ҋ@

        Move,
        //Walk,               // ����
        //Run,                // ����

        Avoidance,          // ���
        Counter,            // �J�E���^�[
        CounterAttack,      // �J�E���^�[�U�� 

        LightAttack0,       // ��U��0
        LightAttack1,       // ��U��1
        LightAttack2,       // ��U��2

        StrongAttack0,      // ���U��0
        StrongAttack1,      // ���U��1

        Damage,
    };

    enum class Animation
    {
        Idle,       // �ҋ@
        Walk,
        Run,        // ����   
        
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

    void Turn(const float& elapsedTime);
    void Move(const float& elapsedTime);

    bool CheckAttackButton(const Player::NextInput& nextInput);

    void ResetFlags(); // �t���O�����Z�b�g����


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

    // �U������L���t���O�ݒ�
    void SetAttackFlag(const bool& activeFlag = true);
    bool GetIsActiveAttackFlag();

public:// --- �擾�E�ݒ� ---
#pragma region [Get, Set] Function
    // ---------- �X�e�[�g�}�V�� --------------------
    StateMachine<State<Player>>* GetStateMachine() { return stateMachine_.get(); }
    void ChangeState(const STATE& state) { stateMachine_.get()->ChangeState(static_cast<int>(state)); }

    // ---------- �s�� -------------------------------------------------------
    [[nodiscard]] const int GetNextInput() const { return nextInput_; }
    [[nodiscard]] const bool GetIsAvoidance() const { return isAvoidance_; }
    void SetIsAvoidance(const bool& isAvoidance) { isAvoidance_ = isAvoidance; }

    // ---------- �L�[���� ----------
    [[nodiscard]] bool GetLightAttackKeyUp() { return Input::Instance().GetGamePad().GetButtonUp() & GamePad::BTN_X; }
    [[nodiscard]] bool GetLightAttackKeyDown() { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_X; }
    [[nodiscard]] bool GetStrongAttackKeyUp() { return Input::Instance().GetGamePad().GetButtonUp() & GamePad::BTN_Y; }
    [[nodiscard]] bool GetStrongAttackKeyDown() { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y; }

#pragma endregion [Get, Set] Function

private:
    // ---------- �� ----------
    GltfModel swordModel_;

    // ---------- �X�e�[�g�}�V�� --------------------
    std::unique_ptr<StateMachine<State<Player>>> stateMachine_;

    // ----- �ړ� -----
    DirectX::XMFLOAT3 moveDirection_ = {};

    // ---------- �s�� ------------------------------
    int nextInput_ = false; // ��s����
    bool isAvoidance_ = false; // ���

    // ---------- Debug�p --------------------
    bool isCollisionSphere_ = true;
    bool isDamageSphere_ = true;
    bool isAttackSphere_ = true;

    DirectX::XMFLOAT3 offset_ = {};

    SwordTrail swordTrail_;
};
