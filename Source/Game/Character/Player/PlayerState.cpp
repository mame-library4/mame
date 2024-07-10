#include "PlayerState.h"
#include <cmath>
#include "Input.h"
#include "Camera.h"
#include "Easing.h"
#include "MathHelper.h"
#include "../Enemy/EnemyManager.h"

// ----- AddForceData -----
namespace PlayerState
{
    // ----- ������ -----
    void AddForceData::Initialize(const float& addForceFrame, const float& force, const float& decelerationForce)
    {
        addForceFrame_ = addForceFrame;
        force_ = force;
        decelerationForce_ = decelerationForce;
        isAddforce_ = false;
    }

    // ----- �X�V -----
    bool AddForceData::Update(const float& animationFrame)
    {
        // ����AddForce���Ă���
        if (isAddforce_) return false;

        // �A�j���[�V�����̃t���[�����w��̃t���[���ɒB���Ă��Ȃ�
        if (animationFrame < addForceFrame_) return false;

        isAddforce_ = true;
        return true;
    }
}

// ----- AttackData -----
namespace PlayerState
{
    // ----- ������ -----
    void AttackData::Initialize(const float& startFrame, const float& endFrame)
    {
        attackStartFrame_ = startFrame;
        attackEndFrame_ = endFrame;
        isAttacked_ = false;
        isFirstTime_ = true;
    }

    // ----- �X�V -----
    bool AttackData::Update(const float& animationFrame, const bool& flag)
    {
        // ���ɍU�����Ă���̂ł����ŏI��
        if (isAttacked_) return false;

        // �U���X�^�[�g�t���[���ɒB���Ă��Ȃ��̂ŏI��
        if (animationFrame < attackStartFrame_) return false;

        // �U���G���h�t���[�����z���Ă���̂ł����ŏI��
        if (animationFrame > attackEndFrame_) return false;

        // �U�������������̂ŏI��
        if (isFirstTime_ == false && flag == false)
        {
            isAttacked_ = true;
            return false;
        }        

        // ��x�ʂ����̂� false �ɂ���
        isFirstTime_ = false;

        return true;
    }
}

// ----- �ҋ@ -----
namespace PlayerState
{
    // ----- ������ -----
    void IdleState::Initialize()
    {
        // �A�j���[�V�����ݒ�
        SetAnimation();

        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        owner_->SetTransitionTime(0.15f);
    }

    // ----- �X�V -----
    void IdleState::Update(const float& elapsedTime)
    {
        // TODO:��ŏ���
        if (GetAsyncKeyState('T') & 0x8000)
        {
            owner_->ChangeState(Player::STATE::Damage);
            return;
        }

        // �J�E���^�[��t
        if (owner_->GetCounterStanceKey())
        {
            owner_->ChangeState(Player::STATE::Counter);
            return;
        }

        // �U������,�����͎�t
        if (owner_->CheckNextInput(Player::NextInput::None)) return;

        // �ړ��l������� MoveState �֑J�ڂ���
        const float aLx = fabsf(Input::Instance().GetGamePad().GetAxisLX());
        const float aLy = fabsf(Input::Instance().GetGamePad().GetAxisLY());
        if (aLx != 0.0f || aLy != 0.0f)
        {
            if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
            {
                owner_->ChangeState(Player::STATE::Run);
                return;
            }

            owner_->ChangeState(Player::STATE::Walk);
            return;
        }
    }

    // ----- �I���� -----
    void IdleState::Finalize()
    {
    }

    // ----- �A�j���[�V�����ݒ� -----
    void IdleState::SetAnimation()
    {
        // �ŏ��̈�񂾂����̏������ʂ�
        if (owner_->GetAnimationIndex() < 0)
        {
            owner_->PlayAnimation(Player::Animation::Idle, true);
            return;
        }

        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        if (animationIndex == Player::Animation::RollForward ||
            animationIndex == Player::Animation::RollBack    ||
            animationIndex == Player::Animation::RollRight   ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->PlayBlendAnimation(Player::Animation::Idle, true);
        }
        else
        {
            owner_->PlayBlendAnimation(Player::Animation::Idle, true);
        }
    }
}

// ----- ���� -----
namespace PlayerState
{
    // ----- ������ -----
    void WalkState::Initialize()
    {
        // �A�j���[�V�����ݒ�
        SetAnimation();

        owner_->ResetFlags();

        // �ő呬�x��ݒ�
        owner_->SetMaxSpeed(2.5f);
    }

    // ----- �X�V -----
    void WalkState::Update(const float& elapsedTime)
    {
        // �J�E���^�[��t
        if (owner_->GetCounterStanceKey())
        {
            owner_->ChangeState(Player::STATE::Counter);
            return;
        }

        // �U������,�����͎�t
        if (owner_->CheckNextInput(Player::NextInput::None)) return;

        // ����
        owner_->Turn(elapsedTime);

        const float aLx = fabsf(Input::Instance().GetGamePad().GetAxisLX());
        const float aLy = fabsf(Input::Instance().GetGamePad().GetAxisLY());
        if (aLx == 0.0f && aLy == 0.0f && owner_->GetIsBlendAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

        if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
        {
            owner_->ChangeState(Player::STATE::Run);
        }
    }

    // ----- �I���� -----
    void WalkState::Finalize()
    {
        owner_->SetMoveDirection({});
        owner_->SetVelocity({});
    }

    // ----- �A�j���[�V�����ݒ� -----
    void WalkState::SetAnimation()
    {
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        if (animationIndex == Player::Animation::RollForward ||
            animationIndex == Player::Animation::RollBack    ||
            animationIndex == Player::Animation::RollRight   ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->SetTransitionTime(0.3f);
        }
        else if (animationIndex == Player::Animation::ComboAttack0_0)
        {
            owner_->SetTransitionTime(0.3f);
        }
        else if (animationIndex == Player::Animation::ComboAttack0_1)
        {
            owner_->SetTransitionTime(0.4f);
        }
        else if (animationIndex == Player::Animation::ComboAttack0_2)
        {
            owner_->SetTransitionTime(0.25f);
        }
        else if (animationIndex == Player::Animation::RunAttack1)
        {
            owner_->SetTransitionTime(0.5f);
        }
        else if (animationIndex == Player::Animation::Run)
        {
            owner_->SetTransitionTime(0.2f);
        }
        else
        {
            owner_->SetTransitionTime(0.15f);
        }

        owner_->PlayBlendAnimation(Player::Animation::Walk, true, 1.4f);
    }
}

// ----- ���� -----
namespace PlayerState
{
    // ----- ������ -----
    void RunState::Initialize()
    {
        // �A�j���[�V�����ݒ�
        SetAnimation();

        owner_->ResetFlags();

        // �ő呬�x��ݒ�
        owner_->SetMaxSpeed(5.0f);

        // �ϐ�������
        stateChangeTimer_ = 0.0f;
    }

    // ----- �X�V -----
    void RunState::Update(const float& elapsedTime)
    {
        // �J�E���^�[��t
        if (owner_->GetCounterStanceKey())
        {
            owner_->ChangeState(Player::STATE::Counter);
            return;
        }

        // �U������,�����͎�t
        if (owner_->GetComboAttack0KeyDown())
        {
            owner_->ChangeState(Player::STATE::RunAttack);
            return;
        }

        if (owner_->CheckNextInput(Player::NextInput::None)) return;

        // ����
        owner_->Turn(elapsedTime);

        // ����p
        if (owner_->GetIsBlendAnimation() == false) stateChangeTimer_ += elapsedTime;

        const float aLx = fabsf(Input::Instance().GetGamePad().GetAxisLX());
        const float aLy = fabsf(Input::Instance().GetGamePad().GetAxisLY());
        if (aLx == 0.0f && aLy == 0.0f)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

        if ((Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER) == false &&
            stateChangeTimer_ > 0.1f)
        {
            owner_->ChangeState(Player::STATE::Walk);
            return;
        }
    }
    
    // ----- �I���� -----
    void RunState::Finalize()
    {
        owner_->SetMoveDirection({});
        owner_->SetVelocity({});
    }

    // ----- �A�j���[�V�����ݒ� -----
    void RunState::SetAnimation()
    {
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        if (animationIndex == Player::Animation::RollForward ||
            animationIndex == Player::Animation::RollBack    ||
            animationIndex == Player::Animation::RollRight   ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->SetTransitionTime(0.2f);
        }
        else if (animationIndex == Player::Animation::ComboAttack0_0 ||
            animationIndex == Player::Animation::ComboAttack0_1 ||
            animationIndex == Player::Animation::RunAttack1)
        {
            owner_->SetTransitionTime(0.3f);
        }
        else if (animationIndex == Player::Animation::ComboAttack0_2)
        {
            owner_->SetTransitionTime(0.3f);
        }
        else
        {
            owner_->SetTransitionTime(0.15f);
        }
        owner_->PlayBlendAnimation(Player::Animation::Run, true);
    }
}

// ----- ���� -----
namespace PlayerState
{
    // ----- ������ -----
    void FlinchState::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::KnockDownStart, false, 2.0f);

        state_ = 0;
    }

    // ----- �X�V -----
    void FlinchState::Update(const float& elapsedTime)
    {
        switch (state_)
        {
        case 0:
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Player::Animation::KnockDownLoop, true);
                state_ = 1;
            }

            break;
        case 1:
            
            if (EnemyManager::Instance().GetEnemy(0)->GetActiveNodeName() != "Roar")
            {
                owner_->PlayBlendAnimation(Player::Animation::KnockDownEnd, false);
                state_ = 2;
            }

            break;
        case 2:
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->ChangeState(Player::STATE::Idle);
            }

            break;
        }

    }

    // ----- �I���� -----
    void FlinchState::Finalize()
    {
    }
}

// ----- �_���[�W -----
namespace PlayerState
{
    // ----- ������ -----
    void DamageState::Initialize()
    {
        // �A�j���[�V�����Đ� 
        owner_->PlayAnimation(Player::Animation::HitLarge, false, 1.2f);

        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �ϐ�������
        addForceData_.Initialize(0.1f, 0.3f, 0.5f);
        isFirstAnimation_ = true;

        DirectX::XMFLOAT3 ownerPos = owner_->GetTransform()->GetPosition();
        DirectX::XMFLOAT3 dragonPos = EnemyManager::Instance().GetEnemy(0)->GetTransform()->GetPosition();
        ownerPos.y = 0;
        dragonPos.y = 0;
        addForceDirection_ = XMFloat3Normalize(ownerPos - dragonPos);

        // ��]
        Turn();
    }

    // ----- �X�V -----
    void DamageState::Update(const float& elapsedTime)
    {
        // �A�j���[�V�����̑��x�ݒ�
        SetAnimationSpeed();

        // �ړ��l
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(addForceDirection_, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // ������΂���A�j���[�V��������
        if (owner_->GetAnimationSeconds() > 1.2f && isFirstAnimation_ == true)
        {// ���͂�����Γ|��Ă��Ԃ��I������
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) != 0.0f || fabsf(aLy) != 0.0f)
            {
                owner_->PlayBlendAnimation(Player::Animation::GetUp, false);
                owner_->SetTransitionTime(0.1f);
                isFirstAnimation_ = false;
            }
        }

        // �A�j���[�V�����I��
        if(owner_->IsPlayAnimation() == false && isFirstAnimation_ == true)
        {
            owner_->PlayBlendAnimation(Player::Animation::GetUp, false);
            owner_->SetTransitionTime(0.1f);
            isFirstAnimation_ = false;
        }
        //else if(owner_->IsPlayAnimation() == false)
        else if(owner_->GetAnimationSeconds() > 1.8f && isFirstAnimation_ == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
        
        if (isFirstAnimation_ == false)
        {
            if (owner_->GetAnimationSeconds() > 1.25f)
            {
                if (owner_->GetAvoidanceKeyDown())
                {
                    owner_->ChangeState(Player::STATE::Avoidance);
                    return;
                }
            }
        }

    }

    // ----- �I���� -----
    void DamageState::Finalize()
    {
    }

    // ----- �A�j���[�V�����̑��x�ݒ� -----
    void DamageState::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // ��ڂ̃A�j���[�V���� ( ������΂��� )
        if (isFirstAnimation_)
        {
            if (animationSeconds > 1.2f)
            {// �N���オ��܂ł̎��Ԃ����΂�
                owner_->SetAnimationSpeed(0.4f);
                //owner_->SetAnimationSpeed(0.1f);
            }
            else if (animationSeconds > 1.0f)
            {
                owner_->SetAnimationSpeed(1.0f);
            }
        }
        // ��ڂ̃A�j���[�V���� ( �N���オ�� )
        else
        {
            if (animationSeconds > 1.0f)
            {
                owner_->SetAnimationSpeed(1.5f);
            }
        }
    }

    // ----- ��]���� -----
    void DamageState::Turn()
    {
        DirectX::XMFLOAT2 ownerFront = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
        ownerFront = XMFloat2Normalize(ownerFront);
        DirectX::XMFLOAT2 addForceDirection = { addForceDirection_.x, addForceDirection_.z };
        addForceDirection = XMFloat2Normalize(addForceDirection * -1.0f);
        
        float cross = XMFloat2Cross(addForceDirection, ownerFront);
        float dot = XMFloat2Dot(addForceDirection, ownerFront) - 1.0f;
        if (cross > 0) owner_->GetTransform()->AddRotationY(dot);
        else owner_->GetTransform()->AddRotationY(-dot);
    }
}

// ----- ���S -----
namespace PlayerState
{
    // ----- ������ -----
    void DeathState::Initialize()
    {
    }

    // ----- �X�V -----
    void DeathState::Update(const float& elapsedTime)
    {
    }

    // ----- �I���� -----
    void DeathState::Finalize()
    {
    }
}

// ----- ��� -----
namespace PlayerState
{
    // ----- ������ -----
    void AvoidanceState::Initialize()
    {
        // �A�j���[�V�����ݒ�
        SetAnimation();

        // �ړ��������Z�o
        CalcMoveDirection();

        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �ϐ�������
        addForceData_.Initialize(0.15f, 0.27f, 0.4f);
    }

    // ----- �X�V -----
    void AvoidanceState::Update(const float& elapsedTime)
    {
        // ��s���͏���
        if (CheckNextInput()) return;

        // �A�j���[�V�����̑��x�ݒ�
        SetAnimationSpeed();

        // �ړ�����
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(moveDirection_, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        if(owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void AvoidanceState::Finalize()
    {
    }

    // ----- ��s���͏��� -----
    const bool AvoidanceState::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // ��s���͎�t
#pragma region ��s���͎�t
        switch (static_cast<Player::Animation>(owner_->GetAnimationIndex()))
        {
        case Player::Animation::RollForward:// �O
        {
            const float nextInputStartFrame = 0.5f; // ��s���͊J�n�t���[��
            if (animationSeconds > nextInputStartFrame)
            {
                // �R���{�U��
                if (owner_->GetComboAttack0KeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::ComboAttack0);
                }
                // ���
                if (owner_->GetAvoidanceKeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::Avoidance);
                }
            }
        }
            break;
        case Player::Animation::RollBack:// ���
        {
            const float nextInputStartFrame = 0.5f;
            if (animationSeconds > nextInputStartFrame)
            {
                // �R���{�U��
                if (owner_->GetComboAttack0KeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::ComboAttack0);
                }
            }
        }
            break;
        case Player::Animation::RollRight:// �E
        {
            const float nextInputStartFrame = 0.5f;
            if (animationSeconds > nextInputStartFrame)
            {
                // �R���{�U��
                if (owner_->GetComboAttack0KeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::ComboAttack0);
                }
                // ���
                if (owner_->GetAvoidanceKeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::Avoidance);
                }
            }
        }
            break;
        case Player::Animation::RollLeft:
        {
            const float nextInputStartFrame = 0.5f;
            if (animationSeconds > nextInputStartFrame)
            {
                // �R���{�U��
                if (owner_->GetComboAttack0KeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::ComboAttack0);
                }
                // ���
                if (owner_->GetAvoidanceKeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::Avoidance);
                }
            }
        }
            break;
        }
#pragma endregion ��s���͎�t

        // ��s���͂ɂ��X�e�[�g�ύX����
#pragma region ��s���͂ɂ��X�e�[�g�ύX����
        switch (static_cast<Player::Animation>(owner_->GetAnimationIndex()))
        {
        case Player::Animation::RollForward:// �O
        {
            // ����̐�s���͂�����ꍇ
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
            {
                const float avoidanceFrame = 0.92f; // ����ɑJ�ڂł���t���[��
                if (animationSeconds > avoidanceFrame)
                {
                    //����͌��݂Ɠ����X�e�[�g�Ȃ̂ŁA���������Ă�
                    Initialize();
                    return true;
                }
            }
            // �R���{�U��0�̏ꍇ
            else if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                const float comboAttack0Frame = 0.9f; // �R���{�U��0�ɑJ�ڂł���t���[��
                if (animationSeconds > comboAttack0Frame)
                {
                    owner_->ChangeState(Player::STATE::ComboAttack0_0);
                    return true;
                }
            }
            // ����ȊO
            else
            {
                const float moveFrame = 0.8f;// �ړ��ɑJ�ڂł���t���[��
                if (animationSeconds > moveFrame)
                {
                    // �ړ��l�����邩����
                    const float aLx = Input::Instance().GetGamePad().GetAxisLX();
                    const float aLy = Input::Instance().GetGamePad().GetAxisLY();
                    if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
                    {
                        // �ړ��l�������Ԃ� RightShoulder��������Ă���� ����ɑJ��
                        if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                        {
                            owner_->ChangeState(Player::STATE::Run);
                            return true;
                        }

                        // �����ɑJ��
                        owner_->ChangeState(Player::STATE::Walk);
                        return true;
                    }
                }
            }
        }
            break;
        case Player::Animation::RollBack:// ���
        {
            // �R���{�U��0�̏ꍇ
            if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                const float comboAttack0Frame = 0.85f; // �R���{�U��0�ɑJ�ڂł���t���[��
                if (animationSeconds > comboAttack0Frame)
                {
                    owner_->ChangeState(Player::STATE::ComboAttack0_0);
                    return true;
                }
            }
            // ����ȊO
            else
            {
                const float moveFrame = 1.0f;// �ړ��ɑJ�ڂł���t���[��
                if (animationSeconds > moveFrame)
                {
                    // �ړ��l�����邩����
                    const float aLx = Input::Instance().GetGamePad().GetAxisLX();
                    const float aLy = Input::Instance().GetGamePad().GetAxisLY();
                    if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
                    {
                        // �ړ��l�������Ԃ� RightShoulder��������Ă���� ����ɑJ��
                        if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                        {
                            owner_->ChangeState(Player::STATE::Run);
                            return true;
                        }

                        // �����ɑJ��
                        owner_->ChangeState(Player::STATE::Walk);
                        return true;
                    }
                }
            }
        }
            break;
        case Player::Animation::RollRight:
        {
            // ����̐�s���͂�����ꍇ
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
            {
                const float avoidanceFrame = 0.92f; // ����ɑJ�ڂł���t���[��
                if (animationSeconds > avoidanceFrame)
                {
                    //����͌��݂Ɠ����X�e�[�g�Ȃ̂ŁA���������Ă�
                    Initialize();
                    return true;
                }
            }
            // �R���{�U��0�̏ꍇ
            else if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                const float comboAttack0Frame = 0.9f; // �R���{�U��0�ɑJ�ڂł���t���[��
                if (animationSeconds > comboAttack0Frame)
                {
                    owner_->ChangeState(Player::STATE::ComboAttack0_0);
                    return true;
                }
            }
            // ����ȊO
            else
            {
                const float moveFrame = 0.8f;// �ړ��ɑJ�ڂł���t���[��
                if (animationSeconds > moveFrame)
                {
                    // �ړ��l�����邩����
                    const float aLx = Input::Instance().GetGamePad().GetAxisLX();
                    const float aLy = Input::Instance().GetGamePad().GetAxisLY();
                    if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
                    {
                        // �ړ��l�������Ԃ� RightShoulder��������Ă���� ����ɑJ��
                        if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                        {
                            owner_->ChangeState(Player::STATE::Run);
                            return true;
                        }

                        // �����ɑJ��
                        owner_->ChangeState(Player::STATE::Walk);
                        return true;
                    }
                }
            }
        }
            break;
        case Player::Animation::RollLeft:
        {
            // ����̐�s���͂�����ꍇ
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
            {
                const float avoidanceFrame = 0.92f; // ����ɑJ�ڂł���t���[��
                if (animationSeconds > avoidanceFrame)
                {
                    //����͌��݂Ɠ����X�e�[�g�Ȃ̂ŁA���������Ă�
                    Initialize();
                    return true;
                }
            }
            // �R���{�U��0�̏ꍇ
            else if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                const float comboAttack0Frame = 0.9f; // �R���{�U��0�ɑJ�ڂł���t���[��
                if (animationSeconds > comboAttack0Frame)
                {
                    owner_->ChangeState(Player::STATE::ComboAttack0_0);
                    return true;
                }
            }
            // ����ȊO
            else
            {
                const float moveFrame = 0.8f;// �ړ��ɑJ�ڂł���t���[��
                if (animationSeconds > moveFrame)
                {
                    // �ړ��l�����邩����
                    const float aLx = Input::Instance().GetGamePad().GetAxisLX();
                    const float aLy = Input::Instance().GetGamePad().GetAxisLY();
                    if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
                    {
                        // �ړ��l�������Ԃ� RightShoulder��������Ă���� ����ɑJ��
                        if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                        {
                            owner_->ChangeState(Player::STATE::Run);
                            return true;
                        }

                        // �����ɑJ��
                        owner_->ChangeState(Player::STATE::Walk);
                        return true;
                    }
                }
            }
        }
            break;
        }
#pragma endregion ��s���͂ɂ��X�e�[�g�ύX����

        return false;
    }

    // ----- �A�j���[�V�����̑��x�ݒ� -----
    void AvoidanceState::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        switch (static_cast<Player::Animation>(owner_->GetAnimationIndex()))
        {
        case Player::Animation::RollForward:// �O
            if (animationSeconds < 0.6f)
            {
                owner_->SetAnimationSpeed(1.4f);
            }
            else
            {
                owner_->SetAnimationSpeed(1.0f);
            }

            break;
        case Player::Animation::RollBack:// ���
            if (animationSeconds < 0.6f)
            {
                owner_->SetAnimationSpeed(1.4f);
            }
            else
            {
                owner_->SetAnimationSpeed(1.0f);
            }

            break;
        case Player::Animation::RollRight:// �E
            if (animationSeconds < 0.6f)
            {
                owner_->SetAnimationSpeed(1.4f);
            }
            else
            {
                owner_->SetAnimationSpeed(1.0f);
            }

            break;
        case Player::Animation::RollLeft:// ��
            if (animationSeconds < 0.6f)
            {
                owner_->SetAnimationSpeed(1.4f);
            }
            else
            {
                owner_->SetAnimationSpeed(1.0f);
            }

            break;
        }
    }

    // ----- �A�j���[�V�����ݒ� -----
    void AvoidanceState::SetAnimation()
    {
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        if (animationIndex == Player::Animation::ComboAttack0_0)
        {
            owner_->SetTransitionTime(0.1f);
        }
        else if (animationIndex == Player::Animation::GetUp)
        {
            //owner_->SetTransitionTime(0.1f);
            owner_->SetTransitionTime(0.2f);
        }
        else
        {
            owner_->SetTransitionTime(0.05f);
        }

        // �v���C���[�̎p���ɍ��킹�ăA�j���[�V�����̕�����ݒ肷��
        const float aLx = Input::Instance().GetGamePad().GetAxisLX();
        const float aLy = Input::Instance().GetGamePad().GetAxisLY();        
        // ���͒l������ꍇ
        if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
        {
            // �J�������猩���X�e�B�b�N�̓��͒l���Z�o����
            const DirectX::XMFLOAT3 cameraFront = Camera::Instance().GetTransform()->CalcForward();
            const DirectX::XMFLOAT3 cameraRight = Camera::Instance().GetTransform()->CalcRight();
            DirectX::XMFLOAT2 cameraInput =
            {
                aLy * cameraFront.x + aLx * cameraRight.x,
                aLy * cameraFront.z + aLx * cameraRight.z,
            };
            cameraInput = XMFloat2Normalize(cameraInput);
            DirectX::XMFLOAT2 ownerFront = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
            ownerFront = XMFloat2Normalize(ownerFront);
            
            // ���ςŊp�x���Z�o
            float dot = acosf(XMFloat2Dot(cameraInput, ownerFront));

            // ���E����
            float corss = XMFloat2Cross(cameraInput, ownerFront);

            // ��]�p���X�O�x������������� �O,�E,�� �̎O��
            if (dot < DirectX::XM_PIDIV2)
            {
                // ��]�p���S�T�x������������� �O����
                if (dot < DirectX::XM_PIDIV4)
                {                    
                    owner_->PlayBlendAnimation(Player::Animation::RollForward, false, 1.0f, 0.15f);
                    return;
                }

                // �E����
                if (corss < 0)
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollRight, false, 1.0f, 0.15f);
                }
                // ������
                else
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollLeft, false, 1.0f, 0.15f);
                }

            }
            // ��]�p���X�O�x�����傫����� ��,�E,�� �̎O��
            else
            {
                // ��]�p���P�R�T�x�����傫����� �����
                if (dot > DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4)
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollBack, false, 1.0f, 0.15f);
                    return;
                }

                // �E����
                if (corss < 0)
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollRight, false, 1.0f, 0.15f);
                }
                // ������
                else
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollLeft, false, 1.0f, 0.15f);
                }
            }
        }
        // ���͒l���Ȃ��ꍇ�O�����̃A�j���[�V������ݒ肷��
        else
        {
            owner_->PlayBlendAnimation(Player::Animation::RollForward, false, 1.0f, 0.15f);
            return;
        }
    }

    // ----- �ړ������Z�o -----
    void AvoidanceState::CalcMoveDirection()
    {        
        // �A�j���[�V�����ɂ���Ĉړ������𕪂���
        switch (static_cast<Player::Animation>(owner_->GetAnimationIndex()))
        {
        case Player::Animation::RollForward:
            // �v���C���[�̑O����
            moveDirection_ = owner_->GetTransform()->CalcForward();
            direction_ = Direction::Fornt;
            
            break;
        case Player::Animation::RollBack:
            // �v���C���[�̌�����
            moveDirection_ = owner_->GetTransform()->CalcForward() * -1;
            direction_ = Direction::Back;
            
            break;
        case Player::Animation::RollRight:
            // �v���C���[�̉E����
            moveDirection_ = owner_->GetTransform()->CalcRight();
            direction_ = Direction::Right;
            
            break;
        case Player::Animation::RollLeft:
            // �v���C���[�̍�����
            moveDirection_ = owner_->GetTransform()->CalcRight() * -1;
            direction_ = Direction::Left;
            
            break;
        }
    }
}

// ----- �J�E���^�[ -----
namespace PlayerState
{
    // ----- ������ -----
    void CounterState::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::ParryCounterAttack0, false);

        // �ϐ�������
        power_[Direction::Front] = 0.6f;
        power_[Direction::Back] = 0.4f;
        addForceFrame_[Direction::Front] = 0.66f;
        addForceFrame_[Direction::Back]  = 0.16f;
        //isAddForce_[Direction::Front] = false;
        //isAddForce_[Direction::Back]  = false;
        isAddForce_[Direction::Front] = true;
        isAddForce_[Direction::Back] = true;


        animationSlowStartFrame_ = 0.5f;
        animationSlowEndFrame_ = 0.7f;

        // �t���O�����Z�b�g����
        owner_->ResetFlags();
    }

    // ----- �X�V -----
    void CounterState::Update(const float& elapsedTime)
    {
        // �A�j���[�V�����ɍ��킹�đO�����Ɉړ�����
        if (isAddForce_[Direction::Front] == false) AddForceFront(elapsedTime);
        
        // �A�j���[�V�����ɍ��킹�Č������Ɉړ�����
        if (isAddForce_[Direction::Back] == false) AddForceBack(elapsedTime);        


        // �A�j���[�V�����Đ��I��
        if (!owner_->IsPlayAnimation())
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

        // �J�E���^�[����
        // TODO:��������B�J�E���^�[
        //if (GetAsyncKeyState('B') & 1)
        //if(owner_->GetStrongAttackKeyDown())
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_A)
        {
            //if (currentAnimationFrame >= animationSlowEndFrame_) return;

            owner_->ChangeState(Player::STATE::CounterCombo);
            return;
        }
    }

    // ----- �I���� -----
    void CounterState::Finalize()
    {
    }

    // ----- �O�����Ɉړ����� -----
    void CounterState::AddForceFront(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // �w��̃A�j���[�V�����t���[���𒴂����珈��������
        if (animationSeconds > addForceFrame_[Direction::Front])
        {
            const DirectX::XMFLOAT3 direction = owner_->GetTransform()->CalcForward();

            owner_->AddForce(direction, power_[Direction::Front]);

            // ���̏����͈�x�����ł����̂Ńt���O�𗧂Ă�
            isAddForce_[Direction::Front] = true;
        }
    }

    // ----- �������Ɉړ����� -----
    void CounterState::AddForceBack(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // �w��̃A�j���[�V�����t���[���𒴂����珈��������
        if (animationSeconds > addForceFrame_[Direction::Back])
        {
            // ���X�e�B�b�N�̓��͂����邩����
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                // �J�������猩�����X�e�B�b�N�̌X����K�������������Z�o����
                const DirectX::XMFLOAT3 cameraForward = Camera::Instance().CalcForward();
                const DirectX::XMFLOAT3 cameraRight = Camera::Instance().CalcRight();
                DirectX::XMFLOAT3 direction =
                {
                    aLy * cameraForward.x + aLx * cameraRight.x,
                    0,
                    aLy * cameraForward.z + aLx * cameraRight.z
                };
                direction = XMFloat3Normalize(direction);
                direction = direction * -1;

                owner_->AddForce(direction, power_[Direction::Back]);

                // ��C�ɃX�e�B�b�N�𔽑Ε����Ɍ�����ƁA�v���C���[���Ⴄ�����������̂ŏC��
                DirectX::XMFLOAT2 cameraForward_float2 = { -direction.x, -direction.z };
                cameraForward_float2 = XMFloat2Normalize(cameraForward_float2);

                DirectX::XMFLOAT2 ownerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
                ownerForward = XMFloat2Normalize(ownerForward);

                // �O�ς����Ăǂ���ɉ�]����̂��𔻒肷��
                float forwardCorss = XMFloat2Cross(cameraForward_float2, ownerForward);

                // ���ςŉ�]�����Z�o
                float forwardDot = XMFloat2Dot(cameraForward_float2, ownerForward) - 1.0f;

                if (forwardCorss > 0)
                {
                    owner_->GetTransform()->AddRotationY(forwardDot);
                }
                else
                {
                    owner_->GetTransform()->AddRotationY(-forwardDot);
                }
            }
            // ���X�e�B�b�N�̓��͂��Ȃ��ꍇ�������Ɉ���
            else
            {
                const DirectX::XMFLOAT3 forwardVec = owner_->GetTransform()->CalcForward();
                const DirectX::XMFLOAT3 direction = forwardVec * -1.0f;

                owner_->AddForce(direction, power_[Direction::Back]);
            }

            // ���̏����͈�x�����ł����̂Ńt���O�𗧂Ă�
            isAddForce_[Direction::Back] = true;
        }
    }
}

// ----- �J�E���^�[�U�� -----
namespace PlayerState
{
    // ----- ������ -----
    void CounterComboState::Initialize()
    {
        // �A�j���[�V�����Đ�
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_1, false);
        

        // �ϐ�������
        addForceFrame_ = 0.3f;
        isAddForce_ = true;
        //isAddForce_ = false;

        Input::Instance().GetGamePad().Vibration(0.3f, 1.0f);

        // �t���O�����Z�b�g����
        owner_->ResetFlags();
    }

    // ----- �X�V -----
    void CounterComboState::Update(const float& elapsedTime)
    {
        //const float currentAnimationFrame = owner_->GetBlendAnimationSeconds();
        //if (currentAnimationFrame > addForceFrame_ &&
        //    isAddForce_ == false)
        //{
        //    // �O�����ɐi��
        //    const DirectX::XMFLOAT3 forwardVec = owner_->GetTransform()->CalcForward();
        //    owner_->AddForce(forwardVec, 0.7f);

        //    isAddForce_ = true;
        //}

        // �A�j���[�V�����I��
        if (!owner_->IsPlayAnimation())
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void CounterComboState::Finalize()
    {
    }
}

// ----- ����U�� -----
namespace PlayerState
{
    // ----- ������ -----
    void RunAttackState::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::RunAttack1, false, 1.0f, 0.2f);
        owner_->SetTransitionTime(0.1f);

        // �t���O���Z�b�g
        owner_->ResetFlags();

        // �ϐ�������
        addForceData_.Initialize(0.2f, 0.4f, 1.0f);
        attackData_.Initialize(0.25f, 0.5f);
    }

    // ----- �X�V -----
    void RunAttackState::Update(const float& elapsedTime)
    {
        // ��s���͏���
        if (CheckNextInput()) return;

        // �ړ�����
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // �U�����菈��
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAbleAttack());
        owner_->SetIsAbleAttack(attackFlag);

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void RunAttackState::Finalize()
    {
    }

    // ----- ��s���͏��� -----
    const bool RunAttackState::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        const float changeComboAttack0StateFrame = 0.6f; // �X�e�[�g�؂�ւ�
        if (animationSeconds > changeComboAttack0StateFrame)
        {
            if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_1);
                return true;
            }
        }

        // �ړ����͒l������΃X�e�[�g�؂�ւ�
        const float changeMoveStateFrame = 0.8f;
        if (animationSeconds > changeMoveStateFrame)
        {
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                {
                    owner_->ChangeState(Player::STATE::Run);
                    return true;
                }

                owner_->ChangeState(Player::STATE::Walk);
                return true;
            }
        }

        const float comboAttack0NextInputStartFrame = 0.3f; // ��s���͊J�n�t���[��
        if (animationSeconds > comboAttack0NextInputStartFrame)
        {
            if (owner_->GetComboAttack0KeyDown())
            {
                owner_->SetNextInput(Player::NextInput::ComboAttack0);
            }
        }

        return false;
    }
}

// ----- �R���{�U��0_0 -----
namespace PlayerState
{
    // ----- ������ -----
    void ComboAttack0_0::Initialize()
    {
        // �A�j���[�V�����ݒ�
        SetAnimation();

        // �t���O���Z�b�g
        owner_->ResetFlags();

        // �ϐ�������
        addForceData_.Initialize(0.15f, 0.2f, 1.0f);
        attackData_.Initialize(0.1f, 0.35f);
    }

    // ----- �X�V -----
    void ComboAttack0_0::Update(const float& elapsedTime)
    {
        // ��s����
        if (CheckNextInput()) return;
        
        // �A�j���[�V�����̑��x�ݒ�
        SetAnimationSpeed();

        // �ړ�����
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(owner_->GetTransform()->CalcForward(),
                addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // �U�����菈��
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAbleAttack());
        owner_->SetIsAbleAttack(attackFlag);

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            //owner_->ChangeState(Player::STATE::ComboAttack0_1);
            return;
        }
    }

    // ----- �I���� -----
    void ComboAttack0_0::Finalize()
    {
    }

    // ----- �A�j���[�V�����ݒ� -----
    void ComboAttack0_0::SetAnimation()
    {
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        // oldAnimation������̏ꍇ�U���̓r���t���[������J�n����
        if (animationIndex == Player::Animation::RollForward ||
            animationIndex == Player::Animation::RollBack    ||
            animationIndex == Player::Animation::RollRight   ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_0, false, 1.0f, 0.1f);
        }
        else if (animationIndex == Player::Animation::Run)
        {
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_0, false, 1.0f, 0.1f);
            owner_->SetTransitionTime(0.1f);
        }
        else if (animationIndex == Player::Animation::ComboAttack0_1)
        {
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_0, false, 1.0f, 0.1f);
            owner_->SetTransitionTime(0.1f);
        }
        else
        {
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_0, false);
            owner_->SetTransitionTime(0.1f);
        }
    }

    // ----- �A�j���[�V�������x�ݒ� -----
    void ComboAttack0_0::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > 1.2f)
        {
            owner_->SetAnimationSpeed(3.0f);
        }
        else if (animationSeconds > 1.0f)
        {
            owner_->SetAnimationSpeed(2.0f);
        }
        else if (animationSeconds > 0.6f)
        {
            owner_->SetAnimationSpeed(1.5f);
        }
        else if (animationSeconds > 0.3f)
        {
            owner_->SetAnimationSpeed(1.3f);
        }
    }

    // ----- ��s���͏��� -----
    const bool ComboAttack0_0::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // ��s���͎�t
        if (animationSeconds > 0.1)
        {
            // �R���{�U��
            if (owner_->GetComboAttack0KeyDown())
            {
                owner_->SetNextInput(Player::NextInput::ComboAttack0);
            }
            // ���
            if (owner_->GetAvoidanceKeyDown())
            {
                owner_->SetNextInput(Player::NextInput::Avoidance);
            }
        }

        if (animationSeconds > 0.3f)
        {
            // �R���{�U��
            if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_1);
                return true;
            }
        }

        if(animationSeconds > 0.4f)
        {
            // ���
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
            {
                owner_->ChangeState(Player::STATE::Avoidance);
                return true;
            }
        }

        if (animationSeconds > 0.6f)
        {
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                {
                    owner_->ChangeState(Player::STATE::Run);
                    return true;
                }

                owner_->ChangeState(Player::STATE::Walk);
                return true;
            }
        }

        return false;
    }
}

// ----- �R���{�U��0_1 -----
namespace PlayerState
{
    // ----- ������ -----
    void ComboAttack0_1::Initialize()
    {
        // �A�j���[�V�����ݒ�
        SetAnimation();

        // �t���O���Z�b�g
        owner_->ResetFlags();

        // �ϐ�������
        addForceData_.Initialize(0.05f, 0.25f, 1.0f);
        attackData_.Initialize(0.06f, 0.3f);
    }

    // ----- �X�V -----
    void ComboAttack0_1::Update(const float& elapsedTime)
    {
        // ��s���͏���
        if (CheckNextInput()) return;

        // �A�j���[�V�����̑��x�ݒ�
        SetAnimationSpeed();

        // �ړ�����
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // �U�����菈��
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAbleAttack());
        owner_->SetIsAbleAttack(attackFlag);

        //if (owner_->IsPlayAnimation() == false)
        if(owner_->GetAnimationSeconds() > 1.0f)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void ComboAttack0_1::Finalize()
    {

    }

    // ----- �A�j���[�V�����ݒ� -----
    void ComboAttack0_1::SetAnimation()
    {
        if (owner_->GetAnimationIndex() == static_cast<int>(Player::Animation::RunAttack1))
        {
            owner_->SetTransitionTime(0.2f);
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_1, false, 1.0f, 0.1f);
        }
        else
        {
            owner_->SetTransitionTime(0.1f);
            owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_1, false);
        }
    }

    // ----- �A�j���[�V�������x�ݒ� -----
    void ComboAttack0_1::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > 1.0f)
        {
            owner_->SetAnimationSpeed(2.5f);
        }
        else if (animationSeconds > 0.6f)
        {
            owner_->SetAnimationSpeed(1.5f);
        }
        else if (animationSeconds > 0.3f)
        {
            owner_->SetAnimationSpeed(1.3f);
        }
    }

    // ----- ��s���͏��� -----
    const bool ComboAttack0_1::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();
        
        if (animationSeconds > 0.1f)
        {
            // �R���{�U��
            if (owner_->GetComboAttack0KeyDown())
            {
                owner_->SetNextInput(Player::NextInput::ComboAttack0);
            }
        }
        if (animationSeconds > 0.1f)
        {
            // ���
            if (owner_->GetAvoidanceKeyDown())
            {
                owner_->SetNextInput(Player::NextInput::Avoidance);
            }
        }

        // ����ɑJ�ڂł���t���[��
        const float changeAvoidanceFrame = 0.25f;
        if (animationSeconds > changeAvoidanceFrame)
        {
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
            {
                owner_->ChangeState(Player::STATE::Avoidance);
                return true;
            }
        }

        const float changeComboAttack0Frame = 0.3f;
        if (animationSeconds > changeComboAttack0Frame)
        {
            if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_2);
                return true;
            }
        }

        // �ړ����͒l������΃X�e�[�g�؂�ւ�
        const float changeMoveStateFrame = 0.5f;
        if (animationSeconds > changeMoveStateFrame)
        {
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                {
                    owner_->ChangeState(Player::STATE::Run);
                    return true;
                }

                owner_->ChangeState(Player::STATE::Walk);
                return true;
            }
        }

        return false;
    }
}

// ----- �R���{�U��0_2 -----
namespace PlayerState
{
    // ----- ������ -----
    void ComboAttack0_2::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_2, false, 1.3f, 0.4f);
        owner_->SetTransitionTime(0.3f);

        // �t���O���Z�b�g
        owner_->ResetFlags();

        // �ϐ�������
        addForceData_.Initialize(0.6f, 0.2f, 1.0f);
        attackData_.Initialize(0.7f, 0.9f);
    }

    // ----- �X�V -----
    void ComboAttack0_2::Update(const float& elapsedTime)
    {
        // ��s���͏���
        if (CheckNextInput()) return;

        // �A�j���[�V�����̑��x�ݒ�
        SetAnimationSpeed();

        // �ړ�����        
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }        

        // �U�����菈��
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAbleAttack());
        owner_->SetIsAbleAttack(attackFlag);


        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void ComboAttack0_2::Finalize()
    {

    }

    // ----- �A�j���[�V�����̑��x�ݒ� -----
    void ComboAttack0_2::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > 1.4f)
        {
            owner_->SetAnimationSpeed(2.5f);
        }
        else if (animationSeconds > 1.2f)
        {
            owner_->SetAnimationSpeed(2.0f);
        }
        else if (animationSeconds > 0.7f)
        {
            owner_->SetAnimationSpeed(1.2f);
        }
    }

    // ----- ��s���͏��� -----
    const bool ComboAttack0_2::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > 0.7f)
        {
            if (owner_->GetComboAttack0KeyDown())
            {
                owner_->SetNextInput(Player::NextInput::ComboAttack0);
            }
        }

        if (animationSeconds > 0.9f)
        {
            if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_3);
                return true;
            }
        }

        if (animationSeconds > 0.7f)
        {
            if (owner_->GetAvoidanceKeyDown())
            {
                owner_->SetNextInput(Player::NextInput::Avoidance);
            }
        }
        if (animationSeconds > 1.1f)
        {
            if (owner_->GetNextInput() == Player::NextInput::Avoidance)
            {
                owner_->ChangeState(Player::STATE::Avoidance);
                return true;
            }
        }

        // �ړ��l������� �ړ��֑J��
        if (animationSeconds > 1.4f)
        {
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
                {
                    owner_->ChangeState(Player::STATE::Run);
                    return true;
                }

                owner_->ChangeState(Player::STATE::Walk);
                return true;
            }
        }

        return false;
    }
}

// ----- �R���{�U��0_3 -----
namespace PlayerState
{
    // ----- ������ -----
    void ComboAttack0_3::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_3, false);
        owner_->SetTransitionTime(0.1f);

        // �t���O���Z�b�g
        owner_->ResetFlags();

        // �ϐ�������
        addForceData_.Initialize(0.45f, 0.25f, 0.5f);
        isVibration_ = false;
    }

    // ----- �X�V -----
    void ComboAttack0_3::Update(const float& elapsedTime)
    {
        // �A�j���[�V�����̑��x�ݒ�
        SetAnimationSpeed();

        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // �R���g���[���[���J���� �U��
        if (owner_->GetAnimationSeconds() > 0.8f && isVibration_ == false)
        {
            Input::Instance().GetGamePad().Vibration(0.2f, 1.0f);
            Camera::Instance().ScreenVibrate(0.1f, 0.2f);

            isVibration_ = true;
        }

        //if (owner_->IsPlayAnimation() == false)
        //if(owner_->GetAnimationSeconds() > 1.6f)
        if(owner_->GetAnimationSeconds() > 1.7f)
        {
            //const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            //const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            //if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            //{
            //    if (Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER)
            //    {
            //        owner_->ChangeState(Player::STATE::Run);
            //        return;
            //    }

            //    owner_->ChangeState(Player::STATE::Walk);
            //    return;
            //}

            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void ComboAttack0_3::Finalize()
    {

    }

    // ----- �A�j���[�V�����̑��x�ݒ� -----
    void ComboAttack0_3::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();


    }
}