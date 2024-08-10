#include "ActionDragon.h"
#include "PostProcess/PostProcess.h"
#include "Easing.h"
#include "Input.h"
#include "Camera.h"
#include "../Player/PlayerManager.h"

// ----- GamePadVibration -----
namespace ActionDragon
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
    void GamePadVibration::Update(const float& animationFrame)
    {
        // ���ɐU�������Ă���
        if (isVibraion_) return;

        // ���݂̃A�j���[�V�����̃t���[�����X�^�[�g�t���[���܂ŒB���Ă��Ȃ�
        if (animationFrame < startFrame_) return;
        
        // �R���g���[���[��U��������
        Input::Instance().GetGamePad().Vibration(time_, power_);
        isVibraion_ = true;
    }
}

// ----- AddForceData -----
namespace ActionDragon
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

// ----- ���S�s�� -----
namespace ActionDragon
{
    const ActionBase::State DeathAction::Run(const float& elapsedTime)
    {
        if (owner_->GetStep() == 0)
        {
            // ���S���J�������g�p����
            Camera::Instance().SetUseEnemyDeathCamera();

            owner_->SetStep(1);
        }

        return ActionBase::State::Run;
    }
}

// ----- ���ݍs�� -----
namespace ActionDragon
{
    const ActionBase::State FlinchAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        const float animationSpeed = 1.5f;
        //const float animationSpeed = 1.0f;
        //const float animationSpeed = 0.8f;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// ������
            // �A�j���[�V�����ݒ�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::CriticalStart, false);
            owner_->SetTransitionTime(0.1f);

            // ���ݎ������o������ݒ�
            owner_->SetDownCollisionActiveFlag();

            // �ϐ�������
            addForceData_.Initialize(0.01, 0.2f, 1.5f);
            loopCounter_ = 0;
            //maxLoopNum_ = 2;
            maxLoopNum_ = 4;

            // �X�e�[�g�ύX
            SetState(STATE::FlinchStart);

            break;
        case STATE::FlinchStart:// ���݃X�^�[�g( �|�ꂱ�� )
        {
            // �ړ�����
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                DirectX::XMFLOAT3 ownerRight = owner_->GetTransform()->CalcRight() * -1;

                owner_->AddForce(ownerRight, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            // �w��̃t���[���𒴂����玟�ɐi��
            const float animationEndFrame = 1.0f;
            if (owner_->GetAnimationSeconds() > animationEndFrame)
            {
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::CriticalLoop, false, animationSpeed);
                owner_->SetTransitionTime(0.1f);
                // �X�e�[�g�ύX
                SetState(STATE::FlinchLoop);
            }
        }

            break;
        case STATE::FlinchLoop:// ���݃��[�v

            if (owner_->IsPlayAnimation() == false)
            {
                if (loopCounter_ > maxLoopNum_)
                {
                    owner_->PlayAnimation(Enemy::DragonAnimation::CriticalEnd, false, animationSpeed);
                    // �X�e�[�g�ύX
                    SetState(STATE::FlinchEnd);
                }
                else
                {
                    owner_->PlayAnimation(Enemy::DragonAnimation::CriticalLoop, false, animationSpeed);
                }

                ++loopCounter_;                
            }

            break;
        case STATE::FlinchEnd:// ���ݏI���            

            // �A�j���[�V�������x�ݒ�
            SetAnimationSpeed();

            if (owner_->IsPlayAnimation() == false)
            {
                // �t���O�����Z�b�g
                owner_->SetIsFlinch(false);
                owner_->SetDownCollisionActiveFlag(false);

                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }

    // ----- �A�j���[�V�������x�ݒ� -----
    void FlinchAction::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > 1.5f)
        {
            owner_->SetAnimationSpeed(1.0f);
        }
    }
}

// ----- ��s�����ݍs�� -----
namespace ActionDragon
{
    const ActionBase::State FlyFlinchAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        const float animationSpeed = 1.5f;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// ������
            // �A�j���[�V�����ݒ�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::CriticalStart, false);
            owner_->SetTransitionTime(0.35f);

            // ���[�g���[�V�����̈ړ��l���Ȃ���
            owner_->SetUseRootMotionMovement(false);

            // ���ݎ��̉����o������
            owner_->SetDownCollisionActiveFlag();

            // �ϐ�������
            easingTimer_ = 0.0f;
            oldPositionY_ = owner_->GetTransform()->GetPositionY() + 0.5f;
            loopCounter_ = 0;
            maxLoopNum_ = 4;

            // �X�e�[�g�ύX
            SetState(STATE::FlinchStart);

            break;
        case STATE::FlinchStart:// ���݊J�n
        {
            const float totalFrame = 0.6f;
            easingTimer_ += elapsedTime;
            easingTimer_ = std::min(easingTimer_, totalFrame);
            const float posY = Easing::InSine(easingTimer_, totalFrame, 0.7f, oldPositionY_);
            owner_->GetTransform()->SetPositionY(posY);
        }

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetUseRootMotionMovement(true);

                owner_->PlayAnimation(Enemy::DragonAnimation::CriticalLoop, false, animationSpeed);
                               
                // �X�e�[�g�ύX
                SetState(STATE::FlinchLoop);
            }

            break;
        case STATE::FlinchLoop:// ���݃��[�v
            // Y�l��0.0f�ŌŒ肷��
            owner_->GetTransform()->SetPositionY(0.0f);

            if (owner_->IsPlayAnimation() == false)
            {
                if (loopCounter_ > maxLoopNum_)
                {
                    owner_->PlayAnimation(Enemy::DragonAnimation::CriticalEnd, false, animationSpeed);

                    // �X�e�[�g�ύX
                    SetState(STATE::FlinchEnd);
                }
                else
                {
                    owner_->PlayAnimation(Enemy::DragonAnimation::CriticalLoop, false, animationSpeed);
                }

                ++loopCounter_;
            }

            break;
        case STATE::FlinchEnd:// ���ݏI���
            // �A�j���[�V�������x�ݒ�
            SetAnimationSpeed();

            if (owner_->IsPlayAnimation() == false)
            {
                // �t���O�����Z�b�g
                owner_->SetIsFlinch(false);
                owner_->SetDownCollisionActiveFlag(false);

                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
            break;
        }

        return ActionBase::State::Run;
    }

    // ----- �A�j���[�V�����̑��x�ݒ� -----
    void FlyFlinchAction::SetAnimationSpeed()
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > 1.5f)
        {
            owner_->SetAnimationSpeed(1.0f);
        }
    }
}

// ----- ��퓬���ҋ@ -----
namespace ActionDragon
{
    const ActionBase::State NonBattleIdleAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// ������
            // �A�j���[�V�����ݒ�
            SetAnimation();

            timer_ = 0.0f;

            owner_->SetStep(1);
            break;
        case 1:

            timer_ += elapsedTime;
            if (timer_ > 3.0f)
            {
                owner_->SetStep(0);
                return ActionBase::State::Failed;
            }

            break;
        }

        return ActionBase::State();
    }

    // ----- �A�j���[�V�����ݒ� -----
    void NonBattleIdleAction::SetAnimation()
    {
        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());

        if (animationIndex == Enemy::DragonAnimation::AttackTurn0)
        {
            //owner_->SetTransitionTime(0.3f);
            owner_->SetTransitionTime(0.15f);
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true);
            //owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true, 1.0f, 0.45f);
            //owner_->PlayAnimation(Enemy::DragonAnimation::Idle0, true);
        }
        else if (animationIndex == Enemy::DragonAnimation::AttackTackle3)
        {
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true);
            owner_->SetTransitionTime(0.15f);
        }
        else
        {
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true);
        }
    }
}

// ----- ��퓬������ -----
namespace ActionDragon
{
    const ActionBase::State NonBattleWalkAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// ������
            // �A�j���[�V�����ݒ�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Walk, true);

            owner_->SetStep(1);
            break;
        case 1:
            break;
        }

        return ActionBase::State();
    }
}

// ----- ���K -----
namespace ActionDragon
{
    const ActionBase::State RoarAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// ������
            // �A�j���[�V�����ݒ�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::BackStepRoar, false);

            // ���W�A���u���[�̃s�N�Z���V�F�[�_�g�p
            PostProcess::Instance().SetUseRadialBlur();

            // ���K�����B
            owner_->SetIsRoar(true);

            // �ϐ�������
            gamePadVibration_.Initialize(1.3f, 1.0f, 1.0f);
            blurTimer_ = 0.0f;

            owner_->SetStep(1);

            break;
        case 1:
            // �R���g���[���[�U��
            gamePadVibration_.Update(owner_->GetAnimationSeconds());

            // �u���[�X�V
            UpdateBlur(elapsedTime);

            if (owner_->IsPlayAnimation() == false)
            {
                // �t���O���Z�b�g
                PostProcess::Instance().SetUseRadialBlur(false);

                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }

    // ----- ���W�A���u���[�X�V -----
    void RoarAction::UpdateBlur(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();
        const float blurStartFrame = 1.3f;
        const float blurEndFrame = 2.3f;
        const float maxBlurTime = 0.2f;
        const float maxBlurPower = 0.03f;

        // �u���[�I���t���[�����߂�����A�u���[���ɂ߂�
        if (animationSeconds > blurEndFrame)
        {
            PostProcess::Instance().GetConstants()->GetData()->blurPower_ =
                Easing::InSine(blurTimer_, maxBlurTime, maxBlurPower, 0.0f);

            blurTimer_ -= elapsedTime;
            blurTimer_ = std::max(blurTimer_, 0.0f);
        }
        // �u���[�J�n�t���[�����߂�����A�u���[��������
        else if (animationSeconds > blurStartFrame)
        {
            PostProcess::Instance().GetConstants()->GetData()->blurPower_ =
                Easing::InQuint(blurTimer_, maxBlurTime, maxBlurPower, 0.0f);

            blurTimer_ += elapsedTime;
            blurTimer_ = std::min(blurTimer_, maxBlurTime);
        }
    }
}

// ----- ���K(�������) -----
namespace ActionDragon
{
    const ActionBase::State RoarLongAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// ������
            // �A�j���[�V�����ݒ�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Roar, false);

            // ���W�A���u���[�̃s�N�Z���V�F�[�_�g�p
            PostProcess::Instance().SetUseRadialBlur();

            // ���K�����B
            owner_->SetIsRoar(true);

            // �ϐ�������
            gamePadVibration_[0].Initialize(1.0f, 0.4f, 0.5f);
            gamePadVibration_[1].Initialize(1.55f, 0.9f, 0.7f);
            gamePadVibration_[2].Initialize(3.0f, 2.0f, 1.0f);

            blurStartFrame_ = 3.0f;
            blurEndFrame_ = 5.0f;
            maxBlurPower_ = 0.01f;
            maxBlurTime_ = 0.2f;
            blurTimer_ = 0.0f;

            isPlayerFilnch_ = false;

            owner_->SetStep(1);
            break;
        case 1:
            // �R���g���[���[�U���X�V
            for (int i = 0; i < vibrationNum_; ++i)
            {
                gamePadVibration_[i].Update(owner_->GetAnimationSeconds());
            }

            if (owner_->GetAnimationSeconds() > 4.1f && isPlayerFilnch_ == false)
            {
                PlayerManager::Instance().GetPlayer()->ChangeState(Player::STATE::Flinch);

                isPlayerFilnch_ = true;
            }

            // ���W�A���u���[�X�V
            UpdateBlur(elapsedTime);

            // �A�j���[�V�������Đ�����������I��
            if (owner_->IsPlayAnimation() == false)
            {
                // �t���O���Z�b�g
                PostProcess::Instance().SetUseRadialBlur(false);

                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

        break;
        }

        return ActionBase::State::Run;
    }

    // ----- ���W�A���u���[�X�V -----
    void RoarLongAction::UpdateBlur(const float& elapsedTime)
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        // �u���[�I���t���[�����߂�����A�u���[���ɂ߂�
        if (animationSeconds > blurEndFrame_)
        {
            PostProcess::Instance().GetConstants()->GetData()->blurPower_ =
                Easing::InSine(blurTimer_, maxBlurTime_, maxBlurPower_, 0.0f);

            blurTimer_ -= elapsedTime;
            blurTimer_ = std::max(blurTimer_, 0.0f);
        }
        // �u���[�J�n�t���[�����߂�����A�u���[��������
        else if (animationSeconds > blurStartFrame_)
        {
            PostProcess::Instance().GetConstants()->GetData()->blurPower_ =
                Easing::InQuint(blurTimer_, maxBlurTime_, maxBlurPower_, 0.0f);

            blurTimer_ += elapsedTime;
            blurTimer_ = std::min(blurTimer_, maxBlurTime_);
        }

        // �A�j���[�V�����̃^�C�~���O�ɍ��킹�ău���[�������P�i�K��������
        if (animationSeconds > 4.1f)
        {
            maxBlurPower_ = 0.03f;
        }
    }
}

// ----- �o�b�N�X�e�b�v�U�� -----
namespace ActionDragon
{
    const ActionBase::State BackStepAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:
            //owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackFly0, false);
            //owner_->PlayBlendAnimation(Enemy::DragonAnimation::BackStep, false);
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::CriticalLoop, false);

            owner_->SetStep(1);
            break;
        case 1:

            if (owner_->IsPlayAnimation() == false)
            {
                //owner_->SetStep(0);
                //
                //return ActionBase::State::Failed;
            }

            break;
        }

        return ActionBase::State();
    }
}

// ----- �󒆂��炽�������U�� -----
namespace ActionDragon
{
    const ActionBase::State FlyAttackAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// ������

            // �A�j���[�V�����ݒ�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackFly0, false);

            // ���[�g���[�V�����͎g��Ȃ�
            owner_->SetUseRootMotion(false);

            // �ϐ�������
            {
                addForceData_[MoveDirection::UpBack].Initialize(0.35f, 0.4f, 1.0f);
                //addForceData_[MoveDirection::DownForward].Initialize(0.0f, 0.7f, 1.0f);
                addForceData_[MoveDirection::DownForward].Initialize(0.0f, 0.65f, 1.0f);

                // SlowAnimation
                slowAnimationSpeed_ = 0.7f;
                slowAnimationEndFrame_ = 0.12f;

                // �\������p
                easingTimer_ = 0.0f;
                isDown_ = false;
                isRise_ = false;
            }

            // �X�e�[�g�ύX
            SetStep(STATE::FlyStart);

            break;
        case STATE::FlyStart:// ��юn��
        {
            // ��]����
            const float turnStartFrame = 0.6f;
            if (owner_->GetAnimationSeconds() > turnStartFrame)
            {
                owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());
            }

            // ���΂ߏ�����Ɉړ�������
            if (addForceData_[static_cast<int>(MoveDirection::UpBack)].Update(owner_->GetAnimationSeconds()))
            {
                DirectX::XMFLOAT3 direction = XMFloat3Normalize(owner_->GetTransform()->CalcForward() * -1 - owner_->GetTransform()->CalcUp() * -1);
                owner_->AddForce(direction, addForceData_[static_cast<int>(MoveDirection::UpBack)].GetForce(), addForceData_[static_cast<int>(MoveDirection::UpBack)].GetDecelerationForce());
            }

            // �A�j���[�V�����Đ�����������
            if (owner_->IsPlayAnimation() == false)
            {
                // ���̃A�j���[�V������ݒ肷��
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackFly1, false);

                // ���݂̈ʒuY��ۑ�����
                savePositionY_ = owner_->GetTransform()->GetPositionY();

                // �X�e�[�g�ύX
                SetStep(STATE::PreAction);
                break;
            }
        }
            break;
        case STATE::PreAction:// �\������
        {
            // ��]����
            const float turnEndFrame = 1.3f;
            if (owner_->GetAnimationSeconds() < turnEndFrame)
            {
                owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());
            }

            // �\������Ƃ��ĉ��ɂ�����������
            if (isDown_ == false)
            {
                const float totalTime = 0.3f;
                addPositionY_ = Easing::InSine(easingTimer_, totalTime, -2.5f, 0.0f);
                easingTimer_ += elapsedTime;

                owner_->SetAnimationSpeed(0.25f);

                if (easingTimer_ > totalTime)
                {
                    easingTimer_ = 0.0f;
                    isDown_ = true;
                }
            }
            // ���ɉ���������A�㏸����
            else if (isRise_ == false)
            {
                const float totalTime = 0.4f;
                addPositionY_ = Easing::InCubic(easingTimer_, totalTime, 4.0f, -2.5f);
                easingTimer_ += elapsedTime;

                owner_->SetAnimationSpeed(1.0f);
                if (easingTimer_ > totalTime)
                {
                    easingTimer_ = 0.0f;
                    isRise_ = true;
                }
            }

            // �ړ��lY�̐ݒ�
            owner_->GetTransform()->SetPositionY(savePositionY_ + addPositionY_);

            // �㏸�I�����Ă����玟�ɂ�����
            if (isRise_)
            {
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackFly2, false, slowAnimationSpeed_);

                // �X�e�[�g�ύX
                SetStep(STATE::FlyAttack);
                break;
            }
        }
            break;
        case STATE::FlyAttack:// �U��

            // �U�����菈��
            if (owner_->GetAnimationSeconds() > 0.4f)
            {
                if(owner_->GetIsAttackActive()) 
                    owner_->SetFlyAttackActiveFlag(false);
            }
            else if (owner_->GetAnimationSeconds() > 0.1f)
            {
                if(owner_->GetIsAttackActive() == false) 
                    owner_->SetFlyAttackActiveFlag();
            }

            // �A�j���[�V�������w��̃t���[���𒴂�����
            if (owner_->GetAnimationSeconds() > slowAnimationEndFrame_)
            {
                // �A�j���[�V�������x�ݒ�
                owner_->SetAnimationSpeed(1.0f);
            }
            
            // �O���΂߉��Ɉړ�����
            if (addForceData_[static_cast<int>(MoveDirection::DownForward)].Update(owner_->GetAnimationSeconds()))
            {
                DirectX::XMFLOAT3 direction = XMFloat3Normalize(owner_->GetTransform()->CalcForward() - owner_->GetTransform()->CalcUp());
                owner_->AddForce(direction, addForceData_[static_cast<int>(MoveDirection::DownForward)].GetForce(), addForceData_[static_cast<int>(MoveDirection::DownForward)].GetDecelerationForce());
            }

            // �ړ��lY�̐��� (0.0f��艺�ɍs���Ȃ��悤�ɂ���)
            float posY = std::max(0.0f, owner_->GetTransform()->GetPositionY());
            owner_->GetTransform()->SetPositionY(posY);

            // �A�j���[�V�������Đ�����������I��
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->GetTransform()->SetPositionY(0.0f);
                
                // �X�e�[�g���Z�b�g
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
            break;
        }
        return ActionBase::State::Run;
    }
}

// ----- ������΂��U�� -----
namespace ActionDragon
{
    const ActionBase::State KnockBackAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// ������
            // �A�j���[�V�����ݒ�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackKnockBackStart, false);

            // ���[�g���[�V�������g�p����
            owner_->SetUseRootMotion(true);

            // �ϐ�������
            {
                loopMax_ = rand() % 3;
                //loopMax_ = rand() % 3;
                loopCounter_ = 0;
            }

            // �X�e�[�g�ύX
            SetState(STATE::Guard);

            break;
        case STATE::Guard:// �K�[�h
            
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackKnockBackLoop, false);
                
                // �X�e�[�g�ύX
                SetState(STATE::Loop);
                break;
            }

            break;
        case STATE::Loop:// ���[�v

            // �A�j���[�V�����Đ��I��
            if (owner_->IsPlayAnimation() == false)
            {
                // �܂����[�v����񐔂��c���Ă���
                if (loopCounter_ < loopMax_)
                {
                    // �X�e�[�g�ύX
                    SetState(STATE::LoopInit);
                    break;
                }
                // ���[�v�I��
                else
                {
                    owner_->PlayAnimation(Enemy::DragonAnimation::AttackKnockBackEnd0, false);
                    
                    // �X�e�[�g�ύX
                    SetState(STATE::Attack);
                }
                
                break;
            }
            break;
        case STATE::LoopInit:// ���[�v������
            // �A�j���[�V�����ݒ�
            owner_->PlayAnimation(Enemy::DragonAnimation::AttackKnockBackLoop, false);

            ++loopCounter_;

            // �X�e�[�g�ύX
            SetState(STATE::Loop);

            break;
        case STATE::Attack:// �U������
            if (owner_->IsPlayAnimation() == false)
            {
                // �X�e�[�g���Z�b�g
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        case STATE::Failed:// �U�����s
            break;
        }

        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State SlamAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State FrontAttackAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        return ActionBase::State();
    }
}

// ----- �R���{���������U�� -----
namespace ActionDragon
{
    const ActionBase::State ComboSlamAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// ������
            // �A�j���[�V�����Đ�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackComboSlam0, false);

            // ���[�g���[�V�������g�p����
            owner_->SetUseRootMotion(true);

            // �ϐ�������
            addForceData_.Initialize(1.0f, 0.4f, 1.0f);

            owner_->SetStep(1);

            break;
        case 1:

            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                DirectX::XMFLOAT3 vec = owner_->CalcDirectionToPlayer();
                vec = XMFloat3Normalize({ vec.x, 0.0f, vec.z });
                owner_->AddForce(vec, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            if (owner_->GetAnimationSeconds() > 0.7f &&
                owner_->GetAnimationSeconds() < 1.3f)
            {
                owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());
            }

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackComboSlam1, false);

                owner_->SetStep(2);
            }

            // �U�����菈��
            if (owner_->GetAnimationSeconds() > 1.55f)
            {
                if (owner_->GetIsAttackActive())
                    owner_->SetComboSlamAttackActiveFlag(false);
            }
            else if (owner_->GetAnimationSeconds() > 1.4f)
            {
                if (owner_->GetIsAttackActive() == false)
                    owner_->SetComboSlamAttackActiveFlag();
            }

            break;
        case 2:

            // �U�����菈��
            if (owner_->GetAnimationSeconds() > 0.75f)
            {
                if (owner_->GetIsAttackActive())
                    owner_->SetComboSlamAttackActiveFlag(false);
            }
            else if (owner_->GetAnimationSeconds() > 0.6f)
            {
                if (owner_->GetIsAttackActive() == false)
                    owner_->SetComboSlamAttackActiveFlag();
            }


            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackComboSlamEnd, false);
                owner_->SetStep(3);
            }

            break;
        case 3:

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State ComboChargeAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        return ActionBase::State();
    }
}

// ----- ��]�U�� -----
namespace ActionDragon
{
    const ActionBase::State TurnAttackAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// ������
            // �A�j���[�V�����ݒ�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackTurnStart, false);

            // ���[�g���[�V�������g�p����
            owner_->SetUseRootMotion(true);

            // �J�E���^�[�L���͈͂�ݒ肷��
            PlayerManager::Instance().GetPlayer()->SetCounterActiveRadius(6.0f);

            // �ϐ�������
            addForceData_.Initialize(1.5f, 0.3f, 0.5f);
 
            // �X�e�[�g�ύX
            SetState(STATE::PreAction);

            break;
        case STATE::PreAction:// �\������

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackTurn0, false);
                SetState(STATE::Attack);
            }

            break;
        case STATE::Attack:// �U��

            // �ړ�����
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }


            if (owner_->GetAnimationSeconds() > 2.2f)
            {
                if (owner_->GetIsAttackActive())
                    owner_->SetTurnAttackActiveFlag(false);
            }
            else if (owner_->GetAnimationSeconds() > 1.2f)
            {
                if (owner_->GetIsAttackActive() == false) 
                    owner_->SetTurnAttackActiveFlag();
            }

            if (owner_->GetAnimationSeconds() > 3.3f)
            //if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
            break;
        }

        return ActionBase::State::Run;
    }
}

// ----- �^�b�N���U�� -----
namespace ActionDragon
{
    const ActionBase::State TackleAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// ������
            // �A�j���[�V�����ݒ�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackTackle0, false);

            // ���[�g���[�V�����g�p
            owner_->SetUseRootMotion(true);

            // �J�E���^�[�L���͈͂�ݒ肷��
            PlayerManager::Instance().GetPlayer()->SetCounterActiveRadius(6.0f);

            // �ϐ�������
            addForceData_.Initialize(0.15f, 0.8f, 0.6f);
            easingTimer_ = 0.0f;

            // �X�e�[�g�ύX
            SetState(STATE::PreAction);

            break;
        case STATE::PreAction:// �\������
        {
            const float animationSeconds = owner_->GetAnimationSeconds();

            // ��]���� ( �v���C���[�̕����Ɍ��� )
            const float turnStateFrame = 0.3f;
            if (animationSeconds > turnStateFrame)
            {
                owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());
            }

            // �A�j���[�V�����Đ��I��
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackTackle1, false);
                SetState(STATE::Tackle);
            }
        }
            break;
        case STATE::Tackle:// �^�b�N��
        {
            // �ړ�����
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                const DirectX::XMFLOAT3 vec = XMFloat3Normalize(owner_->CalcDirectionToPlayer());

                owner_->AddForce(vec, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }
            // ��]����
            if (addForceData_.GetIsAddForce() == false)
            {
                owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());
            }

            // �U������L����
            if (owner_->GetIsAttackActive() == false)
            {
                const float attackActiveStartFrame = 0.2f;
                if (owner_->GetAnimationSeconds() > attackActiveStartFrame)
                {
                    owner_->SetTackleAttackActiveFlag();
                }
            }

            // �O�������܂��Ă��܂��̂ŉ�]�Ŗ�����肵�Ă�
            {
                const float maxAngle = -10.0f;
                if (owner_->GetAnimationSeconds() > 0.65f)
                {
                    const float totalFrame = 0.1f;
                    const float angle = Easing::InSine(easingTimer_, totalFrame, maxAngle, 0.0f);
                    easingTimer_ -= elapsedTime;
                    easingTimer_ = std::max(easingTimer_, 0.0f);
                    owner_->GetTransform()->SetRotationX(DirectX::XMConvertToRadians(angle));
                }
                else if (owner_->GetAnimationSeconds() > 0.45f)
                {
                    const float totalFrame = 0.1f;
                    const float angle = Easing::InSine(easingTimer_, totalFrame, maxAngle, 0.0f);
                    easingTimer_ += elapsedTime;
                    easingTimer_ = std::min(easingTimer_, totalFrame);
                    owner_->GetTransform()->SetRotationX(DirectX::XMConvertToRadians(angle));
                }
            }

            // �A�j���[�V�����Đ��I��
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->GetTransform()->SetPositionY(0);

                addForceData_.Initialize(0.03f, 0.6f, 0.8f);

                owner_->PlayAnimation(Enemy::DragonAnimation::AttackTackle3, false);
                SetState(STATE::Recovery);
            }
        }
            break;
        case STATE::Recovery:// �㌄ ( �r���܂ōU�����肠�� )
        {
            // �U�����薳����
            if (owner_->GetIsAttackActive())
            {
                if (owner_->GetAnimationSeconds() > 0.6f)
                {
                    owner_->SetTackleAttackActiveFlag(false);
                }
            }

            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            // �A�j���[�V�����I��
            const float animationEndFrame = 1.9f;
            if (owner_->GetAnimationSeconds() > animationEndFrame)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
        }
            break;
        }

        return ActionBase::State::Run;
    }
}

// ----- �㏸�U�� -----
namespace ActionDragon
{
    const ActionBase::State RiseAttackAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// ������
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackRise, false);

            // �ϐ�������
            addForceData_.Initialize(1.9f, 1.5f, 2.0f);
            riseTimer_ = 0.0f;
            isCameraSet_ = false;
            isCameraReset_ = false;

            owner_->GetTransform()->SetPositionY(0);

            owner_->SetStep(1);

            break;
        case 1:// �\������

            if(owner_->GetAnimationSeconds() > 1.85f && isCameraSet_ == false)
            {
                // �J�����ݒ�
                //Camera::Instance().SetRiseAttackState(0);
                isCameraSet_ = true;
            }

            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                owner_->AddForce({ 0,1,0 }, addForceData_.GetForce(), addForceData_.GetDecelerationForce());


            }

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackRiseLoop, true);
                owner_->SetStep(2);
            }

            break;
        case 2:// �㏸
        {
            if (riseTimer_ < 1.0f)
            {
                owner_->SetIsStageCollisionJudgement(true);
                DirectX::XMFLOAT3 playerPos = PlayerManager::Instance().GetTransform()->GetPosition();
                owner_->GetTransform()->SetPositionX(playerPos.x);
                owner_->GetTransform()->SetPositionZ(playerPos.z);
            }

            riseTimer_ += elapsedTime;
            if (riseTimer_ > 2.0f)
            {
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackRiseEnd, false);
                owner_->SetStep(3);
                owner_->SetIsStageCollisionJudgement(false);
            }
        }
            break;
        case 3:
        {

            // �A�j���[�V�������x�ݒ�
            /*if (owner_->GetAnimationSeconds() < 1.0f)
            {
                owner_->SetAnimationSpeed(0.6f);

            }
            else if (owner_->GetAnimationSeconds() < 1.6f)            
            {
                owner_->SetAnimationSpeed(0.8f);
            }            
            else
            {
                owner_->SetAnimationSpeed(1.0f);
            }*/

            float ownerPosY = owner_->GetTransform()->GetPositionY();
            const float moveSpeed = 100.0f * elapsedTime;
            ownerPosY -= moveSpeed;
            ownerPosY = std::max(ownerPosY, 0.0f);
            owner_->GetTransform()->SetPositionY(ownerPosY);

            if (owner_->GetAnimationSeconds() > 0.6f && isCameraReset_ == false)
            {
                // �J�����ݒ�
                //Camera::Instance().SetRiseAttackState(-1);
                isCameraReset_ = true;
            }

            if (owner_->IsPlayAnimation() == false)
            {


                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
        }
            break;
        }

        return ActionBase::State::Run;
    }
}

namespace ActionDragon
{
    const ActionBase::State MoveTurnAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        return ActionBase::State();
    }
}

namespace ActionDragon
{
    const ActionBase::State MoveAttackAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        return ActionBase::State();
    }
}
