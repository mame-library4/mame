#include "PlayerState.h"
#include <cmath>
#include "Input.h"
#include "Camera.h"
#include "Easing.h"
#include "MathHelper.h"
#include "../Enemy/EnemyManager.h"
#include "Effect/EffectManager.h"

#include "UI/UIFader.h"

// ----- AddForceData -----
namespace PlayerState
{
    // ----- ������ -----
    void AddForceData::Initialize(const float& addForceFrame, const float& force, const float& decelerationForce)
    {
        addForceFrame_ = addForceFrame;
        force_ = force * 60.0f;
        decelerationForce_ = decelerationForce * 60.0f;
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
    }

    // ----- �X�V -----
    bool AttackData::Update(const float& animationFrame, const bool& flag)
    {
        // ���ɍU�����������Ă���
        if (flag) return false;

        // �U���X�^�[�g�t���[���ɒB���Ă��Ȃ��̂ŏI��
        if (animationFrame < attackStartFrame_) return false;

        // �U���G���h�t���[�����z���Ă���̂ł����ŏI��
        if (animationFrame > attackEndFrame_) return false;

        return true;
    }
}

// ----- GamePadVibration -----
namespace PlayerState
{
    // ----- ������ -----
    void GamePadVibration::Initialize(const float& startFrame, const float& time, const float& power)
    {
        startFrame_ = startFrame;
        time_ = time;
        power_ = power;
        isVibraion_ = false;
    }

    // ----- �X�V -----
    const bool GamePadVibration::Update(const float& animationFrame)
    {
        // ���ɐU�������Ă���
        if (isVibraion_) return false;

        // ���݂̃A�j���[�V�����̃t���[�����X�^�[�g�t���[���܂ŒB���Ă��Ȃ�
        if (animationFrame < startFrame_) return false;

        // �R���g���[���[��U��������
        Input::Instance().GetGamePad().Vibration(time_, power_);
        isVibraion_ = true;

        return true;
    }
}

// ----- �ҋ@ -----
namespace PlayerState
{
    // ----- ������ -----
    void IdleState::Initialize()
    {
        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �A�j���[�V�����ݒ�
        SetAnimation();

        // ��s���͐ݒ�
        owner_->SetNextInputStartFrame();
        owner_->SetNextInputEndFrame();
        owner_->SetNextInputTransitionFrame();

        // ����UI�ݒ�
        if(UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide) !=nullptr)
            UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 0.0f);
    }

    // ----- �X�V -----
    void IdleState::Update(const float& elapsedTime)
    {
        if (owner_->IsGuardCounterKeyDown())
        {
            owner_->ChangeState(Player::STATE::GuardCounter);
            return;
        }

        // ��s���͔���
        if (CheckNextInput()) return;
    }

    // ----- �I���� -----
    void IdleState::Finalize()
    {
    }

    // ----- ImGui�p -----
    void IdleState::DrawDebug()
    {
        if (ImGui::TreeNode(GetName()))
        {
            float a;
            ImGui::DragFloat("a", &a);

            ImGui::TreePop();
        }
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

        if (animationIndex == Player::Animation::Counter)
        {
            owner_->SetTransitionTime(0.3f);
        }
        else if (animationIndex == Player::Animation::RollFront ||
            animationIndex == Player::Animation::RollBack ||
            animationIndex == Player::Animation::RollRight ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->SetTransitionTime(0.15f);
        }
        else
        {
            owner_->SetTransitionTime(0.15f);
        }
        owner_->PlayBlendAnimation(Player::Animation::Idle, true);
    }

    // ----- ��s���͔��� -----
    const bool IdleState::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

#pragma region ----- ��s���͎�t -----
        // ����s���͎�t
        if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
            animationSeconds <= owner_->GetDodgeInputEndFrame())
        {
            if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
        }

        // �U����s���͎�t
        if (animationSeconds >= owner_->GetAttackInputStartFrame() &&
            animationSeconds <= owner_->GetAttackInputEndFrame())
        {
            if (owner_->IsComboAttack0KeyDown()) owner_->SetNextInput(Player::NextInput::ComboAttack0);
        }

        // �J�E���^�[��s���͎�t
        if (animationSeconds >= owner_->GetCounterInputStartFrame() &&
            animationSeconds <= owner_->GetCounterInputEndFrame())
        {
            if (owner_->IsCounterStanceKey()) owner_->SetNextInput(Player::NextInput::Counter);
        }

#pragma endregion ----- ��s���͎�t -----

#pragma region ----- �J�ڃ`�F�b�N -----
        // ���J�ڃ`�F�b�N
        if (owner_->GetNextInput() == Player::NextInput::Dodge)
        {
            if (animationSeconds >= owner_->GetDodgeTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Dodge);
                return true;
            }
        }
        // �U���J�ڃ`�F�b�N
        else if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
        {
            if (animationSeconds >= owner_->GetAttackTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_0);
                return true;
            }
        }
        // �J�E���^�[�J�ڃ`�F�b�N
        else if (owner_->GetNextInput() == Player::NextInput::Counter)
        {
            if (animationSeconds >= owner_->GetCounterTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Counter);
                return true;
            }
        }

#pragma endregion ----- �J�ڃ`�F�b�N -----

        // �ړ����͔���
        if (animationSeconds >= owner_->GetMoveInputStartFrame())
        {
            // �X�e�B�b�N���͂����邩
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (aLx == 0.0f && aLy == 0.0f) return false;

            owner_->ChangeState(Player::STATE::Run);
            return true;
        }

        return false;
    }
}

// ----- ���� -----
namespace PlayerState
{
    // ----- ������ -----
    void RunState::Initialize()
    {
        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �A�j���[�V�����ݒ�
        SetAnimation();

        // �ő呬�x��ݒ�
        owner_->SetMaxSpeed(5.0f);

        // ��s���͐ݒ�
        owner_->SetNextInputStartFrame();
        owner_->SetNextInputEndFrame();
        owner_->SetNextInputTransitionFrame();

        // ����UI�ݒ�
        if (UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide) != nullptr)
            UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 0.0f);

        // �ϐ�������
        changeStateTimer_ = 0.0f;
    }

    // ----- �X�V -----
    void RunState::Update(const float& elapsedTime)
    {
        if (owner_->IsGuardCounterKeyDown())
        {
            owner_->ChangeState(Player::STATE::GuardCounter);
            return;
        }

        // ��s���͔���
        if (CheckNextInput()) return;

        // ����
        owner_->Turn(elapsedTime);

        // �^�C�}�[���Z
        changeStateTimer_ += elapsedTime;

        // �_�b�V������
        UpdateDash(elapsedTime);

        // ����UI�ݒ�
        if(owner_->GetIsDash()) UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(750.0f, 0.0f);
        else UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 0.0f);
    }
    
    // ----- �I���� -----
    void RunState::Finalize()
    {
        owner_->SetMoveDirection({});
        owner_->SetVelocity({});

        owner_->SetIsDash(false);
    }

    // ----- ImGui�p -----
    void RunState::DrawDebug()
    {
        if (ImGui::BeginMenu(GetName()))
        {


            ImGui::EndMenu();
        }
    }

    // ----- �A�j���[�V�����ݒ� -----
    void RunState::SetAnimation()
    {
        // �ړ��L�����Z���̏ꍇ�f�����A�j���[�V������؂�ւ���
        if (owner_->GetIsMoveAttackCancel())
        {
            owner_->SetTransitionTime(0.15f);
            owner_->PlayBlendAnimation(Player::Animation::Run, true);
            owner_->SetIsMoveAttackCancel(false);
            return;
        }

        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        if (animationIndex == Player::Animation::RollFront ||
            animationIndex == Player::Animation::RollBack    ||
            animationIndex == Player::Animation::RollRight   ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->SetTransitionTime(0.2f);
        }
        else if (animationIndex == Player::Animation::Attack0_0 ||
            animationIndex == Player::Animation::Attack0_1 ||
            animationIndex == Player::Animation::RunAttack1)
        {
            owner_->SetTransitionTime(0.3f);
        }
        else if (animationIndex == Player::Animation::Attack0_2)
        {
            owner_->SetTransitionTime(0.3f);
        }
        else
        {
            owner_->SetTransitionTime(0.15f);
        }

        owner_->PlayBlendAnimation(Player::Animation::Run, true);
    }

    // ----- ��s���͔��� -----
    const bool RunState::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

#pragma region ----- ��s���͎�t -----
        // ����s���͎�t
        if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
            animationSeconds <= owner_->GetDodgeInputEndFrame())
        {
            if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
        }
        // �U����s���͎�t
        if (animationSeconds >= owner_->GetAttackInputStartFrame() &&
            animationSeconds <= owner_->GetAttackInputEndFrame())
        {
            if (owner_->IsComboAttack0KeyDown()) owner_->SetNextInput(Player::NextInput::ComboAttack0);
        }

#pragma endregion ----- ��s���͎�t -----

#pragma region ----- �J�ڃ`�F�b�N -----
        // ���J�ڃ`�F�b�N
        if (owner_->GetNextInput() == Player::NextInput::Dodge)
        {
            if (animationSeconds >= owner_->GetDodgeTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Dodge);
                return true;
            }
        }
        // �U���J�ڃ`�F�b�N
        else if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
        {
            if (animationSeconds >= owner_->GetAttackTransitionFrame())
            {
                // �_�b�V�����Ă���Ƃ��͑���U���ɑJ�ڂ���
                if (owner_->GetIsDash()) owner_->ChangeState(Player::STATE::RunAttack);
                // �ʏ�̑���̏ꍇ�̓R���{�U���O�ɑJ�ڂ���
                else owner_->ChangeState(Player::STATE::ComboAttack0_0);

                return true;
            }
        }

#pragma endregion ----- �J�ڃ`�F�b�N -----

        // �ړ����͔���
        if (animationSeconds >= owner_->GetMoveInputStartFrame())
        {
            // ����->�ҋ@ �̑J�ڐ���
            if (changeStateTimer_ <= 0.2f) return false;

            // �X�e�B�b�N���͂����邩
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (aLx == 0.0f && aLy == 0.0f)
            {
                owner_->ChangeState(Player::STATE::Idle);
                return true;
            }
        }

        return false;
    }

    // ----- �_�b�V���̏��� -----
    void RunState::UpdateDash(const float& elapsedTime)
    {
        const bool isDashKey = owner_->IsDashKey();

        // �X�^�~�i����������ꍇ���x��x������
        if(owner_->GetIsStaminaDepleted())
        {
            owner_->SetAnimationSpeed(0.8f);

            // �ő呬�x��ݒ�
            owner_->SetMaxSpeed(3.0f);

            owner_->UseDashStamina(elapsedTime);

            // �_�b�V�����Ă��邩�ݒ�
            owner_->SetIsDash(isDashKey);

            return;
        }

        // �_�b�V���̏���
        if (isDashKey)
        {
            owner_->SetAnimationSpeed(owner_->GetDashAnimationSpeed());

            // �ő呬�x��ݒ�
            owner_->SetMaxSpeed(owner_->GetDashSpeed());

            owner_->SetIsDash(true);

            owner_->UseDashStamina(elapsedTime);
        }
        else
        {
            owner_->SetAnimationSpeed(1.0f);

            // �ő呬�x��ݒ�
            owner_->SetMaxSpeed(5.0f);

            owner_->SetIsDash(false);
        }

    }
}

// ----- �K�[�h�J�E���^�[ -----
namespace PlayerState
{
    // ----- ������ ----
    void GuardCounterState::Initialize()
    {
        // �A�j���[�V�����ݒ�
        owner_->PlayUpperLowerBodyAnimation(static_cast<int>(Player::Animation::BlockLoop), false);

        DirectX::XMFLOAT3 position = owner_->GetJointPosition("pelvis");
        guardEffect_ = EffectManager::Instance().GetEffect("Guard")->Play(position, 1.0f, 1.0f);

        owner_->SetIsGuardCounterStance(true);
        owner_->SetIsGuardCounterSuccessful(false); // ���Z�b�g����


        // �ő呬�x��ݒ�
        //owner_->SetMaxSpeed(2.0f);
        owner_->SetMaxSpeed(5.0f);

        // �ϐ�������
        gamePadVibration_.Initialize(0.0f, 0.2f, 0.5f);
        guardEffectLerpTimer_       = 0.0f;
    }

    // ----- �X�V -----
    void GuardCounterState::Update(const float& elapsedTime)
    {
        // �ړ����͏���
        GamePad& gamePad = Input::Instance().GetGamePad();
        const float aLx = gamePad.GetAxisLX();
        const float aLy = gamePad.GetAxisLY();
        if (fabsf(aLx) != 0.0f || fabsf(aLy) != 0.0f)
        {
            owner_->ChangeLowerBodyAnimation(static_cast<int>(Player::Animation::Run));

            // ����
            owner_->Turn(elapsedTime);
        }
        else
        {
            owner_->ChangeLowerBodyAnimation(static_cast<int>(Player::Animation::Idle));

            // �ړ�&��]�������Z�b�g
            owner_->SetMoveDirection({});
            owner_->SetVelocity({});
        }


        // �A�j���[�V�����X�V
        UpdateAnimation();

        // �G�t�F�N�g�X�V
        UpdateEffect(elapsedTime);

        // �K�[�h�J�E���^�[�𖳌�������
        if (owner_->GetAnimationIndex() == static_cast<int>(Player::Animation::BlockEnd))
        {
            if (owner_->GetAnimationSeconds() > 0.24f && owner_->GetIsGuardCounterStance())
            {
                owner_->SetIsGuardCounterStance(false);
            }
        }

        // �J�E���^�[����
        if (owner_->GetIsGuardCounterSuccessful())
        {
            owner_->ChangeState(Player::STATE::GuardCounterAttack);

            // TODO:���ō���Ă�
            gamePadVibration_.Update(owner_->GetAnimationSeconds());

            return;
        }

    }

    // ----- �I���� -----
    void GuardCounterState::Finalize()
    {
        // �G�t�F�N�g���~������
        EffectManager::Instance().GetEffect("Guard")->Stop(guardEffect_);

        // �ړ�&��]�������Z�b�g
        owner_->SetMoveDirection({});
        owner_->SetVelocity({});
    }

    // ----- ImGui�p -----
    void GuardCounterState::DrawDebug()
    {
        ImGui::DragFloat("GuardEffectStartSize", &guardEffectStartSize_, 0.1f, 1.0f, 6.0f);
        ImGui::DragFloat("GuardEffectEndSize",   &guardEffectEndSize_,   0.1f, 1.0f, 6.0f);
        ImGui::DragFloat("LerpTimer", &guardEffectLerpTimer_);
        ImGui::DragFloat("LerpSpeed", &guardEffectLerpSpeed_);
    }

    // ----- �A�j���[�V�����X�V -----
    void GuardCounterState::UpdateAnimation()
    {
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �G�t�F�N�g�X�V -----
    void GuardCounterState::UpdateEffect(const float& elapsedTime)
    {
        // �K�[�h���I����Ă�����X�V���Ȃ�
        //if (isGuardLoopAnimationEnd_) return;

        Effect* guardEffect = EffectManager::Instance().GetEffect("Guard");
        DirectX::XMFLOAT3 pelvisPosition = owner_->GetJointPosition("pelvis");

        guardEffect->SetPosition(guardEffect_, pelvisPosition);

        // -----------------------------------
        //  �@�G�t�F�N�g�̃T�C�Y���X�V����
        // �@���łɔ���p�̕ϐ����X�V���Ă���
        // -----------------------------------
        guardEffectLerpTimer_ += guardEffectLerpSpeed_ * elapsedTime;
        guardEffectLerpTimer_ = std::min(guardEffectLerpTimer_, 1.0f);
        const float guardEffectSize = XMFloatLerp(guardEffectStartSize_, guardEffectEndSize_, guardEffectLerpTimer_);
        guardEffect->SetScale(guardEffect_, guardEffectSize);

        float guardCounterRadius = XMFloatLerp(guardCounterStartRadius_, guardCounterEndRadius_, guardEffectLerpTimer_);
        owner_->SetGuardCounterRadius(guardCounterRadius);
    }

}

// ----- �K�[�h�J�E���^�[�U�� -----
namespace PlayerState
{
    // ----- ������ -----
    void GuardCounterAttackState::Initialize()
    {
        // �A�j���[�V�����ݒ�    
        //owner_->PlayBlendAnimation(Player::Animation::CounterAttack0, false, 1.0f, 0.35f);
        owner_->PlayBlendAnimation(Player::Animation::CounterAttack0, false, 1.0f, 0.5f);
        owner_->SetTransitionTime(0.1f);

        owner_->SetIsDrawSwordTrail(true);
    }

    // ----- �X�V -----
    void GuardCounterAttackState::Update(const float& elapsedTime)
    {
        // RootMotion�̐ݒ�
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotion���g�p����
            owner_->SetUseRootMotion(true);
        }

        // ���̋O�Ղ̐ݒ�
        if (owner_->GetAnimationSeconds() > 1.0f && owner_->GetIsDrawSwordTrail())
        {
            owner_->SetIsDrawSwordTrail(false);
        }

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void GuardCounterAttackState::Finalize()
    {
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui�p -----
    void GuardCounterAttackState::DrawDebug()
    {
    }
}

// ----- �㋯�� -----
namespace PlayerState
{
    // ----- ������ -----
    void LightFlinchState::Initialize()
    {
        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        owner_->PlayBlendAnimation(Player::Animation::DownStart, false);
    }

    // ----- �X�V -----
    void LightFlinchState::Update(const float& elapsedTime)
    {
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void LightFlinchState::Finalize()
    {
    }
    void LightFlinchState::DrawDebug()
    {
    }
}

// ----- ���� -----
namespace PlayerState
{
    // ----- ������ -----
    void FlinchState::Initialize()
    {
        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::DownStart, false, 2.0f);

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
                owner_->PlayAnimation(Player::Animation::DownLoop, true);
                state_ = 1;
            }

            break;
        case 1:
            
            if (EnemyManager::Instance().GetEnemy(0)->GetActiveNodeName() != "Roar")
            {
                owner_->PlayBlendAnimation(Player::Animation::DownEnd, false);
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
    void FlinchState::DrawDebug()
    {
    }
}

// ----- �_���[�W -----
namespace PlayerState
{
    // ----- ������ -----
    void DamageState::Initialize()
    {
        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �A�j���[�V�����Đ� 
        owner_->PlayAnimation(Player::Animation::Damage, false, 1.2f);

        // ���G��Ԃɂ���
        owner_->SetIsInvincible(true);

        // ����UI�ݒ�
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(1500.0f, 700.0f);

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

        isCameraShakeActive_ = false;
    }

    // ----- �X�V -----
    void DamageState::Update(const float& elapsedTime)
    {
        // �A�j���[�V�����̑��x�ݒ�
        SetAnimationSpeed();

        // �J�����V�F�C�N�����������
        if (isCameraShakeActive_ == false)
        {
            if (owner_->GetAnimationSeconds() > 0.15f)
            {
                Camera::Instance().ScreenVibrate(0.1f, 0.3f);
                isCameraShakeActive_ = true;
            }
        }


        // �ړ��l
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(addForceDirection_, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // ������΂���A�j���[�V��������
        if (owner_->GetAnimationSeconds() > 1.2f && isFirstAnimation_ == true)
        {            
            // ���͂�����Γ|��Ă��Ԃ��I������
            if (owner_->IsGetUpKeyDown())
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
                if (owner_->IsDodgeKeyDown())
                {
                    owner_->ChangeState(Player::STATE::Dodge);
                    return;
                }
            }
        }

    }

    // ----- �I���� -----
    void DamageState::Finalize()
    {
        // ���G��Ԃ���������
        owner_->SetIsInvincible(false);
    }

    void DamageState::DrawDebug()
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
        DirectX::XMFLOAT2 ownerFront = XMFloat2Normalize({ owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z });
        DirectX::XMFLOAT2 addForceDirection = XMFloat2Normalize(DirectX::XMFLOAT2(addForceDirection_.x, addForceDirection_.z) * -1.0f);
        
        float cross = XMFloat2Cross(addForceDirection, ownerFront);
        float angle = acosf(XMFloat2Dot(addForceDirection, ownerFront));

        if (cross > 0)
        {
            owner_->GetTransform()->SetRotationY(-angle);
        }
        else
        {
            owner_->GetTransform()->SetRotationY(angle);
        }
    }
}

// ----- ���S -----
namespace PlayerState
{
    // ----- ������ -----
    void DeathState::Initialize()
    {
        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        owner_->PlayBlendAnimation(Player::Animation::Damage, false, 1.0f, 0.2f);
        owner_->SetTransitionTime(0.3f);

        // ���S�����̂Ŗ��G��Ԃɂ���
        owner_->SetIsInvincible(true);

        // ���S�J�������g�p����
        Camera::Instance().SetUsePlayerDeathCmaera();

        // ����UI�ݒ�
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(1500.0f, 700.0f);

        // �ϐ�������
        deathTimer_ = 0.0f;
        isCreateFadeUi_ = false;
    }

    // ----- �X�V -----
    void DeathState::Update(const float& elapsedTime)
    {
        deathTimer_ += elapsedTime;

        if (deathTimer_ > 2.2f)
        {
            if (isCreateFadeUi_ == false)
            {
                UIFader* uiFader = new UIFader();

                isCreateFadeUi_ = true;
            }
        }

        if (deathTimer_ > 3.0f)
        {
            Camera::Instance().SetUsePlayerDeathCmaera(false);
            Camera::Instance().Initialize();
            owner_->SetHealth(owner_->GetMaxHealth());
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }

        // �e�X�g�p
        if (owner_->GetHealth() > 0)
        {
            owner_->ChangeState(Player::STATE::Idle);
        }
    }

    // ----- �I���� -----
    void DeathState::Finalize()
    {
        owner_->SetIsInvincible(false);
    }
    void DeathState::DrawDebug()
    {
    }
}

// ----- ��� -----
namespace PlayerState
{
    // ----- ������ -----
    void DodgeState::Initialize()
    {
        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �A�j���[�V�����ݒ�
        SetAnimation();

        // �ړ��������Z�o
        CalcMoveDirection();

        // ���G��Ԃɂ���
        owner_->SetIsInvincible(true);

        // �X�^�~�i����
        owner_->UseDodgeStamina();

        // ����UI�ݒ�
        if (UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide) != nullptr)
            UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 0.0f);

        // �ϐ�������
        addForceData_.Initialize(0.15f, 0.27f, 0.4f);
        invincibleTimer_ = 0.0f;

        isRotating_ = false;
        isFirstTime_ = false;
    }

    // ----- �X�V -----
    void DodgeState::Update(const float& elapsedTime)
    {
        // ��s���͏���
        if (CheckNextInput()) return;

        Turn(elapsedTime);

        // �A�j���[�V�����̑��x�ݒ�
        SetAnimationSpeed();

        // �ړ�����
        if (addForceData_.Update(owner_->GetAnimationSeconds()))
        {
            owner_->AddForce(moveDirection_, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
        }

        // ���G���ԏ���
        invincibleTimer_ += elapsedTime;
        if (invincibleTimer_ >= 0.4f)
        {
            owner_->SetIsInvincible(false);
        }

        if(owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void DodgeState::Finalize()
    {
        // �ϐ������Z�b�g���Ă���
        isFirstTime_ = true;
    }

    void DodgeState::DrawDebug()
    {
    }

    // ----- ��]���� -----
    void DodgeState::Turn(const float& elapsedTime)
    {
        // ��]�ʂ��Ȃ����߂����ŏI��
        if (isRotating_ == false) return;
        // ���͒l���Ȃ��̂ŉ�]����K�v���Ȃ��B�����ŏI��
        if (isInputStick_ == false) return;

        DirectX::XMFLOAT2 playerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
        playerForward = XMFloat2Normalize(playerForward);

        float forwardCross = XMFloat2Cross(inputDirection_, playerForward);

        float forwardDot = XMFloat2Dot(inputDirection_, playerForward) - 1.0f;

        if (forwardDot > -0.01f)
        {
            isRotating_ = false;
            return;
        }

        const float speed = owner_->GetRotateSpeed() * elapsedTime;
        float rotateY = forwardDot * speed;
        rotateY = std::min(rotateY, -0.7f * speed);

        if (forwardCross > 0)
        {
            owner_->GetTransform()->AddRotationY(rotateY);
        }
        else
        {
            owner_->GetTransform()->AddRotationY(-rotateY);
        }
    }

    // ----- ���̃X�e�[�g�����Z�b�g(������)���� -----
    void DodgeState::ResetState()
    {
        // ------------------------------
        //  �����A�����ďo���Ă���ꍇ
        // ------------------------------
        if (isFirstTime_ == false)
        {
            // ��]�������s��
            isRotating_ = true;
        }

        // �A�j���[�V�����ݒ�
        SetAnimation();

        // �ړ��������Z�o
        CalcMoveDirection();

        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �X�^�~�i����
        owner_->UseDodgeStamina();

        // �ϐ�������
        addForceData_.Initialize(0.15f, 0.27f, 0.4f);
        invincibleTimer_ = 0.0f;
    }

    // ----- ��s���͏��� -----
    const bool DodgeState::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();


#if 1
        const float nextInputStartFrame = 0.5f; // ��s���͊J�n�t���[��

        if (animationSeconds > nextInputStartFrame)
        {
            if (owner_->IsComboAttack0KeyDown())
            {
                owner_->SetNextInput(Player::NextInput::ComboAttack0);
            }

            // ���
            if (owner_->IsDodgeKeyDown() &&
                owner_->GetAnimationIndex() != static_cast<int>(Player::Animation::RollBack))
            {
                owner_->SetNextInput(Player::NextInput::Dodge);

                GamePad& gamePad = Input::Instance().GetGamePad();
                const float aLx = gamePad.GetAxisLX();
                const float aLy = gamePad.GetAxisLY();
                if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
                {
                    const DirectX::XMFLOAT3 cameraFront = Camera::Instance().CalcForward();
                    const DirectX::XMFLOAT3 cameraRight = Camera::Instance().CalcRight();
                    inputDirection_ =
                    {
                        aLy * cameraFront.x + aLx * cameraRight.x,
                        aLy * cameraFront.z + aLx * cameraRight.z,
                    };
                    inputDirection_ = XMFloat2Normalize(inputDirection_);

                    isInputStick_ = true;
                }
                else
                {
                    isInputStick_ = false;
                }
            }
        }

#else 


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
                if (owner_->IsComboAttack0KeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::ComboAttack0);
                }
                // ���
                if (owner_->IsAvoidanceKeyDown())
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
                if (owner_->IsComboAttack0KeyDown())
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
                if (owner_->IsComboAttack0KeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::ComboAttack0);
                }
                // ���
                if (owner_->IsAvoidanceKeyDown())
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
                if (owner_->IsComboAttack0KeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::ComboAttack0);
                }
                // ���
                if (owner_->IsAvoidanceKeyDown())
                {
                    owner_->SetNextInput(Player::NextInput::Avoidance);
                }
            }
        }
            break;
        }
#pragma endregion ��s���͎�t
#endif

        // ��s���͂ɂ��X�e�[�g�ύX����
#pragma region ��s���͂ɂ��X�e�[�g�ύX����
        switch (static_cast<Player::Animation>(owner_->GetAnimationIndex()))
        {
        case Player::Animation::RollFront:// �O
        {
            // ����̐�s���͂�����ꍇ
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                const float avoidanceFrame = 0.92f; // ����ɑJ�ڂł���t���[��
                if (animationSeconds > avoidanceFrame)
                {
                    //����͌��݂Ɠ����X�e�[�g�Ȃ̂ŁA���������Ă�
                    ResetState();
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
                        owner_->ChangeState(Player::STATE::Run);
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
                        owner_->ChangeState(Player::STATE::Run);
                        return true;
                    }
                }
            }
        }
            break;
        case Player::Animation::RollRight:
        {
            // ����̐�s���͂�����ꍇ
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                const float avoidanceFrame = 0.92f; // ����ɑJ�ڂł���t���[��
                if (animationSeconds > avoidanceFrame)
                {
                    //����͌��݂Ɠ����X�e�[�g�Ȃ̂ŁA���������Ă�
                    ResetState();
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
                        owner_->ChangeState(Player::STATE::Run);
                        return true;
                    }
                }
            }
        }
            break;
        case Player::Animation::RollLeft:
        {
            // ����̐�s���͂�����ꍇ
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                const float avoidanceFrame = 0.92f; // ����ɑJ�ڂł���t���[��
                if (animationSeconds > avoidanceFrame)
                {
                    //����͌��݂Ɠ����X�e�[�g�Ȃ̂ŁA���������Ă�
                    ResetState();
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
                        owner_->ChangeState(Player::STATE::Run);
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
    void DodgeState::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        switch (static_cast<Player::Animation>(owner_->GetAnimationIndex()))
        {
        case Player::Animation::RollFront:// �O
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
    void DodgeState::SetAnimation()
    {
        // --------------------------------------------------
        //  �����A�����ďo���Ă���ꍇ
        // --------------------------------------------------
        if (isFirstTime_ == false)
        {
            // �O�����̃A�j���[�V������ݒ肷��
            owner_->PlayBlendAnimation(Player::Animation::RollFront, false, 1.0f, 0.15f);
            owner_->SetTransitionTime(0.05f);
            return;            
        }

        // ���̃A�j���[�V�����ɉ����ău�����h�̎��Ԃ�ݒ肷��
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());
        if (animationIndex == Player::Animation::Attack0_0)
        {
            owner_->SetTransitionTime(0.1f);
        }
        else if (animationIndex == Player::Animation::RunAttack1)
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

        // ------------------------------------------------------------
        // �v���C���[�̎p���ɍ��킹�ăA�j���[�V�����̕�����ݒ肷��
        // ------------------------------------------------------------
        const float animationSpeed = 1.0f;
        const float animationStartFrame = 0.15f;
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
            float cross = XMFloat2Cross(cameraInput, ownerFront);

            // ��]�p���X�O�x������������� �O,�E,�� �̎O��
            if (dot < DirectX::XM_PIDIV2)
            {
                // ��]�p���S�T�x������������� �O����
                if (dot < DirectX::XM_PIDIV4)
                {                    
                    owner_->PlayBlendAnimation(Player::Animation::RollFront, false, animationSpeed, animationStartFrame);
                    return;
                }

                // �E����
                if (cross < 0)
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollRight, false, animationSpeed, animationStartFrame);
                }
                // ������
                else
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollLeft, false, animationSpeed, animationStartFrame);
                }
            }
            // ��]�p���X�O�x�����傫����� ��,�E,�� �̎O��
            else
            {
                // ��]�p���P�R�T�x�����傫����� �����
                if (dot > DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4)
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollBack, false, animationSpeed, animationStartFrame);
                    return;
                }

                // �E����
                if (cross < 0)
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollRight, false, animationSpeed, animationStartFrame);
                }
                // ������
                else
                {
                    owner_->PlayBlendAnimation(Player::Animation::RollLeft, false, animationSpeed, animationStartFrame);
                }
            }
        }
        // ���͒l���Ȃ��ꍇ�O�����̃A�j���[�V������ݒ肷��
        else
        {
            owner_->PlayBlendAnimation(Player::Animation::RollFront, false, animationSpeed, animationStartFrame);
            return;
        }
    }

    // ----- �ړ������Z�o -----
    void DodgeState::CalcMoveDirection()
    {
        if (isFirstTime_ == false)
        {
            if (isInputStick_)
            {
                moveDirection_ = { inputDirection_.x, 0.0f, inputDirection_.y };
            }
            else
            {
                moveDirection_ = owner_->GetTransform()->CalcForward();
            }
            return;
        }

        // ----------------------------------------
        // �������g���猩���O�㍶�E�̃x�N�g����p�ӂ���
        // ----------------------------------------        
        const DirectX::XMFLOAT3 ownerFront = owner_->GetTransform()->CalcForward();
        const DirectX::XMFLOAT3 ownerRight = owner_->GetTransform()->CalcRight();
        const DirectX::XMFLOAT3 moveDirection[4] =
        {
            ownerFront,
            ownerFront * -1,
            ownerRight,
            ownerRight * -1,
        };
        
        // ----------------------------------------
        // �A�j���[�V�����ɉ����Ĉړ�������ݒ肷��
        // ----------------------------------------
        const int animationIndex = owner_->GetAnimationIndex();
        const int differenceNum = static_cast<int>(Player::Animation::RollFront);

        moveDirection_  = moveDirection[animationIndex - differenceNum];
    }
}

// ----- �J�E���^�[ -----
namespace PlayerState
{
    // ----- ������ -----
    void CounterState::Initialize()
    {
        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �A�j���[�V�����ݒ�
        SetAnimation();

        // �J�E���^�[���J�������g�p����
        Camera::Instance().SetUseCounterCamera();

        // ����UI�ݒ�
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(1500.0f, 0.0f);

        // �ϐ�������
        addForceBack_.Initialize(0.16f, 0.2f, 0.5f);
        addForceFront_.Initialize(0.66f, 0.30f, 1.0f);
        gamePadVibration_.Initialize(0.3f, 0.2f, 0.5f);
        attackData_.Initialize(0.75f, 1.0f);

        isNextInput_ = false;

        isCounterReaction = false;

        isTurnChecked_ = false;

        const DirectX::XMFLOAT3 pos = owner_->GetJointPosition("spine_02");
        Effect* counterEffect = EffectManager::Instance().GetEffect("Mikiri");
        //mikiriEffectHandle_ = counterEffect->Play(pos, 0.3f, 2.0f);
        mikiriEffectHandle_ = counterEffect->Play(pos, 0.05f, 2.0f);
    }

    // ----- �X�V -----
    void CounterState::Update(const float& elapsedTime)
    {
        const float counterStartFrame = 0.1f;
        const float counterEndFrame = 0.6f;
        // �J�E���^�[����
        if (owner_->GetAnimationSeconds() > counterEndFrame)
        {
            if (owner_->GetIsCounter()) owner_->SetIsCounter(false);
        }
        else if (owner_->GetAnimationSeconds() > counterStartFrame)
        {
            if (owner_->GetIsCounter() == false) owner_->SetIsCounter(true);
        }

        // �G�t�F�N�g
        if (addForceBack_.GetIsAddForce())
        {
            EffectManager::Instance().AddPosition(mikiriEffectHandle_, mikiriEffectAddPosition_ * 3.0f * elapsedTime);
        }

        // ���؂肪����������
        if (owner_->GetIsAbleCounterAttack() && isCounterReaction == false)
        {
            // --------------------------------------------------
            //      �R���g���[���[�U���A�G�t�F�N�g�A���ʉ����o���B
            //       �o���^�C�~���O�̓R���g���[���[�U���ɔC����
            // --------------------------------------------------
            bool isVibrated = gamePadVibration_.Update(owner_->GetAnimationSeconds());

            if (isVibrated)
            {
                // �G�t�F�N�g���Đ�����
                Effect* counterEffect = EffectManager::Instance().GetEffect("Counter");
                if (counterEffect != nullptr)
                {
                    // �G�t�F�N�g�͌��̈ʒu�ɏo��
                    const DirectX::XMFLOAT3 offsetPosition = { -50.0f, 13.0f, 20.0f };
                    const DirectX::XMFLOAT3 position = owner_->GetJointPosition("hand_r", offsetPosition);

                    // �ʒu���X�V���邽�߂̃f�[�^��ۑ�����
                    effectOffsetVec_ = XMFloat3Normalize(position - owner_->GetTransform()->GetPosition());
                    effectLength_ = XMFloat3Length(position - owner_->GetTransform()->GetPosition());
                    counterEffectHandle_ = counterEffect->Play(position, 0.1f, 4.0f);
                }

                // TODO: ���ʉ���炷

                isCounterReaction = true;
            }
        }

        // �G�t�F�N�g�̈ʒu���X�V����
        if (isCounterReaction)
        {
            const DirectX::XMFLOAT3 position = owner_->GetTransform()->GetPosition() + effectOffsetVec_ * effectLength_;
            EffectManager::Instance().SetPosition(counterEffectHandle_, position);
        }

        // �A�j���[�V�����̑��x�ݒ�
        SetAnimationSpeed();

        // �ړ�����
        Move();

        // ���񏈗�
        Turn(elapsedTime);

        // �U�����菈��
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);

        // �A�j���[�V�����Đ��I��
        //if(owner_->GetAnimationSeconds() > 1.0f)
        if(owner_->GetAnimationSeconds() > 1.2f)
        //if(owner_->IsPlayAnimation() == false)
        {
            EffectManager::Instance().StopEffect(counterEffectHandle_);

            owner_->ChangeState(Player::STATE::Idle);

            return;
        }

        // �J�E���^�[����
        //if (owner_->GetIsAbleCounterAttack())
        {
            if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_RIGHT_TRIGGER)
            {
                isNextInput_ = true;
            }
        }
        if (isNextInput_ && owner_->GetAnimationSeconds() > 0.9f)
        {
            EffectManager::Instance().StopEffect(counterEffectHandle_);

            owner_->ChangeState(Player::STATE::CounterCombo);
            return;
        }
    }

    // ----- �I���� -----
    void CounterState::Finalize()
    {
    }

    void CounterState::DrawDebug()
    {
    }

    // ----- �A�j���[�V�����ݒ� -----
    void CounterState::SetAnimation()
    {
        const Player::STATE oldState = owner_->GetOldState();
        if (oldState == Player::STATE::ComboAttack0_0 ||
            oldState == Player::STATE::ComboAttack0_1 ||
            oldState == Player::STATE::ComboAttack0_2 ||
            oldState == Player::STATE::RunAttack)
        {
            owner_->PlayBlendAnimation(Player::Animation::Counter, false, 1.0f, 0.15f);
            owner_->SetTransitionTime(0.1f);
            return;
        }

        owner_->PlayBlendAnimation(Player::Animation::Counter, false, 1.0f);
        owner_->SetTransitionTime(0.1f);
    }

    // ----- �ړ����� -----
    void CounterState::Move()
    {
        // �������ɐi��
#pragma region ---------- �������ɐi�� ----------
        if (addForceBack_.Update(owner_->GetAnimationSeconds()))
        {
            // --------------------------------------------------
            //  ���X�e�B�b�N�̓��͂�����΂��̕����Ɍ����悤�ɂ���
            //          �������͂��Ȃ���Ό��ɉ�����
            // --------------------------------------------------
            DirectX::XMFLOAT3 addForceDirection = {};

            // ���X�e�B�b�N�̓��͂����邩����
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                // �J�������猩�����X�e�B�b�N�̌X����K�������������Z�o����
                const DirectX::XMFLOAT3 cameraForward = Camera::Instance().CalcForward();
                const DirectX::XMFLOAT3 cameraRight = Camera::Instance().CalcRight();
                addForceDirection =
                {
                    aLy * cameraForward.x + aLx * cameraRight.x,
                    0,
                    aLy * cameraForward.z + aLx * cameraRight.z
                };
                addForceDirection = XMFloat3Normalize(addForceDirection);
                addForceDirection = addForceDirection * -1;

                // ��C�ɃX�e�B�b�N�𔽑Ε����Ɍ�����ƁA�v���C���[���Ⴄ�����������̂ŏC��
                DirectX::XMFLOAT2 cameraForward_float2 = { -addForceDirection.x, -addForceDirection.z };
                cameraForward_float2 = XMFloat2Normalize(cameraForward_float2);

                DirectX::XMFLOAT2 ownerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
                ownerForward = XMFloat2Normalize(ownerForward);

                // �O�ς����Ăǂ���ɉ�]����̂��𔻒肷��
                float forwardCross = XMFloat2Cross(cameraForward_float2, ownerForward);

                // ���ςŉ�]�����Z�o
                float forwardDot = XMFloat2Dot(cameraForward_float2, ownerForward) - 1.0f;

                if (forwardCross > 0)
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
                addForceDirection = owner_->GetTransform()->CalcForward() * -1.0f;
            }

            mikiriEffectAddPosition_ = addForceDirection;

            owner_->AddForce(addForceDirection, addForceBack_.GetForce(), addForceBack_.GetDecelerationForce());
        }
#pragma endregion ---------- �������ɐi�� ----------

        // �O�����ɐi��
#pragma region ---------- �O�����ɐi�� ----------
        if (addForceFront_.Update(owner_->GetAnimationSeconds()))
        {
            // --------------------------------------------------
            //  ���X�e�B�b�N�̓��͂�����΂��̕����Ɍ����悤�ɂ���
            //          �������͂��Ȃ���΂��̂܂ܑO�ɐi��
            // --------------------------------------------------
            DirectX::XMFLOAT3 addForceDirection = {};

            // ���X�e�B�b�N�̓��͂����邩����
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (fabsf(aLx) > 0.0f || fabsf(aLy) > 0.0f)
            {
                // �J�������猩�����X�e�B�b�N�̌X����K�������������Z�o����
                const DirectX::XMFLOAT3 cameraForward = Camera::Instance().CalcForward();
                const DirectX::XMFLOAT3 cameraRight = Camera::Instance().CalcRight();
                addForceDirection =
                {
                    aLy * cameraForward.x + aLx * cameraRight.x,
                    0,
                    aLy * cameraForward.z + aLx * cameraRight.z
                };
                addForceDirection = XMFloat3Normalize(addForceDirection);

                addForceDirection_ = { addForceDirection.x, addForceDirection.z };
                addForceDirection_ = XMFloat2Normalize(addForceDirection_);

                // ------------------------------------------------------------
                //              ��]����p�x�͍��E�Ƃ��ɂX�O�x�܂�
                // 
                //                      ownerFront
                //                          |
                //                          | 
                //             Left ================== Right
                //                         Back
                // 
                //           0�� ~ 90��, 270�� ~ 360�� �ɂȂ�悤�ɕ␳����
                // ------------------------------------------------------------
                DirectX::XMFLOAT2 ownerFront = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
                ownerFront = XMFloat2Normalize(ownerFront);

                // ���ςŊp�x���Z�o
                float dot = acosf(XMFloat2Dot(addForceDirection_, ownerFront));

                // 90�x�ȏ��]�p������
                if (dot > DirectX::XM_PIDIV2)
                {
                    // ���E����
                    float cross = XMFloat2Cross(addForceDirection_, ownerFront);

                    const DirectX::XMFLOAT3 ownerRight_float3 = owner_->GetTransform()->CalcRight();
                    const DirectX::XMFLOAT2 ownerRight_float2 = XMFloat2Normalize({ ownerRight_float3.x, ownerRight_float3.z });

                    if (cross < 0)
                    {
                        addForceDirection_ = ownerRight_float2;
                        addForceDirection = { addForceDirection_.x, 0.0f, addForceDirection_.y };
                    }
                    else
                    {
                        addForceDirection_ = ownerRight_float2 * -1;
                        addForceDirection = { addForceDirection_.x, 0.0f, addForceDirection_.y };
                    }
                }

                isRotating_ = true;
            }
            else
            {
                addForceDirection = owner_->GetTransform()->CalcForward();
            }

            owner_->AddForce(addForceDirection, addForceFront_.GetForce(), addForceFront_.GetDecelerationForce());
        }
#pragma endregion ---------- �O�����ɐi�� ----------
    }

    // ----- ���񏈗� -----
    void CounterState::Turn(const float& elapsedTime)
    {
        // ��]�p�x���Ȃ��̂ł����ŏI��
        if (isRotating_ == false) return;

        // �܂����񏈗����s��Ȃ�
        if (addForceFront_.GetIsAddForce() == false) return;


        DirectX::XMFLOAT2 ownerForward = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
        ownerForward = XMFloat2Normalize(ownerForward);

        // �O�ς����Ăǂ���ɉ�]����̂��𔻒肷��
        float forwardCross = XMFloat2Cross(addForceDirection_, ownerForward);

        // ���ςŉ�]�����Z�o
        float forwardDot = XMFloat2Dot(addForceDirection_, ownerForward) - 1.0f;

        if (forwardDot > -0.01f)
        {
            isRotating_ = false;
        }

        // TODO:��]���x���Œ�l�œ��ꂿ����Ă�
        //const float speed = owner_->GetRotateSpeed() * elapsedTime;
        const float speed = 8.0f * elapsedTime;
        float rotateY = forwardDot * speed;
        rotateY = std::min(rotateY, -0.7f * speed);

        if (forwardCross > 0)
        {
            owner_->GetTransform()->AddRotationY(rotateY);
        }
        else
        {
            owner_->GetTransform()->AddRotationY(-rotateY);
        }
    }

    // ----- �A�j���[�V�����̑��x�ݒ� -----
    void CounterState::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > 0.7f)
        {
            owner_->SetAnimationSpeed(1.0f);
        }
        else if (animationSeconds > 0.4f)
        {
            owner_->SetAnimationSpeed(0.7f);
        }
    }
}

// ----- �J�E���^�[�U�� -----
namespace PlayerState
{
    // ----- ������ -----
    void CounterComboState::Initialize()
    {
        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �A�j���[�V�����Đ�
        owner_->PlayBlendAnimation(Player::Animation::CounterAttack1, false, 1.0f, 0.35f);        

        // ���G��Ԃɂ���
        owner_->SetIsInvincible(true);

        // ����UI�ݒ�
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(1500.0f, 700.0f);

        // �ϐ�������
        addForceData_.Initialize(0.35f, 0.3f, 1.0f);
        attackData_.Initialize(0.35f, 0.7f);
    }

    // ----- �X�V -----
    void CounterComboState::Update(const float& elapsedTime)
    {
        // RootMotion�̐ݒ�
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotion���g�p����
            owner_->SetUseRootMotion(true);
        }

        // �U�����菈��
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);

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
        // ���G��Ԃɂ���������
        owner_->SetIsInvincible(false);

        owner_->SetUseRootMotion(false);
    }
    void CounterComboState::DrawDebug()
    {
    }
}

// ----- ����U�� -----
namespace PlayerState
{
    // ----- ������ -----
    void RunAttackState::Initialize()
    {
        // �t���O���Z�b�g
        owner_->ResetFlags();

        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::RunAttack1, false, 1.0f, 0.2f);
        owner_->SetTransitionTime(0.1f);

        // ��s���͐ݒ�
        owner_->SetNextInputStartFrame(0.0f, 0.3f, 0.3f, 0.8f);
        owner_->SetNextInputEndFrame(1.8f, 1.8f, 1.8f);
        owner_->SetNextInputTransitionFrame(0.6f, 0.6f, 0.6f);

        // ����UI�ݒ�
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 350.0f);

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
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);

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

    void RunAttackState::DrawDebug()
    {
    }

    // ----- ��s���͏��� -----
    const bool RunAttackState::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // -----------------------------------
        //      ����ɂ��U���L�����Z��
        // -----------------------------------
        if (owner_->GetIsDodgeAttackCancel())
        {
            if (owner_->IsDodgeKeyDown())
            {
                owner_->ChangeState(Player::STATE::Dodge);
                return true;
            }
        }
        else
        {
            // ����s���͎�t
            if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
                animationSeconds <= owner_->GetDodgeInputEndFrame())
            {
                if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
            }
            // ���J�ڃ`�F�b�N
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                if (animationSeconds >= owner_->GetDodgeTransitionFrame())
                {
                    owner_->ChangeState(Player::STATE::Dodge);
                    return true;
                }
            }
        }

#pragma region ----- ��s���͎�t -----
        // �U����s���͎�t
        if (animationSeconds >= owner_->GetAttackInputStartFrame() &&
            animationSeconds <= owner_->GetAttackInputEndFrame())
        {
            if (owner_->IsComboAttack0KeyDown()) owner_->SetNextInput(Player::NextInput::ComboAttack0);
        }
        // �J�E���^�[��s���͎�t
        if (animationSeconds >= owner_->GetCounterInputStartFrame() &&
            animationSeconds <= owner_->GetCounterInputEndFrame())
        {
            if (owner_->IsCounterStanceKey()) owner_->SetNextInput(Player::NextInput::Counter);
        }

#pragma endregion ----- ��s���͎�t -----

#pragma region ----- �J�ڃ`�F�b�N -----
        // �U���J�ڃ`�F�b�N
        if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
        {
            if (animationSeconds >= owner_->GetAttackTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_1);
                return true;
            }
        }
        // �J�E���^�[�J�ڃ`�F�b�N
        else if (owner_->GetNextInput() == Player::NextInput::Counter)
        {
            if (animationSeconds >= owner_->GetCounterTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Counter);
                return true;
            }
        }

#pragma endregion ----- �J�ڃ`�F�b�N -----

        // �ړ����͔���
        if (animationSeconds >= owner_->GetMoveInputStartFrame())
        {
            // �X�e�B�b�N���͂����邩
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (aLx == 0.0f && aLy == 0.0f) return false;

            owner_->ChangeState(Player::STATE::Run);
            return true;
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
        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �A�j���[�V�����ݒ�
        SetAnimation();

        // ��]�␳�ʂ����߂�
        owner_->CalculateRotationAdjustment();

        // ��s���͐ݒ�
        owner_->SetNextInputStartFrame(0.13f, 0.13f, 0.13f, 0.6f);
        owner_->SetNextInputEndFrame(1.583f, 0.75f, 1.5f);
        owner_->SetNextInputTransitionFrame(0.4f, 0.3f, 0.3f);

        // ����UI�ݒ�
        if (UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide) != nullptr)
            UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 350.0f);

        // �ϐ�������
        attackData_.Initialize(0.1f, 0.35f);      

    }

    // ----- �X�V -----
    void ComboAttack0_0::Update(const float& elapsedTime)
    {
        // ��s���͔���
        if (CheckNextInput()) return;

        // RootMotion�̐ݒ�
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotion���g�p����
            owner_->SetUseRootMotion(true);
        }
        
        // �A�j���[�V�����̑��x�ݒ�
        SetAnimationSpeed();

        // �U�����菈��
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);
        

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void ComboAttack0_0::Finalize()
    {
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui�p -----
    void ComboAttack0_0::DrawDebug()
    {
        if (ImGui::TreeNode(GetName()))
        {

            ImGui::TreePop();
        }
    }

    // ----- �A�j���[�V�����ݒ� -----
    void ComboAttack0_0::SetAnimation()
    {
        const Player::Animation animationIndex = static_cast<Player::Animation>(owner_->GetAnimationIndex());

        // oldAnimation������̏ꍇ�U���̓r���t���[������J�n����
        if (animationIndex == Player::Animation::RollFront ||
            animationIndex == Player::Animation::RollBack    ||
            animationIndex == Player::Animation::RollRight   ||
            animationIndex == Player::Animation::RollLeft)
        {
            owner_->PlayBlendAnimation(Player::Animation::Attack0_0, false, 1.0f, 0.1f);
        }
        else if (animationIndex == Player::Animation::Run)
        {
            owner_->PlayBlendAnimation(Player::Animation::Attack0_0, false, 1.0f, 0.1f);
            owner_->SetTransitionTime(0.1f);
        }
        else if (animationIndex == Player::Animation::Attack0_1)
        {
            owner_->PlayBlendAnimation(Player::Animation::Attack0_0, false, 1.0f, 0.1f);
            owner_->SetTransitionTime(0.1f);
        }
        else
        {
            owner_->PlayBlendAnimation(Player::Animation::Attack0_0, false);
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
    
    // ----- ��s���͔��� -----
    const bool ComboAttack0_0::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // -----------------------------------
        //      ����ɂ��U���L�����Z��
        // -----------------------------------
        if (owner_->GetIsDodgeAttackCancel())
        {
            if (owner_->IsDodgeKeyDown())
            {
                owner_->ChangeState(Player::STATE::Dodge);
                return true;
            }
        }
        else
        {
            // ����s���͎�t
            if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
                animationSeconds <= owner_->GetDodgeInputEndFrame())
            {
                if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
            }

            // ���J�ڃ`�F�b�N
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                if (animationSeconds >= owner_->GetDodgeTransitionFrame())
                {
                    owner_->ChangeState(Player::STATE::Dodge);
                    return true;
                }
            }
        }


        // -----------------------------------
        //      �ړ����͂ɂ��㌄�L�����Z��
        // -----------------------------------
        if (animationSeconds >= owner_->GetMoveInputStartFrame())
        {
            // �X�e�B�b�N���͂����邩
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (aLx == 0.0f && aLy == 0.0f) return false;

            owner_->SetIsMoveAttackCancel(true);

            owner_->ChangeState(Player::STATE::Run);
            return true;
        }




#pragma region ----- ��s���͎�t -----

        // �U����s���͎�t
        if (animationSeconds >= owner_->GetAttackInputStartFrame() &&
            animationSeconds <= owner_->GetAttackInputEndFrame())
        {
            if (owner_->IsComboAttack0KeyDown()) owner_->SetNextInput(Player::NextInput::ComboAttack0);
        }
        // �J�E���^�[��s���͎�t
        if (animationSeconds >= owner_->GetCounterInputStartFrame() &&
            animationSeconds <= owner_->GetCounterInputEndFrame())
        {
            if (owner_->IsCounterStanceKey()) owner_->SetNextInput(Player::NextInput::Counter);
        }

#pragma endregion ----- ��s���͎�t -----

#pragma region ----- �J�ڃ`�F�b�N -----

        // �U���J�ڃ`�F�b�N
        if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
        {
            if (animationSeconds >= owner_->GetAttackTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_1);
                return true;
            }
        }
        // �J�E���^�[�J�ڃ`�F�b�N
        else if (owner_->GetNextInput() == Player::NextInput::Counter)
        {
            if (animationSeconds >= owner_->GetCounterTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Counter);
                return true;
            }
        }

#pragma endregion ----- �J�ڃ`�F�b�N -----

        return false;
    }
}

// ----- �R���{�U��0_1 -----
namespace PlayerState
{
    // ----- ������ -----
    void ComboAttack0_1::Initialize()
    {
        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �A�j���[�V�����ݒ�
        SetAnimation();

        // ��]�␳�ʂ����߂�
        owner_->CalculateRotationAdjustment();

        // ��s���͐ݒ�
        owner_->SetNextInputStartFrame(0.13f, 0.13f, 0.13f, 0.5f);
        owner_->SetNextInputEndFrame(1.583f, 0.75f, 1.5f);
        owner_->SetNextInputTransitionFrame(0.4f, 0.3f, 0.3f);

        // ����UI�ݒ�
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(750.0f, 350.0f);

        // �ϐ�������
        attackData_.Initialize(0.06f, 0.3f);
    }

    // ----- �X�V -----
    void ComboAttack0_1::Update(const float& elapsedTime)
    {
        // ��s���͏���
        if (CheckNextInput()) return;

        // RootMotion�̐ݒ�
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotion���g�p����
            owner_->SetUseRootMotion(true);
        }

        // �A�j���[�V�����̑��x�ݒ�
        SetAnimationSpeed();

        // �U�����菈��
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);

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
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui�p -----
    void ComboAttack0_1::DrawDebug()
    {
        if (ImGui::TreeNode(GetName()))
        {

            ImGui::TreePop();
        }
    }

    // ----- �A�j���[�V�����ݒ� -----
    void ComboAttack0_1::SetAnimation()
    {
        if (owner_->GetAnimationIndex() == static_cast<int>(Player::Animation::RunAttack1))
        {
            owner_->SetTransitionTime(0.2f);
            owner_->PlayBlendAnimation(Player::Animation::Attack0_1, false, 1.0f, 0.1f);
        }
        else
        {
            owner_->SetTransitionTime(0.1f);
            owner_->PlayBlendAnimation(Player::Animation::Attack0_1, false);
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

        // -----------------------------------
        //      ����ɂ��U���L�����Z��
        // -----------------------------------
        if (owner_->GetIsDodgeAttackCancel())
        {
            if (owner_->IsDodgeKeyDown())
            {
                owner_->ChangeState(Player::STATE::Dodge);
                return true;
            }
        }
        else
        {
            // ����s���͎�t
            if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
                animationSeconds <= owner_->GetDodgeInputEndFrame())
            {
                if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
            }

            // ���J�ڃ`�F�b�N
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                if (animationSeconds >= owner_->GetDodgeTransitionFrame())
                {
                    owner_->ChangeState(Player::STATE::Dodge);
                    return true;
                }
            }
        }

        // -----------------------------------
        //      �ړ����͂ɂ��㌄�L�����Z��
        // -----------------------------------
        if (animationSeconds >= owner_->GetMoveInputStartFrame())
        {
            // �X�e�B�b�N���͂����邩
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (aLx == 0.0f && aLy == 0.0f) return false;

            owner_->SetIsMoveAttackCancel(true);

            owner_->ChangeState(Player::STATE::Run);
            return true;
        }

#pragma region ----- ��s���͎�t -----
        // �U����s���͎�t
        if (animationSeconds >= owner_->GetAttackInputStartFrame() &&
            animationSeconds <= owner_->GetAttackInputEndFrame())
        {
            if (owner_->IsComboAttack0KeyDown()) owner_->SetNextInput(Player::NextInput::ComboAttack0);
        }
        // �J�E���^�[��s���͎�t
        if (animationSeconds >= owner_->GetCounterInputStartFrame() &&
            animationSeconds <= owner_->GetCounterInputEndFrame())
        {
            if (owner_->IsCounterStanceKey()) owner_->SetNextInput(Player::NextInput::Counter);
        }

#pragma endregion ----- ��s���͎�t -----

#pragma region ----- �J�ڃ`�F�b�N -----
        // �U���J�ڃ`�F�b�N
        if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
        {
            if (animationSeconds >= owner_->GetAttackTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_2);
                return true;
            }
        }
        // �J�E���^�[�J�ڃ`�F�b�N
        else if (owner_->GetNextInput() == Player::NextInput::Counter)
        {
            if (animationSeconds >= owner_->GetCounterTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Counter);
                return true;
            }
        }

#pragma endregion ----- �J�ڃ`�F�b�N -----

        return false;
    }
}

// ----- �R���{�U��0_2 -----
namespace PlayerState
{
    // ----- ������ -----
    void ComboAttack0_2::Initialize()
    {
        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::Attack0_2, false, 1.3f, 0.4f);
        owner_->SetTransitionTime(0.3f);

        // ��]�␳�ʂ����߂�
        owner_->CalculateRotationAdjustment();

        // ��s���͐ݒ�
        owner_->SetNextInputStartFrame(0.7f, 0.7f, 0.7f, 1.4f);
        owner_->SetNextInputEndFrame(1.9f, 1.3f, 1.3f);
        owner_->SetNextInputTransitionFrame(1.1f, 0.9f, 0.9f);
        
        // ����UI�ݒ�
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(1500.0f, 350.0f);

        // �ϐ�������
        attackData_.Initialize(0.7f, 0.9f);

    }

    // ----- �X�V -----
    void ComboAttack0_2::Update(const float& elapsedTime)
    {
        // ��s���͏���
        if (CheckNextInput()) return;

        // RootMotion�̐ݒ�
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotion���g�p����
            owner_->SetUseRootMotion(true);
        }

        // �A�j���[�V�����̑��x�ݒ�
        SetAnimationSpeed();

        // �U�����菈��
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);




        if (owner_->IsPlayAnimation() == false)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void ComboAttack0_2::Finalize()
    {
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui�p -----
    void ComboAttack0_2::DrawDebug()
    {
        if (ImGui::TreeNode(GetName()))
        {

            ImGui::TreePop();
        }
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

        // -----------------------------------
        //      ����ɂ��U���L�����Z��
        // -----------------------------------
        if (owner_->GetIsDodgeAttackCancel())
        {
            if (owner_->IsDodgeKeyDown())
            {
                owner_->ChangeState(Player::STATE::Dodge);
                return true;
            }
        }
        else
        {
            // ����s���͎�t
            if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
                animationSeconds <= owner_->GetDodgeInputEndFrame())
            {
                if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
            }

            // ���J�ڃ`�F�b�N
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                if (animationSeconds >= owner_->GetDodgeTransitionFrame())
                {
                    owner_->ChangeState(Player::STATE::Dodge);
                    return true;
                }
            }
        }

        // -----------------------------------
        //      �ړ����͂ɂ��㌄�L�����Z��
        // -----------------------------------
        if (animationSeconds >= owner_->GetMoveInputStartFrame())
        {
            // �X�e�B�b�N���͂����邩
            const float aLx = Input::Instance().GetGamePad().GetAxisLX();
            const float aLy = Input::Instance().GetGamePad().GetAxisLY();
            if (aLx == 0.0f && aLy == 0.0f) return false;

            owner_->SetIsMoveAttackCancel(true);

            owner_->ChangeState(Player::STATE::Run);
            return true;
        }

#pragma region ----- ��s���͎�t -----
        // �U����s���͎�t
        if (animationSeconds >= owner_->GetAttackInputStartFrame() &&
            animationSeconds <= owner_->GetAttackInputEndFrame())
        {
            if (owner_->IsComboAttack0KeyDown()) owner_->SetNextInput(Player::NextInput::ComboAttack0);
        }
        // �J�E���^�[��s���͎�t
        if (animationSeconds >= owner_->GetCounterInputStartFrame() &&
            animationSeconds <= owner_->GetCounterInputEndFrame())
        {
            if (owner_->IsCounterStanceKey()) owner_->SetNextInput(Player::NextInput::Counter);
        }

#pragma endregion ----- ��s���͎�t -----

#pragma region ----- �J�ڃ`�F�b�N -----
        // �U���J�ڃ`�F�b�N
        if (owner_->GetNextInput() == Player::NextInput::ComboAttack0)
        {
            if (animationSeconds >= owner_->GetAttackTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::ComboAttack0_3);
                return true;
            }
        }
        // �J�E���^�[�J�ڃ`�F�b�N
        else if (owner_->GetNextInput() == Player::NextInput::Counter)
        {
            if (animationSeconds >= owner_->GetCounterTransitionFrame())
            {
                owner_->ChangeState(Player::STATE::Counter);
                return true;
            }
        }

#pragma endregion ----- �J�ڃ`�F�b�N -----

        return false;
    }
}

// ----- �R���{�U��0_3 -----
namespace PlayerState
{
    // ----- ������ -----
    void ComboAttack0_3::Initialize()
    {
        // �t���O�����Z�b�g����
        owner_->ResetFlags();

        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Player::Animation::Attack0_3, false);
        owner_->SetTransitionTime(0.1f);

        // ��]�␳�ʂ����߂�
        owner_->CalculateRotationAdjustment();

        // ��s���͐ݒ�
        owner_->SetNextInputStartFrame(0.7f, 3.0f, 3.0f, 3.0f);
        owner_->SetNextInputEndFrame(2.0f, 3.0f, 3.0f);
        owner_->SetNextInputTransitionFrame(1.3f, 3.0f, 3.0f);

        // ����UI�ݒ�
        UIManager::Instance().GetUI(UIManager::UIType::UIActionGuide)->GetTransform()->SetTexPos(0.0f, 700.0f);

        // �ϐ�������
        attackData_.Initialize(0.65f, 0.8f);
        isVibration_ = false;

 
    }

    // ----- �X�V -----
    void ComboAttack0_3::Update(const float& elapsedTime)
    {
        // ��s���͔���
        if (CheckNextInput()) return;

        // �A�j���[�V�����̑��x�ݒ�
        SetAnimationSpeed();

        // RootMotion�̐ݒ�
        if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == false)
        {
            // RootMotion���g�p����
            owner_->SetUseRootMotion(true);
        }

        // �U�����菈��
        const bool attackFlag = attackData_.Update(owner_->GetAnimationSeconds(), owner_->GetIsAttackHit());
        owner_->SetIsAttackValid(attackFlag);


        // �R���g���[���[���J���� �U��
        if (owner_->GetAnimationSeconds() > 0.8f && isVibration_ == false)
        {
            Input::Instance().GetGamePad().Vibration(0.2f, 1.0f);
            Camera::Instance().ScreenVibrate(0.1f, 0.2f);

            isVibration_ = true;
        }

        if(owner_->GetAnimationSeconds() > 1.7f)
        {
            owner_->ChangeState(Player::STATE::Idle);
            return;
        }
    }

    // ----- �I���� -----
    void ComboAttack0_3::Finalize()
    {
        owner_->SetUseRootMotion(false);
    }

    // ----- ImGui�p -----
    void ComboAttack0_3::DrawDebug()
    {
        if (ImGui::TreeNode(GetName()))
        {


            ImGui::TreePop();
        }
    }

    // ----- �A�j���[�V�����̑��x�ݒ� -----
    void ComboAttack0_3::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();


    }
    
    // ----- ��s���͏��� -----
    const bool ComboAttack0_3::CheckNextInput()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // -----------------------------------
        //      ����ɂ��U���L�����Z��
        // -----------------------------------
        if (owner_->GetIsDodgeAttackCancel())
        {
            if (owner_->IsDodgeKeyDown())
            {
                owner_->ChangeState(Player::STATE::Dodge);
                return true;
            }
        }
        else
        {
            // ����s���͎�t
            if (animationSeconds >= owner_->GetDodgeInputStartFrame() &&
                animationSeconds <= owner_->GetDodgeInputEndFrame())
            {
                if (owner_->IsDodgeKeyDown()) owner_->SetNextInput(Player::NextInput::Dodge);
            }

            // ���J�ڃ`�F�b�N
            if (owner_->GetNextInput() == Player::NextInput::Dodge)
            {
                if (animationSeconds >= owner_->GetDodgeTransitionFrame())
                {
                    owner_->ChangeState(Player::STATE::Dodge);
                    return true;
                }
            }
        }

        return false;
    }
}