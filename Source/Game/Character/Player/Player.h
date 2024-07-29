#pragma once
#include <memory>
#include "../Character.h"
#include "StateMachine/StateMachine.h"
#include "Input.h"
#include "Graphics.h"
#include "Effect/SwordTrail/SwordTrail.h"

#include "Effect/Effect.h"

class Player : public Character
{
public:// --- �萔 ---
#pragma region �萔 
    enum class STATE
    {
        Idle,           // �ҋ@
        //Move,           // �ړ�
        Walk,
        Run,
        Flinch,
        Damage,         // �_���[�W
        Death,          // ���S
        Avoidance,      // ���
        Counter,        // �J�E���^�[
        CounterCombo,   // �J�E���^�[�R���{

        RunAttack,

        ComboAttack0_0,   // �R���{0
        ComboAttack0_1,   // �R���{0
        ComboAttack0_2,   // �R���{0
        ComboAttack0_3,   // �R���{0
        
        ComboAttack1_0,   // �R���{1
        ComboAttack1_1,   // �R���{1
        ComboAttack1_2,   // �R���{1

        
    };

    enum class Animation
    {
        // �ҋ@
        Idle,

        // ����
        WalkStart,
        Walk,
        WalkEnd,

        // ����
        RunStart,
        Run,
        RunEnd,

        // ���
        RollForward,
        RollBack,
        RollRight,
        RollLeft,

        // �U��
        ComboAttack0_0,
        ComboAttack0_1,
        ComboAttack0_2,
        ComboAttack0_3,

        // �h��
        BlockStart,
        BlockLoop,
        BlockEnd,
        
        // �J�E���^�[�U��
        ParryCounterAttack0, // Right
        ParryCounterAttack1, // Left

        HitLarge,
        GetUp,
        HitForward,
        HitBack,
        HitRight,
        HitLeft,

        KnockDownStart,
        KnockDownLoop,
        KnockDownEnd,
        KnockDownDeath,

        ComboAttack1_0,
        ComboAttack1_1,
        ComboAttack1_2,
        ComboAttack1_3,

        RunAttack0,
        RunAttack1,

        Counter,
    };

    enum class NextInput
    {
        None,           // ��s���͂Ȃ�
        ComboAttack0,   // �R���{�U��0
        ComboAttack1,   // �R���{�U��1
        Avoidance,      // ���
    };

#pragma endregion �萔

public:
    Player();
    ~Player() override;

    void Initialize();                                  // ������
    void Finalize();                                    // �I����
    void Update(const float& elapsedTime)    override;  // �X�V
    void Render(ID3D11PixelShader* psShader) override;  // �`��
    void DrawDebug()                         override;  // ImGui�p
    
    void RenderTrail();                                 // ���̋O�Օ`��
    void DebugRender(DebugRenderer* debugRenderer);     // ����p�}�`�`��

    // ---------- Animation ----------
    void PlayAnimation(const Animation& index, const bool& loop, const float& speed = 1.0f) { Object::PlayAnimation(static_cast<int>(index), loop, speed); }
    void PlayBlendAnimation(const Animation& index, const bool& loop, const float& speed = 1.0f, const float& blendAnimationFrame = 0.0f) { Object::PlayBlendAnimation(static_cast<int>(index), loop, speed, blendAnimationFrame); }
    void SetAnimationSpeed(const float& speed) { Object::SetAnimationSpeed(speed); }

    // ---------- �ړ�����] ----------
    void Turn(const float& elapsedTime); // ���񏈗�
    void Move(const float& elapsedTime); // �ړ�����

    // ---------- Collision ----------
    void UpdateCollisions(const float& elapsedTime) override;
    void UpdateCollisionDetectionData();    // �����o������ʒu�X�V

    // ---------- ���̍��W�X�V ----------
    void UpdateSwordTransform();

    bool Player::CheckNextInput(const Player::NextInput& nextInput);

public:// --- �擾�E�ݒ� ---
#pragma region [Get, Set] Function
    // ---------- �X�e�[�g�}�V�� --------------------
    StateMachine<State<Player>>* GetStateMachine() { return stateMachine_.get(); }
    void ChangeState(const STATE& state);
    [[nodiscard]] const STATE GetCurrentState() const { return currentState_; }

    // ---------- �ړ� ------------------------------
    void SetMoveDirection(const DirectX::XMFLOAT3 direction) { moveDirection_ = direction; } // �ړ�����

    // ---------- �s�� -------------------------------------------------------
    // ----- �t���O�����Z�b�g -----
    void ResetFlags();    
    // ----- ��s���� -----
    [[nodiscard]] const NextInput GetNextInput() const { return nextInput_; }
    void SetNextInput(const NextInput& nextInput) { nextInput_ = nextInput; }
    // ----- ��� -----
    [[nodiscard]] const bool GetIsAvoidance() const { return isAvoidance_; }
    void SetIsAvoidance(const bool& isAvoidance) { isAvoidance_ = isAvoidance; }
    // ----- �J�E���^�[��Ԃ� -----
    [[nodiscard]] const bool GetIsCounter() const { return isCounter_; }
    void SetIsCounter(const bool& flag) { isCounter_ = flag; }
    // ----- �J�E���^�[(�ǉ���)�U�����\�� -----
    [[nodiscard]] const bool GetIsAbleCounterAttack() const { return isAbleCounterAttack_; }
    void SetIsAbleCounterAttack(const bool& flag) { isAbleCounterAttack_ = flag; }
    // ----- �U������ -----
    [[nodiscard]] const bool GetIsAbleAttack() const { return isAbleAttack_; }
    void SetIsAbleAttack(const bool& flag) { isAbleAttack_ = flag; }

    // ----- �J�E���^�[�L���͈� -----
    [[nodiscard]] const float GetCounterActiveRadius() const { return counterActiveRadius_; }
    void SetCounterActiveRadius(const float& range) { counterActiveRadius_ = range; }

    // ---------- �L�[���� ----------
    [[nodiscard]] bool GetComboAttack0KeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_B; }
    [[nodiscard]] bool GetComboAttack1KeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y; }
    [[nodiscard]] bool GetAvoidanceKeyDown()    const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_A; }
    [[nodiscard]] bool GetCounterStanceKey()    const { return Input::Instance().GetGamePad().GetButton() & GamePad::BTN_B && Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_TRIGGER; }

#pragma endregion [Get, Set] Function

private:
    // ---------- Collision ----------
    void RegisterCollisionData();

private:
    // ---------- ���� ----------
    GltfModel weapon_;

    // ---------- �X�e�[�g�}�V�� --------------------
    std::unique_ptr<StateMachine<State<Player>>> stateMachine_;
    STATE currentState_ = STATE::Idle;

    // ---------- �ړ� ----------
    DirectX::XMFLOAT3 moveDirection_ = {};

    // ---------- �s�� ------------------------------
    NextInput nextInput_        = NextInput::None;  // ��s����
    bool isAvoidance_           = false;            // ���
    bool isCounter_             = false;            // �J�E���^�[��Ԃ�
    bool isAbleCounterAttack_   = false;            // �J�E���^�[�U�����\��( �ǉ��� )
    bool isAbleAttack_          = false;            // �U���\��

    // ---------- �J�E���^�[�L���͈� ----------
    float counterActiveRadius_   = 0.0f; // �J�E���^�[�L���͈�

    // ---------- Debug�p --------------------
    bool isCollisionSphere_ = true;
    bool isDamageSphere_ = true;
    bool isAttackSphere_ = true;


    SwordTrail swordTrail_;

#if 1
    DirectX::XMFLOAT3 socketLocation_ = { 700, 10500, -7000 };
    DirectX::XMFLOAT3 socketRotation_ = { 145.0f, -16.0f, 0.0f };
    DirectX::XMFLOAT3 socketScale_ = { 1.0f, 1.0f, -1.0f };
#else   
    DirectX::XMFLOAT3 socketLocation_ = { 770, 12000, 2500 };
    DirectX::XMFLOAT3 socketRotation_ = { -100.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 socketScale_ = { 1.0f, 1.0f, 1.0f };
#endif  
    DirectX::XMFLOAT4X4 weaponWorld_;

};
