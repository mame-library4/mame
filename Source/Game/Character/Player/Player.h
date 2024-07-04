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

    void Initialize();
    void Finalize();
    void Update(const float& elapsedTime)   override;
    void Render(ID3D11PixelShader* psShader)override;
    void RenderTrail();
    void DrawDebug()                        override;  
    void DebugRender(DebugRenderer* debugRenderer);

    void Turn(const float& elapsedTime);
    void Move(const float& elapsedTime);

    bool CheckNextInput(const Player::NextInput& nextInput, const float& nextAttackFrame = -1);

    void ResetFlags(); // �t���O�����Z�b�g����


    // ���̍��W�X�V
    void UpdateSwordTransform();

    void PlayAnimation(const Animation& index, const bool& loop, const float& speed = 1.0f)
    {
        Object::PlayAnimation(static_cast<int>(index), loop, speed);
        //swordModel_.PlayAnimation(static_cast<int>(index), loop, speed);
    }
    void SetAnimationSpeed(const float& speed)
    {
        Object::SetAnimationSpeed(speed);
        //swordModel_.SetAnimationSpeed(speed);
    }
    float slowAnimationSpeed_ = 0.15f;
    //float slowAnimationSpeed_ = 0.25f;

    
    void PlayBlendAnimation(const Animation& index, const bool& loop, const float& speed = 1.0f, const float& blendAnimationFrame = 0.0f) { Object::PlayBlendAnimation(static_cast<int>(index), loop, speed, blendAnimationFrame); }

    void UpdateCollisions(const float& elapsedTime) override;


    void SetMoveDirection(const DirectX::XMFLOAT3 direction) { moveDirection_ = direction; }

    // ---------- Animation ----------
    [[nodiscard]] const bool GetUseBlendAnimation() const { return useBlendAnimation_; }
    void SetUseBlendAnimation(const bool& flag) { useBlendAnimation_ = flag; }

    // �U������L���t���O�ݒ�
    void SetAttackFlag(const bool& activeFlag = true);
    bool GetIsActiveAttackFlag();

public:// --- �擾�E�ݒ� ---
#pragma region [Get, Set] Function
    // ---------- �X�e�[�g�}�V�� --------------------
    StateMachine<State<Player>>* GetStateMachine() { return stateMachine_.get(); }
    void ChangeState(const STATE& state);

    // ---------- �s�� -------------------------------------------------------
    [[nodiscard]] const NextInput GetNextInput() const { return nextInput_; }
    void SetNextInput(const NextInput& nextInput) { nextInput_ = nextInput; }

    [[nodiscard]] const bool GetIsAvoidance() const { return isAvoidance_; }
    void SetIsAvoidance(const bool& isAvoidance) { isAvoidance_ = isAvoidance; }

    // ---------- �L�[���� ----------
    [[nodiscard]] bool GetComboAttack0KeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_B; }
    [[nodiscard]] bool GetComboAttack1KeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y; }
    [[nodiscard]] bool GetAvoidanceKeyDown()    const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_A; }
    [[nodiscard]] bool GetCounterStanceKey()    const { return Input::Instance().GetGamePad().GetButton() & GamePad::BTN_B && Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_TRIGGER; }

#pragma endregion [Get, Set] Function

private:
    // ---------- ���� ----------
    GltfModel weapon_;

    // ---------- �X�e�[�g�}�V�� --------------------
    std::unique_ptr<StateMachine<State<Player>>> stateMachine_;

    // ----- �ړ� -----
    DirectX::XMFLOAT3 moveDirection_ = {};

    // ---------- �s�� ------------------------------
    NextInput nextInput_ = NextInput::None; // ��s����
    bool isAvoidance_ = false; // ���

    // ---------- Debug�p --------------------
    bool isCollisionSphere_ = true;
    bool isDamageSphere_ = true;
    bool isAttackSphere_ = true;

    // ---------- Animation ----------
    bool useBlendAnimation_ = false;

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
