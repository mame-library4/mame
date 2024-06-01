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
        owner_->SetWeight(1.0f);
        owner_->PlayBlendAnimation(Player::Animation::Walk, Player::Animation::Idle, true);
    }

    // ----- �X�V -----
    void IdleState::Update(const float& elapsedTime)
    {
        owner_->AddWeight(elapsedTime * 4.0f);

        // �U�����͎�t ( �X�e�[�g���ύX���ꂽ�ꍇ�����ŏI�� )
        if (owner_->CheckAttackButton(Player::NextInput::None)) return;

        GamePad gamePad = Input::Instance().GetGamePad();
        float aLX = fabs(gamePad.GetAxisLX());
        float aLY = fabs(gamePad.GetAxisLY());
        if (aLX != 0.0f && aLY != 0.0f)
        {
            owner_->ChangeState(Player::STATE::Move);
            return;
        }        

        // �X�e�[�g��؂�ւ��邩����
        //int isChangeState = isInputMove();
        //if (isChangeState)
        //{
        //    // �l���P�̏ꍇ ���� �Ȃ̂� WalkState �֑J��
        //    if (isChangeState == 1)
        //    {
        //        owner_->ChangeState(Player::STATE::Walk);
        //        return;
        //    }

        //    // �l���Q�̏ꍇ ���� �Ȃ̂� RunState �֑J��
        //    if (isChangeState == 2)
        //    {
        //        owner_->ChangeState(Player::STATE::Run);
        //        return;
        //    }
        //}

        // ���x�v�Z
        DirectX::XMFLOAT3 velocity = owner_->GetVelocity();

        velocity.x = std::max(0.0f, velocity.x - 5.0f * elapsedTime);
        velocity.z = std::max(0.0f, velocity.z - 5.0f * elapsedTime);

        owner_->SetVelocity(velocity);




        // ��莞�ԑ��삪�Ȃ��Ɠ����o�������Ƃ�����肽��

        // ------------------------------------------
    }

    // ----- �I���� -----
    void IdleState::Finalize()
    {
    }

    // ------ ���͒l�ɉ����ĕԂ��l��ς��� -----
    int IdleState::isInputMove()
    {
        GamePad gamePad = Input::Instance().GetGamePad();

        float aLX = fabs(gamePad.GetAxisLX());
        float aLY = fabs(gamePad.GetAxisLY());

        // ���͒l�� 0.5�ȏゾ�����瑖��̏�ԂȂ̂� �Q ��Ԃ�
        if (aLX > 0.5f || aLY > 0.5f) return 2;

        // ���͒l�� 0 �ȏゾ����������̏�ԂȂ̂� �P ��Ԃ�
        if (aLX > 0.0f || aLY > 0.0f) return 1;

        // ���͒l�Ȃ�
        return 0;
    }

}

// ---------- �ړ� ----------
#pragma region �ړ�

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
        // �U�����͎�t ( �X�e�[�g���ύX���ꂽ�ꍇ�����ŏI�� )
        if (owner_->CheckAttackButton(Player::NextInput::None)) return;

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

#if 0
// ----- ���� -----
namespace PlayerState
{
    // ----- ������ -----
    void WalkState::Initialize()
    {
        owner_->PlayBlendAnimation(Player::Animation::Walk, true);
        owner_->SetWeight(1.0f);
    }

    // ----- �X�V -----
    void WalkState::Update(const float& elapsedTime)
    {
        // �X�e�[�g��؂�ւ��邩����
        int isChangeState = isInputMove();
        // �l���O�̏ꍇ���͂��Ȃ��̂ŁA�ړ����x���Ȃ��ꍇ IdleState �֑J��
        if (isChangeState == 0)
        {
            // if( moveSpeed <= 0.0f ) owner_->ChangeState(Player::State::Idle); return;

        }
        // �l���Q�̏ꍇ ���� �Ȃ̂� RunState �֑J��
        if (isChangeState == 2)
        {
            owner_->ChangeState(Player::STATE::Run);
            return;
        }

        // ��U��
        if (owner_->GetLightAttackKeyDown())
        {
            owner_->ChangeState(Player::STATE::LightAttack0);
            return;
        }

        GamePad& gamePad = Input::Instance().GetGamePad();
        float aLX = gamePad.GetAxisLX();
        float aLY = gamePad.GetAxisLY();

        // ��]
        DirectX::XMFLOAT2 input = { fabs(gamePad.GetAxisLX()), fabs(gamePad.GetAxisLY()) };
        DirectX::XMFLOAT3 cameraFront = Camera::Instance().CalcForward();
        DirectX::XMFLOAT3 cameraRight = Camera::Instance().CalcRight();

        DirectX::XMFLOAT3 direction =
        {
            aLY * cameraFront.x + aLX * cameraRight.x,
            0,
            aLY * cameraFront.z + aLX * cameraRight.z,
        };
        direction = XMFloat3Normalize(direction);
        if (input.x > 0.0f || input.y > 0.0f)
        {


            // �J�����̑O�����ƃv���C���[�̑O�������擾
#if 0
            DirectX::XMFLOAT2 cameraForward = { Camera::Instance().CalcForward().x ,Camera::Instance().CalcForward().z };
            cameraForward = XMFloat2Normalize(cameraForward);
#else 
            DirectX::XMFLOAT2 cameraForward = { direction.x, direction.z };
            cameraForward = XMFloat2Normalize(cameraForward);
#endif

            DirectX::XMFLOAT2 playerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
            playerForward = XMFloat2Normalize(playerForward);

            // �O�ς����Ăǂ���ɉ�]����̂��𔻒肷��
            float forwardCorss = XMFloat2Cross(cameraForward, playerForward);

            // ���ςŉ�]�����Z�o
            float forwardDot = XMFloat2Dot(cameraForward, playerForward) - 1.0f;

            if (forwardCorss > 0)
            {
                owner_->GetTransform()->AddRotationY(forwardDot);
            }
            else
            {
                owner_->GetTransform()->AddRotationY(-forwardDot);
            }
        }

        // ���x�v�Z
        DirectX::XMFLOAT3 velocity = owner_->GetVelocity();

        velocity.x = std::min(owner_->GetMaxSpeed(), velocity.x * elapsedTime);
        velocity.z = std::min(owner_->GetMaxSpeed(), velocity.z * elapsedTime);

        owner_->SetVelocity(velocity);
        owner_->GetTransform()->AddPosition(direction * elapsedTime * 2.0f);

        aLX = fabs(gamePad.GetAxisLX());
        aLY = fabs(gamePad.GetAxisLY());

        if (aLX <= 0 && aLY <= 0)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

    }

    // ----- �I���� -----
    void WalkState::Finalize()
    {
    }

    // ----- ���͒l�ɉ����ĕԂ��l��ς��� -----
    int WalkState::isInputMove()
    {
        GamePad gamePad = Input::Instance().GetGamePad();

        float aLX = fabs(gamePad.GetAxisLX());
        float aLY = fabs(gamePad.GetAxisLY());

        // ���͒l�� 0.5�ȏゾ�����瑖��̏�ԂȂ̂� �Q ��Ԃ�
        if (aLX > 0.5f || aLY > 0.5f) return 2;

        // ���͒l�� 0 �ȏゾ����������̏�ԂȂ̂� �P ��Ԃ�
        if (aLX > 0.0f || aLY > 0.0f) return 1;

        // ���͒l�Ȃ�
        return 0;
    }
}

// ----- ���� -----
namespace PlayerState
{
    // ----- ������ -----
    void RunState::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::Run, true);
        owner_->SetWeight(0.0f);

        DirectX::XMFLOAT3 velocity = owner_->GetVelocity();

        velocity.x = 1.5f;
        velocity.z = 1.5f;

        owner_->SetVelocity(velocity);
    }

    // ----- �X�V -----
    void RunState::Update(const float& elapsedTime)
    {
        //owner_->AddWeight(elapsedTime);

        // ��U��
        if (owner_->GetLightAttackKeyDown())
        {
            owner_->ChangeState(Player::STATE::LightAttack0);
            return;
        }

        // ���U��
        if (owner_->GetStrongAttackKeyDown())
        {
            owner_->ChangeState(Player::STATE::StrongAttack0);
            return;
        }

        GamePad& gamePad = Input::Instance().GetGamePad();
        float aLX = gamePad.GetAxisLX();
        float aLY = gamePad.GetAxisLY();

        // ��]
        DirectX::XMFLOAT2 input = { fabs(gamePad.GetAxisLX()), fabs(gamePad.GetAxisLY()) };
        DirectX::XMFLOAT3 cameraFront = Camera::Instance().CalcForward();
        DirectX::XMFLOAT3 cameraRight = Camera::Instance().CalcRight();

        DirectX::XMFLOAT3 direction =
        {
            aLY * cameraFront.x + aLX * cameraRight.x,
            0,
            aLY * cameraFront.z + aLX * cameraRight.z,
        };
        direction = XMFloat3Normalize(direction);
        if (input.x > 0.0f || input.y > 0.0f)
        {


            // �J�����̑O�����ƃv���C���[�̑O�������擾
#if 0
            DirectX::XMFLOAT2 cameraForward = { Camera::Instance().CalcForward().x ,Camera::Instance().CalcForward().z };
            cameraForward = XMFloat2Normalize(cameraForward);
#else 
            DirectX::XMFLOAT2 cameraForward = { direction.x, direction.z };
            cameraForward = XMFloat2Normalize(cameraForward);
#endif

            DirectX::XMFLOAT2 playerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
            playerForward = XMFloat2Normalize(playerForward);

            // �O�ς����Ăǂ���ɉ�]����̂��𔻒肷��
            float forwardCorss = XMFloat2Cross(cameraForward, playerForward);

            // ���ςŉ�]�����Z�o
            float forwardDot = XMFloat2Dot(cameraForward, playerForward) - 1.0f;

            if (forwardCorss > 0)
            {
                owner_->GetTransform()->AddRotationY(forwardDot);
            }
            else
            {
                owner_->GetTransform()->AddRotationY(-forwardDot);
            }
        }

        // ���x�v�Z
        DirectX::XMFLOAT3 velocity = owner_->GetVelocity();
        const float speed = owner_->GetSpeed() * elapsedTime;
        const float maxSpeed = owner_->GetMaxSpeed();

        float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
        const float weight = std::min(1.0f, length / maxSpeed);
        owner_->SetWeight(weight);

        velocity.x += direction.x * speed;
        velocity.z += direction.z * speed;
        if (fabs(velocity.x) > maxSpeed)
        {
            velocity.x = (velocity.x > 0) ? maxSpeed : -maxSpeed;
        }
        if (fabs(velocity.z) > maxSpeed)
        {
            velocity.z = (velocity.z > 0) ? maxSpeed : -maxSpeed;
        }


        owner_->SetVelocity(velocity);
        owner_->GetTransform()->AddPosition(velocity);

        aLX = fabs(gamePad.GetAxisLX());
        aLY = fabs(gamePad.GetAxisLY());

        if (aLX <= 0 && aLY <= 0)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void RunState::Finalize()
    {
    }
}

#endif

#pragma endregion �ړ�

// ---------- ��� ----------
#pragma region ���

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
        return;
        // �A�j���[�V�����ݒ�
        //owner_->PlayAnimation(Player::Animation::Protect, false);
    }

    // ----- �X�V -----
    void CounterState::Update(const float& elapsedTime)
    {
        owner_->ChangeState(Player::STATE::Idle);
        return;
        // �A�j���[�V�����Đ��I��
        if (!owner_->IsPlayAnimation())
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

        // �J�E���^�[����
        // TODO:��������B�J�E���^�[
        if (GetAsyncKeyState('B') & 1)
        {
            owner_->ChangeState(Player::STATE::CounterAttack);
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
    void CounterAttackState::Initialize()
    {
        owner_->PlayAnimation(Player::Animation::StrongAttack0, false);
    }

    // ----- �X�V -----
    void CounterAttackState::Update(const float& elapsedTime)
    {
        // �A�j���[�V�����I��
        if (!owner_->IsPlayAnimation())
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void CounterAttackState::Finalize()
    {
    }
}
#pragma endregion ���

// ---------- ��U�� ----------
#pragma region ��U��

// ----- ��U���O -----
namespace PlayerState
{
    // ----- ������ -----
    void LightAttack0State::Initialize()
    {
        owner_->PlayBlendAnimation(Player::Animation::LightAttack0, false);
        owner_->SetWeight(1.0f);

        // �ړ����x�����Z�b�g����
        owner_->SetVelocity({});

        // �U������L����
        owner_->SetAttackFlag();

        // �t���O�����Z�b�g����
        owner_->ResetFlags();
    }

    // ----- �X�V -----
    void LightAttack0State::Update(const float& elapsedTime)
    {
        // ��s���͎�t
        owner_->CheckAttackButton(Player::NextInput::LightAttack);

        // ��s���͂�����ꍇ�A���݂̍U���t���[�����I��������ɃX�e�[�g��؂�ւ���
        if (owner_->GetBlendAnimationSeconds() > comboAttackFrame_)
        {
            // �U������𖳌�������
            if (owner_->GetIsActiveAttackFlag() == true)
            {
                owner_->SetAttackFlag(false);
            }

            if (owner_->GetNextInput() == static_cast<int>(Player::NextInput::LightAttack))
            {
                owner_->ChangeState(Player::STATE::LightAttack1);
                return;
            }
            if (owner_->GetNextInput() == static_cast<int>(Player::NextInput::StrongAttack))
            {
                owner_->ChangeState(Player::STATE::StrongAttack0);
                return;
            }
        }

        // �A�j���[�V�������I��������ҋ@�X�e�[�g�ɐ؂�ւ���
        if (owner_->IsPlayAnimation() == false)
        //if(owner_->GetBlendAnimationSeconds() > animationEndFrame_)
        {
            owner_->ChangeState(Player::STATE::Idle);
        }
    }

    // ----- �I���� -----
    void LightAttack0State::Finalize()
    {
    }
}

// ----- ��U���P -----
namespace PlayerState
{
    // ----- ������ -----
    void LightAttack1State::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::LightAttack1, false);
        owner_->SetWeight(1.0f);

        // �U������L����
        owner_->SetAttackFlag();

        // �t���O�����Z�b�g����
        owner_->ResetFlags();
    }

    // ----- �X�V -----
    void LightAttack1State::Update(const float& elapsedTime)
    {
        // ��s���͎�t
        owner_->CheckAttackButton(Player::NextInput::LightAttack);

        // ��s���͂�����ꍇ�A���݂̍U���t���[�����I��������ɃX�e�[�g��؂�ւ���
        if (owner_->GetBlendAnimationSeconds() > comboAttackFrame_)
        {
            // �U������𖳌�������
            if (owner_->GetIsActiveAttackFlag() == true)
            {
                owner_->SetAttackFlag(false);
            }

            if (owner_->GetNextInput() == static_cast<int>(Player::NextInput::LightAttack))
            {
                owner_->ChangeState(Player::STATE::LightAttack2);
                return;
            }
            if (owner_->GetNextInput() == static_cast<int>(Player::NextInput::StrongAttack))
            {
                owner_->ChangeState(Player::STATE::StrongAttack0);
                return;
            }
        }

        // �A�j���[�V�������I��������ҋ@�X�e�[�g�ɐ؂�ւ���
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
        }
    }

    // ----- �I���� -----
    void LightAttack1State::Finalize()
    {
    }
}

// ----- ��U���Q -----
namespace PlayerState
{
    // ----- ������ -----
    void LightAttack2State::Initialize()
    {
        owner_->PlayBlendAnimation(Player::Animation::LightAttack2, false);
        owner_->SetWeight(1.0f);

        // �U������L����
        owner_->SetAttackFlag();

        // �t���O�����Z�b�g����
        owner_->ResetFlags();
    }

    // ----- �X�V -----
    void LightAttack2State::Update(const float& elapsedTime)
    {
        // �A�j���[�V�������I��������ҋ@�X�e�[�g�ɐ؂�ւ���
        if (owner_->IsPlayAnimation() == false)
        {
            // �U������𖳌�������
            if (owner_->GetIsActiveAttackFlag() == true)
            {
                owner_->SetAttackFlag(false);
            }

            owner_->ChangeState(Player::STATE::Idle);
        }
    }

    // ----- �I���� -----
    void LightAttack2State::Finalize()
    {
    }
}

#pragma endregion ��U��

// ---------- ���U�� ----------
#pragma region ���U��

// ----- ���U���O -----
namespace PlayerState
{
    // ----- ������ -----
    void StrongAttack0State::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::StrongAttack0, false);
        owner_->SetWeight(1.0f);

        // �ړ����x�����Z�b�g����
        owner_->SetVelocity({});

        // �U������L����
        owner_->SetAttackFlag();

        // �t���O�����Z�b�g����
        owner_->ResetFlags();
    }

    // ----- �X�V -----
    void StrongAttack0State::Update(const float& elapsedTime)
    {
        // ��s���͎�t
        owner_->CheckAttackButton(Player::NextInput::StrongAttack);

        // ��s���͂�����ꍇ�A���݂̍U���t���[�����I��������ɃX�e�[�g��؂�ւ���
        if (owner_->GetNextInput() == static_cast<int>(Player::NextInput::StrongAttack) &&
            owner_->GetBlendAnimationSeconds() > comboAttackFrame_)
        {
            // �U������𖳌�������
            if (owner_->GetIsActiveAttackFlag() == true)
            {
                owner_->SetAttackFlag(false);
            }

            owner_->ChangeState(Player::STATE::StrongAttack1);
            return;
        }

        // �A�j���[�V�������I��������ҋ@�X�e�[�g�ɐ؂�ւ���
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
        }
    }

    // ----- �I���� -----
    void StrongAttack0State::Finalize()
    {
    }
}

// ----- ���U���P -----
namespace PlayerState
{
    // ----- ������ -----
    void StrongAttack1State::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::StrongAttack1, false);
        owner_->SetWeight(1.0f);

        // �U������L����
        owner_->SetAttackFlag();

        // �t���O�����Z�b�g����
        owner_->ResetFlags();
    }

    // ----- �X�V -----
    void StrongAttack1State::Update(const float& elapsedTime)
    {
        // �A�j���[�V�������I��������ҋ@�X�e�[�g�ɐ؂�ւ���
        if (owner_->IsPlayAnimation() == false)
        {
            // �U������𖳌�������
            if (owner_->GetIsActiveAttackFlag() == true)
            {
                owner_->SetAttackFlag(false);
            }

            owner_->ChangeState(Player::STATE::Idle);
        }
    }

    // ----- �I���� -----
    void StrongAttack1State::Finalize()
    {
    }
}

#pragma endregion ���U��

// ----- �_���[�W -----
namespace PlayerState
{
    // ----- ������ -----
    void DamageState::Initialize()
    {
        // �A�j���[�V�����Đ� 
        owner_->PlayAnimation(Player::Animation::Damage1, false);
        //owner_->PlayAnimation(Player::Animation::Damage0, false);
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