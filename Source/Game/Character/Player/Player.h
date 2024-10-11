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
#pragma region ----- �萔 -----
    enum class STATE
    {
        Idle,           // �ҋ@
        Run,
        LightFlinch,
        Flinch,
        Damage,         // �_���[�W
        Death,          // ���S
        Dodge,      // ���

        Skill,

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

        Skill0,
        Skill1,
    };

    // ��s���͂̎��
    enum class NextInput
    {
        None,           // ��s���͂Ȃ�
        ComboAttack0,   // �R���{�U��
        Dodge,      // ���
        Counter,        // �J�E���^�[
    };

#pragma endregion ----- �萔 -----

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

    void CalculateRotationAdjustment(); // ��]�␳�ʐݒ�
    void UpdateRotationAdjustment(const float& elapsedTime);    // ��]�␳

    // ---------- �X�^�~�i ----------
    void UpdateStaminaRecovery(const float& elapsedTime);

    // ---------- Collision ----------
    void UpdateCollisions(const float& elapsedTime) override;
    void UpdateCollisionDetectionData();    // �����o������ʒu�X�V

    // ---------- ���̍��W�X�V ----------
    void UpdateSwordTransform();

public:// --- �擾�E�ݒ� ---
#pragma region [Get, Set] Function
    // ---------- �X�e�[�g�}�V�� --------------------
    StateMachine<State<Player>>* GetStateMachine() { return stateMachine_.get(); }
    void ChangeState(const STATE& state);
    [[nodiscard]] const STATE GetCurrentState() const { return currentState_; }
    [[nodiscard]] const STATE GetOldState() const { return oldState_; }

    // ---------- �ړ� ------------------------------
    void SetMoveDirection(const DirectX::XMFLOAT3 direction) { moveDirection_ = direction; } // �ړ�����

    [[nodiscard]] const float GetDashSpeed() const { return dashSpeed_; }
    [[nodiscard]] const float GetDashAnimationSpeed() const { return dashAnimationSpeed_; }
    [[nodiscard]] const bool GetIsDash() const { return isDash_; }
    void SetIsDash(const bool& flag) { isDash_ = flag; }

    // ---------- �X�^�~�i ----------
    [[nodiscard]] const float GetStamina() const { return stamina_; }
    [[nodiscard]] const float GetMaxStamina() const { return maxStamina_; }
    void SetStanima(const float& stamina) { stamina_ = stamina; }
    
    [[nodiscard]] const float GetDodgeStaminaCost() const { return dodgeStaminaCost_; }
    [[nodiscard]] const float GetDashStamiaCost() const { return dashStaminaCost_; }
    void UseDodgeStamina() { stamina_ -= dodgeStaminaCost_; }
    void UseDashStamina(const float& elapsedTime);
    
    [[nodiscard]] const float GetStaminaRecoverySpeed() const { return staminaRecoverySpeed_; }
    void SetStaminaRecoverySpeed(const float& speed) { staminaRecoverySpeed_ = speed; }

    [[nodiscard]] const bool GetIsStaminaDepleted() const { return isStaminaDepleted; }

    // ---------- �s�� -------------------------------------------------------
    // ----- �t���O�����Z�b�g -----
    void ResetFlags();    
    // ----- ��s���� -----
    void SetNextInputStartFrame(const float& dodge = 0.0f, const float& attack = 0.0f, const float& counter = 0.0f, const float& move = 0.0f);
    void SetNextInputEndFrame(const float& dodge = 10.0f, const float& attack = 10.0f, const float& counter = 10.0f);
    void SetNextInputTransitionFrame(const float& dodge = 0.0f, const float& attack = 0.0f, const float& counter = 0.0f);



    [[nodiscard]] const NextInput GetNextInput() const { return nextInput_; }
    void SetNextInput(const NextInput& nextInput) { nextInput_ = nextInput; }
    // ----- ��� -----
    [[nodiscard]] const bool GetIsDodge() const { return isDodge_; }
    void SetIsDodge(const bool& flag) { isDodge_ = flag; }
    // ----- �J�E���^�[��Ԃ� -----
    [[nodiscard]] const bool GetIsCounter() const { return isCounter_; }
    void SetIsCounter(const bool& flag) { isCounter_ = flag; }
    // ----- �J�E���^�[(�ǉ���)�U�����\�� -----
    [[nodiscard]] const bool GetIsAbleCounterAttack() const { return isAbleCounterAttack_; }
    void SetIsAbleCounterAttack(const bool& flag) { isAbleCounterAttack_ = flag; }
    
    // ----- �U������ -----
    [[nodiscard]] const bool GetIsAttackValid() const { return isAttackValid_; }
    void SetIsAttackValid(const bool& flag) { isAttackValid_ = flag; }

    [[nodiscard]] const bool GetIsAbleAttack() const { return isAbleAttack_; }
    void SetIsAbleAttack(const bool& flag) { isAbleAttack_ = flag; }

    // ----- ���G���� -----
    [[nodiscard]] const bool GetIsInvincible() const { return isInvincible_; }
    void SetIsInvincible(const bool& flag) { isInvincible_ = flag; }

    // ----- �J�E���^�[�L���͈� -----
    [[nodiscard]] const float GetCounterActiveRadius() const { return counterActiveRadius_; }
    void SetCounterActiveRadius(const float& range) { counterActiveRadius_ = range; }

    // ---------- �L�[���� ----------
    [[nodiscard]] bool IsComboAttack0KeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_B; }
    [[nodiscard]] bool IsDodgeKeyDown()        const;
    [[nodiscard]] bool IsCounterStanceKey()    const;
    [[nodiscard]] bool IsGetUpKeyDown()        const;
    [[nodiscard]] bool IsDashKey()             const;

#pragma endregion [Get, Set] Function

    [[nodiscard]] const float GetMoveInputStartFrame()      const { return moveInputStartFrame_; }
    [[nodiscard]] const float GetDodgeInputStartFrame()     const { return dodgeInputStartFrame_; }
    [[nodiscard]] const float GetDodgeInputEndFrame()       const { return dodgeInputEndFrame_; }
    [[nodiscard]] const float GetDodgeTransitionFrame()     const { return dodgeTransitionFrame_; }
    [[nodiscard]] const float GetAttackInputStartFrame()    const { return attackInputStartFrame_; }
    [[nodiscard]] const float GetAttackInputEndFrame()      const { return attackInputEndFrame_; }
    [[nodiscard]] const float GetAttackTransitionFrame()    const { return attackTransitionFrame_; }
    [[nodiscard]] const float GetCounterInputStartFrame()   const { return counterInputStartFrame_; }
    [[nodiscard]] const float GetCounterInputEndFrame()     const { return counterInputEndFrame_; }
    [[nodiscard]] const float GetCounterTransitionFrame()   const { return counterTransitionFrame_; }

private:
    // ---------- Collision ----------
    void RegisterCollisionData();

private:
    // ---------- ���� ----------
    GltfModel weapon_;

    // ---------- �X�e�[�g�}�V�� --------------------
    std::unique_ptr<StateMachine<State<Player>>> stateMachine_;
    STATE currentState_ = STATE::Idle;
    STATE oldState_     = STATE::Idle;

    // ---------- �ړ� ----------
    DirectX::XMFLOAT3 moveDirection_        = {};
    float             dashSpeed_            = 8.0f;  // �_�b�V�����̑��x
    float             dashAnimationSpeed_   = 1.3f;  // �_�b�V���̃A�j���[�V�������x
    bool              isDash_               = false; // ���݃_�b�V�����Ă��邩

    // ---------- ��] ----------
    float startAngle_               = 0.0f;     // ��ԊJ�n�n�_
    float endAngle_                 = 0.0f;     // ��ԏI���n�_
    float rotationTimer_            = 0.0f;     // ��Ԑ���p
    float rotateAngleThreshold_     = 0.52f;    // ��]�p臒l
    float rotationAmount_           = 0.52f;    // ��]��
    float rotationSpeed_            = 3.0f;     // ��]���x
    bool  useRotationAdjustment_    = false;    // �X�V���������邩

    // ---------- �s�� ------------------------------
    NextInput   nextInput_                  = NextInput::None;  // ��s���͕ۑ��p
    float       dodgeInputStartFrame_       = 0.0f;             // ����s���͊J�n�t���[��
    float       attackInputStartFrame_      = 0.0f;             // �U����s���͊J�n�t���[��
    float       counterInputStartFrame_     = 0.0f;             // �J�E���^�[��s���͊J�n�t���[��
    float       moveInputStartFrame_        = 0.0f;
    float       dodgeInputEndFrame_         = 0.0f;
    float       attackInputEndFrame_        = 0.0f;
    float       counterInputEndFrame_       = 0.0f;
    float       dodgeTransitionFrame_       = 0.0f;             // ����֑J�ډ\�t���[��
    float       attackTransitionFrame_      = 0.0f;             // �U���֑J�ډ\�t���[��
    float       counterTransitionFrame_     = 0.0f;             // �J�E���^�[�֑J�ډ\�t���[��

    bool isDodge_           = false;            // ���
    bool isCounter_             = false;            // �J�E���^�[��Ԃ�
    bool isAbleCounterAttack_   = false;            // �J�E���^�[�U�����\��( �ǉ��� )
    
    bool isAttackValid_         = false;            // �U�����L����
    bool isAbleAttack_          = false;            // �U���\��

    // ---------- �J�E���^�[�L���͈� ----------
    float counterActiveRadius_   = 0.0f; // �J�E���^�[�L���͈�

    // ---------- ���G ----------
    bool isInvincible_ = false;

    // ---------- Debug�p --------------------
    bool isCollisionSphere_ = true;
    bool isDamageSphere_ = true;
    bool isAttackSphere_ = true;

    // ---------- �X�^�~�i ----------
    float stamina_      = 1.0f;
    float maxStamina_   = 1.0f;
    float staminaRecoverySpeed_ = 15.0f;

    float dodgeStaminaCost_ = 10.0f; // ����Ɏg���X�^�~�i��
    float dashStaminaCost_  = 3.0f; // �_�b�V���Ɏg���X�^�~�i��

    bool isStaminaDepleted = false;


    SwordTrail swordTrail_;


    // ---------- �� ----------
    DirectX::XMFLOAT3 socketLocation_ = { 700, 10500, -7000 };
    DirectX::XMFLOAT3 socketRotation_ = { 145.0f, -16.0f, 0.0f };
    DirectX::XMFLOAT3 socketScale_ = { 1.0f, 1.0f, -1.0f };
    DirectX::XMFLOAT4X4 weaponWorld_;
};
