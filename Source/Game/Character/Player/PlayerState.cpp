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
    }

    // ----- �X�V -----
    void IdleState::Update(const float& elapsedTime)
    {
        // �U������,�����͎�t
        if (owner_->CheckNextInput(Player::NextInput::None)) return;

        // �J�E���^�[��t
        if (owner_->GetCounterStanceKey())
        {
            owner_->ChangeState(Player::STATE::Counter);
            return;
        }

        // �ړ��l������� MoveState �֑J�ڂ���
        const float aLx = fabsf(Input::Instance().GetGamePad().GetAxisLX());
        const float aLy = fabsf(Input::Instance().GetGamePad().GetAxisLY());
        if (aLx != 0.0f || aLy != 0.0f)
        {
            owner_->ChangeState(Player::STATE::Move);
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
        // �A�j���[�V�������ݒ肳��Ă��Ȃ�
        if (owner_->GetBlendAnimationIndex1() == -1)
        {
            owner_->PlayBlendAnimation(Player::Animation::Walk, Player::Animation::Idle, true);
        }
        // ���݈�ڂɐݒ肵�Ă��郂�[�V������Walk�̏ꍇ�ݒ肵���˂�̂ő΍�
        else if (owner_->GetBlendAnimationIndex1() == static_cast<int>(Player::Animation::Walk))
        {
            owner_->PlayBlendAnimation(Player::Animation::Run, Player::Animation::Idle, true);
        }
        // ����ȊO�͂����ɓ��� (��{�I�ɂ����̏����ɂȂ�)
        else
        {
            owner_->PlayBlendAnimation(Player::Animation::Walk, Player::Animation::Idle, true);
        }

        owner_->SetWeight(1.0f);
    }
}

// ----- �ړ� -----
namespace PlayerState
{
    // ----- ������ -----
    void MoveState::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::Idle, Player::Animation::Run, true);
        owner_->SetWeight(0.0f);
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

        // �ړ�
        owner_->Move(elapsedTime);
    }

    // ----- �I���� -----
    void MoveState::Finalize()
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
        owner_->PlayAnimation(Player::Animation::GetHit, false);
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
    }

    // ----- �X�V -----
    void AvoidanceState::Update(const float& elapsedTime)
    {
    }

    // ----- �I���� -----
    void AvoidanceState::Finalize()
    {
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
        owner_->SetWeight(1.0f);

        // �ϐ�������
        power_ = 0.4f;
        addForceFrame_ = 0.16f;
        isAddForce_ = false;

        animationSlowStartFrame_ = 0.5f;
        animationSlowEndFrame_ = 0.7f;
    }

    // ----- �X�V -----
    void CounterState::Update(const float& elapsedTime)
    {
        const float currentAnimationFrame = owner_->GetBlendAnimationSeconds();

        // �w��̃A�j���[�V�����̊Ԃ̎��Ԃ�ݒ肷��
        if (currentAnimationFrame > animationSlowStartFrame_ && currentAnimationFrame < animationSlowEndFrame_)
        {
            owner_->SetAnimationSpeed(owner_->slowAnimationSpeed_);
        }
        else
        {
            owner_->SetAnimationSpeed(1.0f);
        }

        if (currentAnimationFrame > addForceFrame_ &&
            isAddForce_ == false)
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

                owner_->AddForce(direction, power_);

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

                owner_->AddForce(direction, power_);
            }

            isAddForce_ = true;
        }


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
            if (currentAnimationFrame >= animationSlowEndFrame_) return;

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
        owner_->SetWeight(1.0f);

        // �ϐ�������
        addForceFrame_ = 0.3f;
        isAddForce_ = false;

        Input::Instance().GetGamePad().Vibration(0.3f, 1.0f);
    }

    // ----- �X�V -----
    void CounterComboState::Update(const float& elapsedTime)
    {
        const float currentAnimationFrame = owner_->GetBlendAnimationSeconds();
        if (currentAnimationFrame > addForceFrame_ &&
            isAddForce_ == false)
        {
            // �O�����ɐi��
            const DirectX::XMFLOAT3 forwardVec = owner_->GetTransform()->CalcForward();
            owner_->AddForce(forwardVec, 0.7f);

            isAddForce_ = true;
        }

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

namespace PlayerState
{
    void ComboAttack0_0::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_1, false);
        owner_->SetWeight(1.0f);
    }

    void ComboAttack0_0::Update(const float& elapsedTime)
    {
        owner_->CheckNextInput(Player::NextInput::ComboAttack0);

        // �A�j���[�V�����Đ��I��
        if (owner_->IsPlayAnimation() == false)
        {
            switch (owner_->GetNextInput())
            {
            case Player::NextInput::Avoidance:
                owner_->ChangeState(Player::STATE::Avoidance);
                return;
            case Player::NextInput::ComboAttack0:
                owner_->ChangeState(Player::STATE::ComboAttack0_1);
                return;
            case Player::NextInput::ComboAttack1:
                owner_->ChangeState(Player::STATE::ComboAttack1_0);
                return;
            }

            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    void ComboAttack0_0::Finalize()
    {
    }
}

namespace PlayerState
{
    void ComboAttack0_1::Initialize()
    {
        owner_->PlayBlendAnimation(Player::Animation::ComboAttack0_2, false);
        owner_->SetWeight(1.0f);
    }

    void ComboAttack0_1::Update(const float& elapsedTime)
    {
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    void ComboAttack0_1::Finalize()
    {
    }
}

namespace PlayerState
{
    void ComboAttack0_2::Initialize()
    {
    }

    void ComboAttack0_2::Update(const float& elapsedTime)
    {
    }

    void ComboAttack0_2::Finalize()
    {
    }
}

namespace PlayerState
{
    void ComboAttack0_3::Initialize()
    {
    }

    void ComboAttack0_3::Update(const float& elapsedTime)
    {
    }

    void ComboAttack0_3::Finalize()
    {
    }
}

namespace PlayerState
{
    void ComboAttack1_0::Initialize()
    {
    }

    void ComboAttack1_0::Update(const float& elapsedTime)
    {
    }

    void ComboAttack1_0::Finalize()
    {
    }
}

namespace PlayerState
{
    void ComboAttack1_1::Initialize()
    {
    }

    void ComboAttack1_1::Update(const float& elapsedTime)
    {
    }

    void ComboAttack1_1::Finalize()
    {
    }
}

namespace PlayerState
{
    void ComboAttack1_2::Initialize()
    {
    }

    void ComboAttack1_2::Update(const float& elapsedTime)
    {
    }

    void ComboAttack1_2::Finalize()
    {
    }
}