#include "PlayerState.h"
#include <cmath>
#include "Input.h"
#include "Camera.h"
#include "Easing.h"
#include "MathHelper.h"

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
        int a = owner_->GetAnimationIndex();
        if (a < 0)
        {
            owner_->PlayAnimation(Player::Animation::Idle, true);
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
        owner_->PlayBlendAnimation(Player::Animation::Walk, true);

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
        if (aLx == 0.0f || aLy == 0.0f)
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
}

namespace PlayerState
{
    void RunState::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::Run, true);

        owner_->ResetFlags();

        // �ő呬�x��ݒ�
        owner_->SetMaxSpeed(5.0f);
    }
    void RunState::Update(const float& elapsedTime)
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
        if (aLx == 0.0f || aLy == 0.0f)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

        if ((Input::Instance().GetGamePad().GetButton() & GamePad::BTN_RIGHT_SHOULDER) == false)
        {
            owner_->ChangeState(Player::STATE::Walk);
            return;
        }
    }
    void RunState::Finalize()
    {
        owner_->SetMoveDirection({});
        owner_->SetVelocity({});
    }
}

// ----- �ړ� -----
namespace PlayerState
{
    // ----- ������ -----
    void MoveState::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::Run, true);
        //owner_->PlayBlendAnimation(Player::Animation::Idle, Player::Animation::Run, true);
        //owner_->SetWeight(0.0f);

        // �t���O�����Z�b�g����
        owner_->ResetFlags();
    }

    // ----- �X�V -----
    void MoveState::Update(const float& elapsedTime)
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
    }

    // ----- �I���� -----
    void MoveState::Finalize()
    {
        // ���x�����Z�b�g����
        owner_->SetVelocity({});
    }
}

// ----- �_���[�W -----
namespace PlayerState
{
    // ----- ������ -----
    void DamageState::Initialize()
    {
        // �A�j���[�V�����Đ� 
        owner_->PlayAnimation(Player::Animation::GetHit, false);

        // �t���O�����Z�b�g����
        owner_->ResetFlags();
    }

    // ----- �X�V -----
    void DamageState::Update(const float& elapsedTime)
    {
        // �A�j���[�V�����I��
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void DamageState::Finalize()
    {
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

        // �t���O�����Z�b�g����
        owner_->ResetFlags();
    }

    // ----- �X�V -----
    void AvoidanceState::Update(const float& elapsedTime)
    {
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void AvoidanceState::Finalize()
    {
    }

    // ----- �A�j���[�V�����ݒ� -----
    void AvoidanceState::SetAnimation()
    {
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
                    owner_->PlayBlendAnimation(Player::Animation::StepFront, false);
                    return;
                }

                // �E����
                if (corss < 0)
                {
                    owner_->PlayBlendAnimation(Player::Animation::StepRight, false);
                }
                // ������
                else
                {
                    owner_->PlayBlendAnimation(Player::Animation::StepLeft, false);
                }

            }
            // ��]�p���X�O�x�����傫����� ��,�E,�� �̎O��
            else
            {
                // ��]�p���P�R�T�x�����傫����� �����
                if (dot > DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4)
                {
                    owner_->PlayBlendAnimation(Player::Animation::StepBack, false);
                    return;
                }

                // �E����
                if (corss < 0)
                {
                    owner_->PlayBlendAnimation(Player::Animation::StepRight, false);
                }
                // ������
                else
                {
                    owner_->PlayBlendAnimation(Player::Animation::StepLeft, false);
                }
            }
        }
        // ���͒l���Ȃ��ꍇ�O�����̃A�j���[�V������ݒ肷��
        else
        {
            owner_->PlayBlendAnimation(Player::Animation::StepBack, false);
            return;
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
        owner_->PlayBlendAnimation(Player::Animation::Counter, false);


        // �ϐ�������
        power_ = 0.4f;
        addForceFrame_ = 0.16f;
        isAddForce_ = false;

        animationSlowStartFrame_ = 0.5f;
        animationSlowEndFrame_ = 0.7f;

        // �t���O�����Z�b�g����
        owner_->ResetFlags();
    }

    // ----- �X�V -----
    void CounterState::Update(const float& elapsedTime)
    {
        



        //if (currentAnimationFrame > addForceFrame_ &&
        //    isAddForce_ == false)
        //{
        //    // ���X�e�B�b�N�̓��͂����邩����
        //    const float aLx = Input::Instance().GetGamePad().GetAxisLX();
        //    const float aLy = Input::Instance().GetGamePad().GetAxisLY();
        //    if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
        //    {
        //        // �J�������猩�����X�e�B�b�N�̌X����K�������������Z�o����
        //        const DirectX::XMFLOAT3 cameraForward = Camera::Instance().CalcForward();
        //        const DirectX::XMFLOAT3 cameraRight = Camera::Instance().CalcRight();
        //        DirectX::XMFLOAT3 direction =
        //        {
        //            aLy * cameraForward.x + aLx * cameraRight.x,
        //            0,
        //            aLy * cameraForward.z + aLx * cameraRight.z
        //        };
        //        direction = XMFloat3Normalize(direction);
        //        direction = direction * -1;

        //        owner_->AddForce(direction, power_);

        //        // ��C�ɃX�e�B�b�N�𔽑Ε����Ɍ�����ƁA�v���C���[���Ⴄ�����������̂ŏC��
        //        DirectX::XMFLOAT2 cameraForward_float2 = { -direction.x, -direction.z };
        //        cameraForward_float2 = XMFloat2Normalize(cameraForward_float2);

        //        DirectX::XMFLOAT2 ownerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
        //        ownerForward = XMFloat2Normalize(ownerForward);

        //        // �O�ς����Ăǂ���ɉ�]����̂��𔻒肷��
        //        float forwardCorss = XMFloat2Cross(cameraForward_float2, ownerForward);

        //        // ���ςŉ�]�����Z�o
        //        float forwardDot = XMFloat2Dot(cameraForward_float2, ownerForward) - 1.0f;

        //        if (forwardCorss > 0)
        //        {
        //            owner_->GetTransform()->AddRotationY(forwardDot);
        //        }
        //        else
        //        {
        //            owner_->GetTransform()->AddRotationY(-forwardDot);
        //        }
        //    }
        //    // ���X�e�B�b�N�̓��͂��Ȃ��ꍇ�������Ɉ���
        //    else
        //    {
        //        const DirectX::XMFLOAT3 forwardVec = owner_->GetTransform()->CalcForward();
        //        const DirectX::XMFLOAT3 direction = forwardVec * -1.0f;

        //        owner_->AddForce(direction, power_);
        //    }

        //    isAddForce_ = true;
        //}


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
}

// ----- �J�E���^�[�U�� -----
namespace PlayerState
{
    // ----- ������ -----
    void CounterComboState::Initialize()
    {
        // �A�j���[�V�����Đ�
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_3, false);
        

        // �ϐ�������
        addForceFrame_ = 0.3f;
        isAddForce_ = false;

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

// ----- �R���{�U��0_0 -----
namespace PlayerState
{
    // ----- ������ -----
    void ComboAttack0_0::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_0, false, normalAnimationSpeed_);
        

        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �ϐ�������
        nextInputStartFrame_ = 0.3f;
        nextInputEndFrame_ = 0.85f;
        nextAttackFrame_ = 0.55f;
        slowAnimationStartFrame_ = 0.3f;
        isSlowAnimation_ = false;   
    }

    // ----- �X�V -----
    void ComboAttack0_0::Update(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // �A�j���[�V�����̑��x�𐧌䂷��
        if (animationSeconds > slowAnimationStartFrame_ && isSlowAnimation_ == false)
        {
            owner_->SetAnimationSpeed(slowAnimationSpeed_);
            isSlowAnimation_ = true;
        }

        // ��s���͂��L���ȃt���[��
        if (animationSeconds > nextInputStartFrame_ && animationSeconds < nextInputEndFrame_)
        {
            // ��s���͎�t
            owner_->CheckNextInput(Player::NextInput::ComboAttack0);
            owner_->SetTransitionTime(0.1f);
        }
        if (animationSeconds > nextAttackFrame_ && owner_->GetNextInput() != Player::NextInput::None)
        {
            switch (owner_->GetNextInput())
            {
            case Player::NextInput::ComboAttack0:
                owner_->ChangeState(Player::STATE::ComboAttack0_1);
                break;
            case Player::NextInput::ComboAttack1:
                owner_->ChangeState(Player::STATE::ComboAttack1_0);
                break;
            case Player::NextInput::Avoidance:
                owner_->ChangeState(Player::STATE::Avoidance);
                break;
            }
        }


        // �A�j���[�V�����Đ��I��
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void ComboAttack0_0::Finalize()
    {
    }
}

// ----- �R���{�U��0_1 -----
namespace PlayerState
{
    // ----- ������ -----
    void ComboAttack0_1::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_1, false, 1.0f);
        

        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �ϐ�������
        nextInputStartFrame_ = 0.1f;
        nextInputEndFrame_ = 0.6f;
    }

    // ----- �X�V -----
    void ComboAttack0_1::Update(const float& elapsedTime)
    {
        //const float currentAnimationSeconds = owner_->GetBlendAnimationSeconds();

        //// ��s���͂��L���ȃt���[��
        //if (currentAnimationSeconds > nextInputStartFrame_ && currentAnimationSeconds < nextInputEndFrame_)
        //{
        //    owner_->CheckNextInput(Player::NextInput::ComboAttack0);
        //}
        //else if (currentAnimationSeconds > nextInputEndFrame_ && owner_->GetNextInput() != Player::NextInput::None)
        //{
        //    switch (owner_->GetNextInput())
        //    {
        //    case Player::NextInput::ComboAttack0:
        //        owner_->ChangeState(Player::STATE::ComboAttack0_2);
        //        break;
        //    case Player::NextInput::ComboAttack1:
        //        owner_->ChangeState(Player::STATE::ComboAttack1_0);
        //        break;
        //    case Player::NextInput::Avoidance:
        //        owner_->ChangeState(Player::STATE::Avoidance);
        //        break;
        //    }
        //}

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void ComboAttack0_1::Finalize()
    {
    }
}

// ----- �R���{�U��0_2 -----
namespace PlayerState
{
    // ----- ������ -----
    void ComboAttack0_2::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_2, false);
        

        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �ϐ�������
        nextInputStartFrame_ = 0.1f;
        nextInputEndFrame_ = 0.65f;
    }

    // ----- �X�V -----
    void ComboAttack0_2::Update(const float& elapsedTime)
    {
        //const float currentAnimationSeconds = owner_->GetBlendAnimationSeconds();

        //// ��s���͂��L���ȃt���[��
        //if (currentAnimationSeconds > nextInputStartFrame_ && currentAnimationSeconds < nextInputEndFrame_)
        //{
        //    owner_->CheckNextInput(Player::NextInput::ComboAttack0);
        //}
        //else if (currentAnimationSeconds > nextInputEndFrame_ && owner_->GetNextInput() != Player::NextInput::None)
        //{
        //    switch (owner_->GetNextInput())
        //    {
        //    case Player::NextInput::ComboAttack0:
        //        owner_->ChangeState(Player::STATE::ComboAttack0_3);
        //        break;
        //    case Player::NextInput::ComboAttack1:
        //        owner_->ChangeState(Player::STATE::ComboAttack1_0);
        //        break;
        //    case Player::NextInput::Avoidance:
        //        owner_->ChangeState(Player::STATE::Avoidance);
        //        break;
        //    }
        //}


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
}

// ----- �R���{�U��0_3 -----
namespace PlayerState
{
    void ComboAttack0_3::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_3, false);
        

        // �t���O�����Z�b�g����
        owner_->ResetFlags();
    }

    void ComboAttack0_3::Update(const float& elapsedTime)
    {
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    void ComboAttack0_3::Finalize()
    {
    }
}

// ----- �R���{�U��1_0 -----
namespace PlayerState
{
    void ComboAttack1_0::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack1_0, false);
        
    }

    void ComboAttack1_0::Update(const float& elapsedTime)
    {
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::ComboAttack1_1);
            return;
        }
    }

    void ComboAttack1_0::Finalize()
    {
    }
}

// ----- �R���{�U��1_1 -----
namespace PlayerState
{
    void ComboAttack1_1::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack1_1, false);
        
    }

    void ComboAttack1_1::Update(const float& elapsedTime)
    {
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::ComboAttack1_2);
            return;
        }
    }

    void ComboAttack1_1::Finalize()
    {
    }
}

// ----- �R���{�U��1_2 -----
namespace PlayerState
{
    void ComboAttack1_2::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack1_2, false);
        
    }

    void ComboAttack1_2::Update(const float& elapsedTime)
    {
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    void ComboAttack1_2::Finalize()
    {
    }
}