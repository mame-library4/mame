#pragma once
#include <memory>
#include "../Character.h"
#include "StateMachine/StateMachine.h"
#include "Input.h"
#include "Graphics.h"
#include "Effect/SwordTrail/SwordTrail.h"

#include "Effect/Effect.h"

#include "Sword/Sword.h"

class Player : public Character
{
public:// --- �萔 ---
#pragma region ----- �萔 -----
    enum class STATE
    {
        Idle,           // �ҋ@
        Run,

        GuardCounter,       // �K�[�h�J�E���^�[
        GuardCounterAttack, // �K�[�h�J�E���^�[�U��

        LightFlinch,
        Flinch,
        Damage,         // �_���[�W
        Death,          // ���S
        Dodge,          // ���

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
        Idle,           // �ҋ@
        Walk,           // ����
        Run,            // ����
        RollFront,      // ���O
        RollBack,       // ������
        RollRight,      // ����E
        RollLeft,       // �����
        Attack0_0,      // �U��0_0
        Attack0_1,      // �U��0_1
        Attack0_2,      // �U��0_2
        Attack0_3,      // �U��0_3
        BlockStart,     // �h��J�n
        BlockLoop,      // �h�䃋�[�v
        BlockEnd,       // �h��I���        
        CounterAttack0, // �J�E���^�[�U��0
        CounterAttack1, // �J�E���^�[�U��0
        Damage,         // �_���[�W
        GetUp,          // �N���オ��
        HitFront,       // ��_���[�W�O
        HitBack,        // ��_���[�W���
        HitRight,       // ��_���[�W�E
        HitLeft,        // ��_���[�W��
        DownStart,      // �_�E���J�n
        DownLoop,       // �_�E�����[�v
        DownEnd,        // �_�E���I���
        DownDeath,      // �_�E�����S
        Attack1_0,      // �U��1_0
        Attack1_1,      // �U��1_1
        Attack1_2,      // �U��1_2
        Attack1_3,      // �U��1_3
        RunAttack0,     // ����U��0
        RunAttack1,     // ����U��1
        Counter,        // �J�E���^�[
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


    // ----- �U���L�����Z�� -----
    [[nodiscard]] const bool GetIsMoveAttackCancel() const { return isMoveAttackCancel_; }
    void SetIsMoveAttackCancel(const bool& flag) { isMoveAttackCancel_ = flag; }
    [[nodiscard]] const bool GetIsDodgeAttackCancel() const { return isDodgeAttackCancel_; }
    void SetIsDodgeAttackCancel(const bool& flag) { isDodgeAttackCancel_ = flag; }

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

    [[nodiscard]] const bool GetIsAttackHit() const { return isAttackHit_; }
    void SetIsAttackHit(const bool& flag) { isAttackHit_ = flag; }

    // ----- ���G���� -----
    [[nodiscard]] const bool GetIsInvincible() const { return isInvincible_; }
    void SetIsInvincible(const bool& flag) { isInvincible_ = flag; }

    // ----- �J�E���^�[�L���͈� -----
    [[nodiscard]] const float GetCounterActiveRadius() const { return counterActiveRadius_; }
    void SetCounterActiveRadius(const float& range) { counterActiveRadius_ = range; }

    // ---------- �K�[�h�J�E���^�[�@----------
    [[nodiscard]] const float GetGuardCounterRadius() const { return guardCounterRadius_; }
    void SetGuardCounterRadius(const float& radius) { guardCounterRadius_ = radius; }
    [[nodiscard]] const bool GetIsGuardCounterStance() const { return isGuardCounterStance_; }
    void SetIsGuardCounterStance(const bool& flag) { isGuardCounterStance_ = flag; }
    [[nodiscard]] const bool GetIsGuardCounterSuccessful() const { return isGuardCounterSuccessful_; }
    void SetIsGuardCounterSuccessful(const bool& flag) { isGuardCounterSuccessful_ = flag; }

    // ---------- �L�[���� ----------
    [[nodiscard]] bool IsComboAttack0KeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_B; }
    [[nodiscard]] bool IsDodgeKeyDown()        const;
    [[nodiscard]] bool IsCounterStanceKey()    const;
    [[nodiscard]] bool IsGetUpKeyDown()        const;
    [[nodiscard]] bool IsDashKey()             const;

    [[nodiscard]] bool IsGuardCounterKeyDown() const { return Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_X; }

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
    bool isAttackHit_           = false;            // �U��������������

    bool isMoveAttackCancel_ = false;  // �ړ����͂ɂ��U���㌄�L�����Z��
    bool isDodgeAttackCancel_ = false; // ����ɂ��U���L�����Z��

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
    Sword sword_;
    bool isSwordPrimitiveDraw_ = false;
    //bool isSwordPrimitiveDraw_ = true;

    // ---------- �� ----------
    DirectX::XMFLOAT3 socketLocation_ = { 700, 10500, -7000 };
    DirectX::XMFLOAT3 socketRotation_ = { 145.0f, -16.0f, 0.0f };
    DirectX::XMFLOAT3 socketScale_ = { 1.0f, 1.0f, -1.0f };
    DirectX::XMFLOAT4X4 weaponWorld_;

    // ---------- �K�[�h�J�E���^�[�@----------
    float guardCounterRadius_       = 0.9f;
    bool  isGuardCounterStance_     = false; // �K�[�h�J�E���^�[��Ԃ�
    bool  isGuardCounterSuccessful_ = false; // ����������
};
