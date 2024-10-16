#include "ActionDragon.h"
#include "PostProcess/PostProcess.h"
#include "Easing.h"
#include "Input.h"
#include "Camera.h"
#include "../Player/PlayerManager.h"

#include "Projectile/ProjectileManager.h"
#include "Projectile/Fireball.h"

#include "Particle/ParticleManager.h"
#include "Common.h"

#include "sprite.h"
#include "Application.h"

#include "UI/UIFlashOut.h"

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

// ----- SuperNova -----
namespace ActionDragon
{
    const ActionBase::State SuperNovaAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange())
        {
            Finalize();

            return ActionBase::State::Failed;
        }

        switch (owner_->GetStep())
        {
        case 0:
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Nova1, false);

            // �ϐ�������
            superNovaParticle_ = new SuperNovaParticle();
            
            isCreateLavaCrawlerParticle_ = false;
            isCreateCoreBurst_           = false;

            scaleLerpTimer_ = 0.0f;

            radialBlurTimer_ = 0.0f;
            preRadialBlurTimer_ = 0.0f;
            intenseBlurFrameCount_ = 0;

            owner_->SetStep(1);

            break;
        case 1:

            // �`���[�W�G�t�F�N�g����
            GenerateChargeEffect(elapsedTime);

            // �`���[�W�G�t�F�N�g�X�V
            UpdateChargeEffect(elapsedTime);

            // ���C���G�t�F�N�g����
            GenarateMainEffect(elapsedTime);

            // ���W�A���u���[�X�V
            UpdateRadialBlur(elapsedTime);


            if (owner_->IsPlayAnimation() == false)
            {
                owner_->SetStep(0);

                Finalize();

                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }

    void SuperNovaAction::DrawDebug()
    {
    }

    // ----- �`���[�W�G�t�F�N�g���� -----
    void SuperNovaAction::GenerateChargeEffect(const float& elapsedTime)
    {
        // �������ɐ������Ă��邽�߂����ŏI��
        if (isCreateLavaCrawlerParticle_) return;

        // �����t���[���ɒB���Ă��Ȃ�
        if (owner_->GetAnimationSeconds() <= 0.65f) return;

        // �`���[�W�G�t�F�N�g�ƃp�[�e�B�N�����Đ�����
        DirectX::XMFLOAT3 emitterPosition = owner_->GetJointPosition("Dragon15_neck_1");
        powerEffectHandle_ = EffectManager::Instance().GetEffect("Power")->Play(emitterPosition, 0.1f, 1.0f);
        
        emitterPosition.y = 0.3f;
        superNovaParticle_->PlayLavaCrawlerParticle(elapsedTime, emitterPosition);

        isCreateLavaCrawlerParticle_ = true;
    }

    // ----- ���C���G�t�F�N�g���� -----
    void SuperNovaAction::GenarateMainEffect(const float& elapsedTime)
    {
        // �������Ƀ��C���G�t�F�N�g����������Ă���
        if (isCreateCoreBurst_) return;
        // �����t���[���ɒB���Ă��Ȃ�
        if (owner_->GetAnimationSeconds() <= 3.9f) return;
        
        // �`���[�W�G�t�F�N�g���~����
        EffectManager::Instance().GetEffect("Power")->Stop(powerEffectHandle_);

        const DirectX::XMFLOAT3 emitterPosition = owner_->GetJointPosition("Dragon15_neck_1");

        // �����p�[�e�B�N���Đ�
        superNovaParticle_->PlayCoreBurstParticle(elapsedTime, emitterPosition);
        // �����G�t�F�N�g�Đ�
        Effect* superNovaEffect = EffectManager::Instance().GetEffect("SuperNova");
        superNovaEffect->Play(emitterPosition, 1.3f, 1.0f);

        // ����щ摜�𐶐�
        UIFlashOut* uiFlashOut = new UIFlashOut();

        // �J�����V�F�C�N
        Camera::Instance().ScreenVibrate(0.3f, 1.5f);

        // �n�ʂ𔇂��p�[�e�B�N���̑��x���グ��
        superNovaParticle_->SetLavaCrawlerParticleSpeed(30.0f);

        isCreateCoreBurst_ = true;
    }

    // ----- �`���[�W�G�t�F�N�g�X�V -----
    void SuperNovaAction::UpdateChargeEffect(const float& elapsedTime)
    {
        // �܂��`���[�W�G�t�F�N�g����������Ă��Ȃ�
        if (isCreateLavaCrawlerParticle_ == false) return;
        // ���C���̔����G�t�F�N�g����������Ă���̂ŏ������Ȃ�
        if (isCreateCoreBurst_) return;

        PostProcess::Instance().SetUseRadialBlur();
        preRadialBlurTimer_ += elapsedTime;
        preRadialBlurTimer_ = std::min(preRadialBlurTimer_, 1.0f);

        // offset���g���ĐU����\������
        const float strength = XMFloatLerp(0.0f, 0.1f, preRadialBlurTimer_);
        const float offset = rand() % 10 * 0.01f;

        PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = strength + offset;
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = maxSampleCount_;

        // �u���[�̊J�n���S�_�����߂�
        const DirectX::XMFLOAT3 dragonNeckPosition = owner_->GetJointPosition("Dragon15_neck_1");
        DirectX::XMFLOAT2 centerPosition = Sprite::ConvertToScreenPos(dragonNeckPosition);
        centerPosition.x /= SCREEN_WIDTH;
        centerPosition.y /= SCREEN_HEIGHT;

        // ��ʊO�ɂ���Ƃ��͔����n�_����ʒ����ɂ���
        if (centerPosition.x > 1.0f || centerPosition.y > 1.0f ||
            centerPosition.x < 0.0f || centerPosition.y < 0.0f)
        {
            centerPosition = { 0.5f, 0.5f };
        }
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->uvOffset_ = centerPosition;

        // �G�t�F�N�g�̃T�C�Y�����X�ɑ傫������
        const float speed = 0.4f;
        scaleLerpTimer_ += speed * elapsedTime;
        scaleLerpTimer_ = std::min(scaleLerpTimer_, 1.0f);

        const float scale = XMFloatLerp(0.1f, 7.0f, scaleLerpTimer_);
        EffectManager::Instance().GetEffect("Power")->SetScale(powerEffectHandle_, scale);
    }

    // ----- ���W�A���u���[�X�V -----
    void SuperNovaAction::UpdateRadialBlur(const float& elapsedTime)
    {
        // ���C���̔����p�[�e�B�N������������Ă��Ȃ��̂ōX�V���Ȃ�
        if (isCreateCoreBurst_ == false) return;

        // ���W�A���u���[���g�p���� �T���v�����O�񐔂͂T��
        PostProcess::Instance().SetUseRadialBlur();
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = maxSampleCount_;

        // �u���[�̊J�n���S�_�����߂�
        const DirectX::XMFLOAT3 dragonNeckPosition = owner_->GetJointPosition("Dragon15_neck_1");
        DirectX::XMFLOAT2 centerPosition = Sprite::ConvertToScreenPos(dragonNeckPosition);
        centerPosition.x /= SCREEN_WIDTH;
        centerPosition.y /= SCREEN_HEIGHT;

        // �u���[�̊J�n�_�� 0.0 ~ 1.0 �𒴂��Ă����ꍇ�^�񒆂ɕ␳����
        if (centerPosition.x > 1.0f || centerPosition.y > 1.0f ||
            centerPosition.x < 0.0f || centerPosition.y < 0.0f)
        {
            centerPosition = { 0.5f, 0.5f };
        }
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->uvOffset_ = centerPosition;

        // �u���[�J�n�̐��t���[�����ő勭�x�̃u���[��������
        if (intenseBlurFrameCount_ < intenseBlurFrame_)
        {
            PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = 1.5f;
            ++intenseBlurFrameCount_;
            return;
        }

        // ���W�A���u���[�̋��x�����X�ɉ�����
        if (radialBlurTimer_ < 2.0f)
        {
            radialBlurTimer_ += elapsedTime;
            radialBlurTimer_ = std::min(radialBlurTimer_, 1.0f);

            const float strength = XMFloatLerp(1.0f, 0.0f, radialBlurTimer_);

            PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = strength;

            return;
        }

        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 1;
    }

    // ----- �I������ -----
    void SuperNovaAction::Finalize()
    {
        if (superNovaParticle_ != nullptr)
        {
            ParticleManager::Instance().Remove(superNovaParticle_);
            superNovaParticle_ = nullptr;
        }
        
        EffectManager::Instance().GetEffect("Power")->Stop(powerEffectHandle_);
        
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 1;
    }
}

// ----- ���S�s�� -----
namespace ActionDragon
{
    const ActionBase::State DeathAction::Run(const float& elapsedTime)
    {
        if (owner_->GetStep() == 0)
        {

            owner_->SetStep(1);
        }

        return ActionBase::State::Run;
    }
    void DeathAction::DrawDebug()
    {
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
            SetAnimation();

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

    void FlinchAction::DrawDebug()
    {
    }

    // ----- �A�j���[�V�����ݒ� -----
    void FlinchAction::SetAnimation()
    {
#if 0
        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());

        if (animationIndex == Enemy::DragonAnimation::AttackTackle3)
        {
            owner_->SetTransitionTime(0.2f);
        }
        else
        {
            owner_->SetTransitionTime(0.1f);
        }
#endif
        owner_->SetTransitionTime(0.1f);

        owner_->PlayBlendAnimation(Enemy::DragonAnimation::CriticalStart, false);
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

    void FlyFlinchAction::DrawDebug()
    {
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

namespace ActionDragon
{
    const ActionBase::State PartDestructionFlinchAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// �����ݒ�
            // �A�j���[�V�����ݒ�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::GetHitStart, false, 1.0f, 0.1f);
            owner_->SetTransitionTime(0.15f);

            owner_->SetUseRootMotion(false);

            owner_->SetStep(1);

            break;
        case 1:
            if (owner_->GetUseRootMotionMovement() == false)
            {
                if (owner_->GetIsBlendAnimation() == false) owner_->SetUseRootMotion(true);
            }

            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::GetHitLoop, false);

                owner_->SetUseRootMotion(false);

                owner_->SetStep(2);
            }

            break;
        case 2:
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::GetHitEnd, false);

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

        return ActionBase::State::Run;
    }
    void PartDestructionFlinchAction::DrawDebug()
    {
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

            // ���[�g���[�V�������g�p���Ȃ�
            owner_->SetUseRootMotion(false);

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

    void NonBattleIdleAction::DrawDebug()
    {
    }

    // ----- �A�j���[�V�����ݒ� -----
    void NonBattleIdleAction::SetAnimation()
    {
        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());

        // �ʏ�A�j���[�V�������Đ�����
        if (animationIndex == Enemy::DragonAnimation::AttackTackle3         || // �ːi�U��
            animationIndex == Enemy::DragonAnimation::AttackKnockBackEnd0   || // ������΂��U��
            animationIndex == Enemy::DragonAnimation::AttackFly2            || // �󒆂��炽�������U��
            animationIndex == Enemy::DragonAnimation::FireBreathFront       || // �u���X 
            animationIndex == Enemy::DragonAnimation::AttackRiseEnd            // �㏸�U��
            )
        {
            owner_->PlayAnimation(Enemy::DragonAnimation::Idle0, true);
            return;
        }

        // ��]�U��
        if (animationIndex == Enemy::DragonAnimation::AttackTurn)
        {
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true, 1.0f, 0.2f);
            owner_->SetTransitionTime(0.2f);
            return;
        }

        // ���������U��
        if (animationIndex == Enemy::DragonAnimation::AttackComboSlamEnd)
        {
            owner_->SetTransitionTime(0.1f);
        }

        owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true);
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
    void NonBattleWalkAction::DrawDebug()
    {
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

    void RoarAction::DrawDebug()
    {
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
            PostProcess::Instance().GetConstants()->GetData()->dummy_ =
                Easing::InSine(blurTimer_, maxBlurTime, maxBlurPower, 0.0f);

            blurTimer_ -= elapsedTime;
            blurTimer_ = std::max(blurTimer_, 0.0f);
        }
        // �u���[�J�n�t���[�����߂�����A�u���[��������
        else if (animationSeconds > blurStartFrame)
        {
            PostProcess::Instance().GetConstants()->GetData()->dummy_ =
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
            PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 5;
            PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = 0.0f;

            // ���K�����B
            owner_->SetIsRoar(true);

            // �ϐ�������
            gamePadVibration_[0].Initialize(1.0f, 0.4f, 0.5f);
            gamePadVibration_[1].Initialize(1.55f, 0.9f, 0.7f);
            gamePadVibration_[2].Initialize(3.0f, 2.0f, 1.0f);

            intenseBlurFrameCount_  = 0;
            radialBlurTimer_        = 0.0f;

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
                PostProcess::Instance().GetRadialBlurConstants()->GetData()->sampleCount_ = 1;

                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

        break;
        }

        return ActionBase::State::Run;
    }

    void RoarLongAction::DrawDebug()
    {
        ImGui::DragFloat("RadialBlurTimer", &radialBlurTimer_);
        ImGui::DragFloat("RadialBlurSpeed", &radialBlurSpeed_);
        ImGui::DragInt("IntenseBlurFrame", &intenseBlurFrame_);
        ImGui::DragInt("IntenseBlurFrameCounter", &intenseBlurFrameCount_);
        ImGui::DragFloat("MaxStrength", &maxStrength_);
    }

    // ----- ���W�A���u���[�X�V -----
    void RoarLongAction::UpdateBlur(const float& elapsedTime)
    {
        // �u���[�̊J�n���S�_�����߂�
        const DirectX::XMFLOAT3 dragonNeckPosition = owner_->GetJointPosition("Dragon15_neck_1");
        DirectX::XMFLOAT2 centerPosition = Sprite::ConvertToScreenPos(dragonNeckPosition);
        centerPosition.x /= SCREEN_WIDTH;
        centerPosition.y /= SCREEN_HEIGHT;

        // �u���[�̊J�n�_�� 0.0 ~ 1.0 �𒴂��Ă����ꍇ�^�񒆂ɕ␳����
        if (centerPosition.x > 1.0f || centerPosition.y > 1.0f ||
            centerPosition.x < 0.0f || centerPosition.y < 0.0f)
        {
            centerPosition = { 0.5f, 0.5f };
        }
        PostProcess::Instance().GetRadialBlurConstants()->GetData()->uvOffset_ = centerPosition;



        if (owner_->GetAnimationSeconds() > 4.1f)
        {
            // �u���[�J�n�̐��t���[�����ő勭�x�̃u���[��������
            if (intenseBlurFrameCount_ < intenseBlurFrame_)
            {
                PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = maxStrength_;
                ++intenseBlurFrameCount_;
                return;
            }
            else
            {
                radialBlurTimer_ += radialBlurSpeed_ * elapsedTime;
                radialBlurTimer_ = std::min(radialBlurTimer_, 1.0f);
                const float strength = XMFloatLerp(maxStrength_, 0.0f, radialBlurTimer_);

                PostProcess::Instance().GetRadialBlurConstants()->GetData()->strength_ = strength;
            }
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
    void BackStepAction::DrawDebug()
    {
    }
}

// ----- FlyAttackAction -----
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
            SetAnimation();

            // �_���[�W�ݒ�
            owner_->SetAttackDamage(50.0f);

            // ���[�g���[�V�������g�p���Ȃ�
            owner_->SetUseRootMotion(false);

            // �J�E���^�[�L���͈͂�ݒ肷��
            PlayerManager::Instance().GetPlayer()->SetCounterActiveRadius(6.0f);

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


            if(owner_->GetAnimationSeconds() > 2.55f)
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

    void FlyAttackAction::DrawDebug()
    {
    }

    // ----- �A�j���[�V�����ݒ� -----
    void FlyAttackAction::SetAnimation()
    {
        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());

        if (animationIndex == Enemy::DragonAnimation::AttackKnockBackEnd1)
        {
            owner_->SetTransitionTime(0.2f);
        }
        else
        {
            owner_->SetTransitionTime(0.1f);
        }

        owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackFly0, false);
    }
}

// ----- KnockBackAction -----
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

            // ���[�g���[�V�������g�p���Ȃ�
            owner_->SetUseRootMotion(false);

            // �_���[�W�ݒ�
            owner_->SetAttackDamage(50.0f);

            // �J�E���^�[�L���͈͂�ݒ肷��
            PlayerManager::Instance().GetPlayer()->SetCounterActiveRadius(8.0f);

            // �ϐ�������
            loopMax_ = 2;
            loopCounter_ = 0;

            // �X�e�[�g�ύX
            SetState(STATE::Guard);

            break;
        case STATE::Guard:// �K�[�h
            
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackKnockBackLoop, false);
                
                // ���݂̗̑͂�ۑ�����
                oldHealth_ = owner_->GetHealth();

                // �X�e�[�g�ύX
                SetState(STATE::Loop);
                break;
            }

            break;
        case STATE::Loop:// ���[�v

            // �U�����������������
            if (owner_->GetHealth() != oldHealth_)
            {
                // �U�����󂯂��̂�Attack�Ɉڍs
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackKnockBackEnd0, false);

                // �X�e�[�g�ύX
                SetState(STATE::Attack);
            }

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
                    // �U���������Ȃ�����
                    owner_->PlayAnimation(Enemy::DragonAnimation::AttackKnockBackEnd1, false);
                    
                    // �X�e�[�g�ύX
                    SetState(STATE::Failed);
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
            // �U�����菈��
            if (owner_->GetAnimationSeconds() > 1.0f)
            {
                if (owner_->GetIsAttackActive())
                    owner_->SetKnockBackAttackActiveFalg(false);
            }
            else if (owner_->GetAnimationSeconds() > 0.36f)
            {
                if (owner_->GetIsAttackActive() == false)
                    owner_->SetKnockBackAttackActiveFalg();
            }

            if(owner_->GetAnimationSeconds() > 1.75f)
            {
                // �X�e�[�g���Z�b�g
                owner_->SetStep(0);

                owner_->SetUseRootMotion(false);

                return ActionBase::State::Complete;
            }

            break;
        case STATE::Failed:// �U�����s

            if (owner_->GetAnimationSeconds() > 1.15f)
            {
                // �X�e�[�g���Z�b�g
                owner_->SetStep(0);

                owner_->SetUseRootMotion(false);

                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }
    void KnockBackAction::DrawDebug()
    {
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
    void SlamAction::DrawDebug()
    {
    }
}

// ----- FireBreath -----
namespace ActionDragon
{
    const ActionBase::State FireBreath::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (owner_->GetStep())
        {
        case 0:// ������
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::FireBreathFront, false);

            // �_���[�W�ݒ�
            owner_->SetAttackDamage(50.0f);

            // ���[�g���[�V�������g�p���Ȃ�
            owner_->SetUseRootMotion(false);
            
            // �ϐ�������
            targetPosition_ = PlayerManager::Instance().GetTransform()->GetPosition();
            isCreateFireball_ = false;

            owner_->SetStep(1);

            break;
        case 1:

            // ��]����
            if (owner_->GetAnimationSeconds() > 0.25f)
            {
                owner_->Turn(elapsedTime, targetPosition_);
            }


            if (owner_->GetAnimationSeconds() > 0.95 && isCreateFireball_ == false)
            {
                Fireball* fireball = new Fireball();
                
                DirectX::XMFLOAT3 mouthPosition = owner_->GetJointPosition("Dragon15_tongue4");
                DirectX::XMFLOAT3 playerPosition = PlayerManager::Instance().GetTransform()->GetPosition();
                DirectX::XMFLOAT3 direction = XMFloat3Normalize(playerPosition - mouthPosition);

                fireball->Launch(elapsedTime, mouthPosition, direction);

                isCreateFireball_ = true;
            }

            if(owner_->GetAnimationSeconds() > 2.4f)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }

            break;
        }

        return ActionBase::State::Run;
    }
    void FireBreath::DrawDebug()
    {
    }
}

// ----- FireBreathCombo -----
namespace ActionDragon
{
    const ActionBase::State FireBreathCombo::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// ������
            // �A�j���[�V�����ݒ�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::FireBreathLeft, false, 1.0f, 0.14f);
            owner_->SetTransitionTime(0.3f);

            // �_���[�W�ݒ�
            owner_->SetAttackDamage(50.0f);

            // ���[�g���[�V�������g�p���Ȃ�
            owner_->SetUseRootMotion(false);

            // �ϐ�������
            targetPosition_ = PlayerManager::Instance().GetTransform()->GetPosition();
            isCreateFireball_ = false;

            // �X�e�[�g�ύX
            SetState(STATE::FirstAttack);

            break;
        case STATE::FirstAttack:
        {
            // ��]����
            if (owner_->GetAnimationSeconds() > 0.25f)
            {
                owner_->Turn(elapsedTime, targetPosition_);
            }

            // �΋�����
            Launch(elapsedTime, 0.95f);

            //if (owner_->IsPlayAnimation() == false)
            if (owner_->GetAnimationSeconds() > 1.7f)
            {
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::FireBreathRight, false, 1.0f, 0.14f);
                owner_->SetTransitionTime(0.25f);

                isCreateFireball_ = false;

                // �X�e�[�g�ύX
                SetState(STATE::SecondAttack);
            }
        }
            break;
        case STATE::SecondAttack:
        {
            // �΋�����
            Launch(elapsedTime, 0.95f);

            //if (owner_->IsPlayAnimation() == false)
            if (owner_->GetAnimationSeconds() > 1.7f)
            {
                owner_->PlayBlendAnimation(Enemy::DragonAnimation::FireBreathFront, false, 1.0f, 0.2f);

                isCreateFireball_ = false;

                // �X�e�[�g�ύX
                SetState(STATE::ThirdAttack);
            }
        }
            break;
        case STATE::ThirdAttack:
        {
            // �΋�����
            Launch(elapsedTime, 0.95f);

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

    void FireBreathCombo::DrawDebug()
    {
    }

    // ----- �΋����� -----
    void FireBreathCombo::Launch(const float& elapsedTime, const float& launchFrame)
    {
        // ���ɔ��˂��Ă���
        if (isCreateFireball_) return;

        // �w�肳�ꂽ���˃t���[���ɒB���Ă��Ȃ�
        if (owner_->GetAnimationSeconds() < launchFrame) return;


        Fireball* fireball = new Fireball();
        DirectX::XMFLOAT3 tongue3Position = owner_->GetJointPosition("Dragon15_tongue3");
        DirectX::XMFLOAT3 tongue4Position = owner_->GetJointPosition("Dragon15_tongue4");

        DirectX::XMFLOAT3 direction = tongue4Position - tongue3Position;
        direction.y = 0;
        direction = XMFloat3Normalize(direction);

        fireball->Launch(elapsedTime, tongue4Position, direction);

        isCreateFireball_ = true;
    }
}

// ----- ComboSlamAction -----
namespace ActionDragon
{
    const ActionBase::State ComboSlamAction::Run(const float& elapsedTime)
    {
        // ���s���m�[�h�𒆒f���邩
        if (owner_->CheckStatusChange()) return ActionBase::State::Failed;

        switch (static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// ������
            // �A�j���[�V�����Đ�
            SetAnimation();

            // �_���[�W�ݒ�
            owner_->SetAttackDamage(50.0f);

            // ���[�g���[�V�������g�p���邪���������_�ł͎g�p���Ȃ�
            owner_->SetUseRootMotion(false);

            // �J�E���^�[�L���͈͂�ݒ肷��
            PlayerManager::Instance().GetPlayer()->SetCounterActiveRadius(6.0f);

            // �ϐ�������
            addForceData_.Initialize(1.0f, 0.4f, 1.0f);

            // �X�e�[�g�ύX
            SetState(STATE::Attack0);

            break;
        case STATE::Attack0:// �U���ꔭ��

            // ���[�g���[�V�������g�p����
            if (owner_->GetUseRootMotionMovement() == false)
            {
                if (owner_->GetIsBlendAnimation() == false) owner_->SetUseRootMotion(true);
            }

            // �ړ�����
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                DirectX::XMFLOAT3 vec = owner_->CalcDirectionToPlayer();
                vec = XMFloat3Normalize({ vec.x, 0.0f, vec.z });
                owner_->AddForce(vec, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            // ��]����
            if (owner_->GetAnimationSeconds() > 0.7f &&
                owner_->GetAnimationSeconds() < 1.3f)
            {
                owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());
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

            // �A�j���[�V�����Đ��I��������X�e�[�g�ύX
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackComboSlam1, false);

                // ���[�g���[�V�������Z�b�g
                owner_->SetUseRootMotion(true);

                // �X�e�[�g�ύX
                SetState(STATE::Attack1);
            }

            break;
        case STATE::Attack1:// �U���񔭖�

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

            // �A�j���[�V�����Đ��I��������X�e�[�g�ύX
            if (owner_->IsPlayAnimation() == false)
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackComboSlamEnd, false);
                
                // �X�e�[�g�ύX
                SetState(STATE::Recovery);
            }

            break;
        case STATE::Recovery:// �㌄
        {
            // �w�肵���t���[���𒴂�����I��
            const float animationEndFrame = 1.1f;
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

    void ComboSlamAction::DrawDebug()
    {
    }

    // ----- �A�j���[�V�����ݒ� -----
    void ComboSlamAction::SetAnimation()
    {
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackComboSlam0, false);
    }
}

// ----- �R���{���������U��(�����킹���Ă���) -----
namespace ActionDragon
{
    const ActionBase::State ComboFlySlamAction::Run(const float& elapsedTime)
    {
        switch(static_cast<STATE>(owner_->GetStep()))
        {
        case STATE::Initialize:// ������
            // �A�j���[�V�����Đ�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackComboSlam0, false);

            // �_���[�W�ݒ�
            owner_->SetAttackDamage(50.0f);

            // �ŏ��̒i�K�ł̓��[�g���[�V�������g�p���Ȃ�
            owner_->SetUseRootMotion(false);

            // �ϐ�������
            addForceData_.Initialize(1.0f, 0.4f, 1.0f);
            comboNum_ = 0;

            // �X�e�[�g�ύX
            SetState(STATE::Attack);

            break;
        case STATE::Attack:

            if (owner_->GetIsBlendAnimation() == false && owner_->GetUseRootMotionMovement() == true)
            {
                owner_->SetUseRootMotion(true);
            }

            // �ړ�����
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                DirectX::XMFLOAT3 vec = owner_->CalcDirectionToPlayer();
                vec = XMFloat3Normalize({ vec.x, 0.0f, vec.z });
                owner_->AddForce(vec, addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            // ��]����
            if (owner_->GetAnimationSeconds() > 0.7f &&
                owner_->GetAnimationSeconds() < 1.3f)
            {
                owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());
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

            // �A�j���[�V�����Đ��I��������X�e�[�g�ύX
            if (comboNum_ < maxComboNum_)
            {
                if(owner_->GetAnimationSeconds() > 1.9f)
                {
                    // �X�e�[�g�ύX
                    SetState(STATE::ComboJudge);
                }
            }
            else
            {
                if (owner_->IsPlayAnimation() == false)
                {
                    // �X�e�[�g�ύX
                    SetState(STATE::ComboJudge);
                }
            }
            

            break;
        case STATE::ComboJudge:

            if (comboNum_ < maxComboNum_)
            {
                ++comboNum_;

                owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackComboSlam0, false, 1.0f, 0.75f);
                owner_->SetTransitionTime(0.25f);

                owner_->SetUseRootMotion(false);

                addForceData_.Initialize(1.0f, 0.4f, 1.0f);

                // �X�e�[�g�ύX
                SetState(STATE::Attack);
            }
            else
            {
                owner_->PlayAnimation(Enemy::DragonAnimation::AttackComboSlamEnd, false);
                owner_->SetTransitionTime(0.1f);

                // �X�e�[�g�ύX
                SetState(STATE::Recovery);
            }

            break;
        case STATE::Recovery:
        {
            // �w�肵���t���[���𒴂�����I��
            const float animationEndFrame = 1.1f;
            if (owner_->GetAnimationSeconds() > animationEndFrame)
            {
                owner_->SetStep(0);

                owner_->SetUseRootMotion(false);

                return ActionBase::State::Complete;
            }
        }
            break;
        }

        return ActionBase::State::Run;
    }
    void ComboFlySlamAction::DrawDebug()
    {
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
    void ComboChargeAction::DrawDebug()
    {
    }
}

// ----- TurnAttackAction -----
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
            SetAnimation();

            // �_���[�W�ݒ�
            owner_->SetAttackDamage(50.0f);

            // ���[�g���[�V�������g�p���邪���������_�ł͎g�p���Ȃ�
            owner_->SetUseRootMotion(false);
             
            // �J�E���^�[�L���͈͂�ݒ肷��
            PlayerManager::Instance().GetPlayer()->SetCounterActiveRadius(6.0f);

            // �ϐ�������
            addForceData_.Initialize(1.5f, 0.3f, 0.5f);
 
            // �X�e�[�g�ύX
            SetState(STATE::Attack);

            break;
        case STATE::Attack:// �U��

            // ���[�g���[�V�������g�p����
            if (owner_->GetUseRootMotionMovement() == false)
            {
                if (owner_->GetIsBlendAnimation() == false) owner_->SetUseRootMotion(true);
            }

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

            if(owner_->GetAnimationSeconds() >= 3.25f)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
            break;
        }

        return ActionBase::State::Run;
    }

    void TurnAttackAction::DrawDebug()
    {
    }

    // ----- �A�j���[�V������ݒ肷�� -----
    void TurnAttackAction::SetAnimation()
    {
        const Enemy::DragonAnimation animationIndex = static_cast<Enemy::DragonAnimation>(owner_->GetAnimationIndex());

        if(animationIndex == Enemy::DragonAnimation::AttackTurn)
        {
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackTurn, false, 1.0f, 0.3f);
            return;
        }
        
        if (animationIndex == Enemy::DragonAnimation::AttackKnockBackEnd1)
        {
            owner_->SetTransitionTime(0.2f);
        }
        else
        {
            owner_->SetTransitionTime(0.1f);
        }
        
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::AttackTurn, false);
    }
}

// ----- TackleAction -----
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

            // �_���[�W�ݒ�
            owner_->SetAttackDamage(50.0f);

            // ���[�g���[�V�������g�p���邪���������_�ł͎g�p���Ȃ�
            owner_->SetUseRootMotion(false);

            // �J�E���^�[�L���͈͂�ݒ肷��
            PlayerManager::Instance().GetPlayer()->SetCounterActiveRadius(6.0f);

            // �ϐ�������
            addForceData_.Initialize(0.15f, 0.6f, 0.6f);
            //addForceData_.Initialize(0.15f, 0.8f, 0.6f);
            easingTimer_ = 0.0f;

            // �X�e�[�g�ύX
            SetState(STATE::PreAction);

            break;
        case STATE::PreAction:// �\������
        {
            // ���[�g���[�V�������g�p����
            if (owner_->GetUseRootMotionMovement() == false)
            {
                if (owner_->GetIsBlendAnimation() == false) owner_->SetUseRootMotion(true);
            }

            // ��]���� ( �v���C���[�̕����Ɍ��� )
            const float turnStateFrame = 0.3f;
            if (owner_->GetAnimationSeconds() > turnStateFrame)
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
                // �n�ʂɖ��܂�Ȃ��悤�ɂO�n�_�Ƀ��Z�b�g����
                owner_->GetTransform()->SetPositionY(0);

                addForceData_.Initialize(0.03f, 0.5f, 0.6f);
                //addForceData_.Initialize(0.03f, 0.6f, 0.8f);

                owner_->PlayAnimation(Enemy::DragonAnimation::AttackTackle3, false);

                // ���[�g���[�V�������Z�b�g
                owner_->SetUseRootMotion(true);

                SetState(STATE::Recovery);
            }
        }
            break;
        case STATE::Recovery:// �㌄ ( �r���܂ōU�����肠�� )
        {
            // �r�����ʔj�󂳂�Ă���ꍇ�͓r���ŏI�����A���݂Ɉڍs����
            if (owner_->GetIsPartDestruction(Enemy::PartName::Leg))
            {
                if (owner_->GetAnimationSeconds() >= 0.35f)
                {
                    // �U�����薳����
                    owner_->SetTackleAttackActiveFlag(false);

                    // �ʒuY���O�ɂ��Ă�����
                    owner_->GetTransform()->SetPositionY(0.0f);

                    owner_->SetStep(0);
                    return ActionBase::State::Complete;
                }
            }

            // �U�����薳����
            if (owner_->GetIsAttackActive())
            {
                if (owner_->GetAnimationSeconds() > 0.6f)
                {
                    owner_->SetTackleAttackActiveFlag(false);
                }
            }

            // �ړ�����
            if (addForceData_.Update(owner_->GetAnimationSeconds()))
            {
                owner_->AddForce(owner_->GetTransform()->CalcForward(), addForceData_.GetForce(), addForceData_.GetDecelerationForce());
            }

            // �A�j���[�V�����I��
            if(owner_->GetAnimationSeconds() > 1.9f)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
        }
            break;
        }

        return ActionBase::State::Run;
    }
    void TackleAction::DrawDebug()
    {
    }
}

namespace ActionDragon
{
    const ActionBase::State ComboTackleAction::Run(const float& elapsedTime)
    {
        return ActionBase::State();
    }
    void ComboTackleAction::DrawDebug()
    {
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

            // �_���[�W�ݒ�
            owner_->SetAttackDamage(50.0f);

            // ���[�g���[�V�������g�p���Ȃ�
            owner_->SetUseRootMotion(false);

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
    void RiseAttackAction::DrawDebug()
    {
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
    void MoveTurnAction::DrawDebug()
    {
    }
}

namespace ActionDragon
{
    const ActionBase::State MoveAction::Run(const float& elapsedTime)
    {        
        switch (owner_->GetStep())
        {
        case 0:// ������
            // �A�j���[�V�����ݒ�
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::Walk, true);
            //owner_->PlayBlendAnimation(Enemy::DragonAnimation::Run, true);
            owner_->SetTransitionTime(0.1f);

            // �ړ��ړI�n��ݒ�
            targetPosition_ = PlayerManager::Instance().GetTransform()->GetPosition();

            owner_->SetStep(1);

            break;
        case 1:
        {
            // ��]����
            owner_->Turn(elapsedTime, targetPosition_);

            // �ړ�����
            const DirectX::XMFLOAT3 vec = XMFloat3Normalize(owner_->CalcDirectionToPlayer());
            const float speed = owner_->GetWalkSpeed() * elapsedTime;

            owner_->GetTransform()->AddPosition(vec * speed);

            const float distance = owner_->CalcDistanceToPlayer();
            if (distance < 10.0f)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }


        }
            break;
        }

        return ActionBase::State::Run;
    }
    void MoveAction::DrawDebug()
    {
    }
}