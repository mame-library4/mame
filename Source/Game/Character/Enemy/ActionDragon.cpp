#include "ActionDragon.h"
#include "PostProcess/PostProcess.h"
#include "Input.h"
#include "Easing.h"

// ----- ���S�s�� -----
const ActionBase::State DeathAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:// ������
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::Death, false);

        owner_->SetStep(1);
        break;
    case 1:
        // �A�j���[�V�������Ō�܂ōĐ����ꂽ��
        if (owner_->IsPlayAnimation() == false)
        {
            // ���S���[�v���[�V�����𗬂�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::DeathLoop, false);

            owner_->SetStep(2);
            break;
        }

        break;
    case 2:
        // ���S���Ă���̂ł����ŕ��u
        break;
    }

    return ActionBase::State::Run;
}

// ----- ���ݍs�� -----
const ActionBase::State FlinchAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

// ----- ��퓬���ҋ@ -----
const ActionBase::State NonBattleIdleAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:// ������
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true);

        timer_ = 0.0f;

        owner_->SetStep(1);
        break;
    case 1:
        
        timer_ += elapsedTime;
        if(timer_ > 1.0f)
        {
            owner_->SetStep(0);
            return ActionBase::State::Failed;
        }

        break;
    }

    return ActionBase::State();
}

// ----- ��퓬������ -----
const ActionBase::State NonBattleWalkAction::Run(const float& elapsedTime)
{
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

// ----- ���K -----
const ActionBase::State RoarAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:// ������
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::Roar, false);

        // ���W�A���u���[�̃s�N�Z���V�F�[�_�g�p
        owner_->SetIsRoar(true);

        // �ϐ�������
        isVibration_[0] = false;
        isVibration_[1] = false;
        isVibration_[2] = false;
#define Normal 0
#if Normal
        blurStartFrame_ = 4.1f;
        blurEndFrame_ = 5.0f;
        maxBlurPower_ = 0.03f;
        maxBlurTime_ = 0.2f;
        blurTimer_ = 0.0f;
#else
        blurStartFrame_ = 3.0f;
        blurEndFrame_ = 5.0f;
        maxBlurPower_ = 0.01f;
        maxBlurTime_ = 0.2f;
        blurTimer_ = 0.0f;
#endif

        owner_->SetStep(1);
        break;
    case 1:
    {
        const float animationSeconds = owner_->GetAnimationSeconds();

        if (animationSeconds > vibrationStartFrame_[0] && isVibration_[0] == false)
        {
            Input::Instance().GetGamePad().Vibration(0.4f, 0.5f);
            isVibration_[0] = true;
        }
        if (animationSeconds > vibrationStartFrame_[1] && isVibration_[1] == false)
        {
            Input::Instance().GetGamePad().Vibration(0.9f, 0.7f);
            isVibration_[1] = true;
        }
        if (animationSeconds > vibrationStartFrame_[2] && isVibration_[2] == false)
        {
            Input::Instance().GetGamePad().Vibration(2.0f, 1.0f);
            isVibration_[2] = true;
        }

        // ���W�A���u���[
        if (animationSeconds > blurEndFrame_)
        {
            PostProcess::Instance().GetConstants()->GetData()->blurPower_ =
                Easing::InSine(blurTimer_, maxBlurTime_, maxBlurPower_, 0.0f);

            blurTimer_ -= elapsedTime;
            blurTimer_ = std::max(blurTimer_, 0.0f);
        }
        else if(animationSeconds > blurStartFrame_)
        {
            PostProcess::Instance().GetConstants()->GetData()->blurPower_ =
                Easing::InQuint(blurTimer_, maxBlurTime_, maxBlurPower_, 0.0f);

            blurTimer_ += elapsedTime;
            blurTimer_ = std::min(blurTimer_, maxBlurTime_);
        }

#if Normal
#else
        if (animationSeconds > 4.1f)
        {
            maxBlurPower_ = 0.03f;
        }
#endif


        if (owner_->IsPlayAnimation() == false)
        {
            // �t���O���Z�b�g
            owner_->SetIsRoar(false);

            owner_->SetStep(0);
            return ActionBase::State::Complete;
        }

    }
        break;
    }

    return ActionBase::State::Run;
}

// ----- �o�b�N�X�e�b�v���K -----
const ActionBase::State BackStepRoarAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:// ������
        // �A�j���[�V�����ݒ�
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::BackStepRoar, false);
        
        owner_->SetStep(1);
        
        break;
    case 1:

        if (owner_->IsPlayAnimation() == false)
        {
            // �t���O���Z�b�g
            owner_->SetIsRoar(false);

            owner_->SetStep(0);
            return ActionBase::State::Complete;
        }

        break;
    }
    
    return ActionBase::State::Run;
}

const ActionBase::State BackStepAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State FlyAttackAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State KnockBackAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State SlamAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State FrontAttackAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State ComboSlamAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State ComboChargeAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State TurnAttackAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State TackleAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State RiseAttackAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State MoveTurnAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State MoveAttackAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}
