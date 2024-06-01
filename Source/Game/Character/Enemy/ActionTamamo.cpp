#include "ActionTamamo.h"
#include "EnemyTamamo.h"
#include "../Other/MathHelper.h"
#include "../Other/Easing.h"
#include "../Player/PlayerManager.h"
#include "Camera.h"

// ----- ���S�s�� -----
const ActionBase::State DeathAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        // �A�j���[�V�����Đ�

        timer_ = 0.0f;

        owner_->SetStep(1);
        break;
    case 1:
        timer_ += elapsedTime;

        if (timer_ > 1.0f)
        {
            owner_->isWin_ = true;
            owner_->SetStep(2);
        }

        break;
    case 2:
        // ���S�Ȃ̂ł��̂܂ܕ��u

        float posY = owner_->GetTransform()->GetPositionY();

        posY -= elapsedTime * 5.0f;
        float max = -100.0f;
        posY = std::max(max, posY);

        owner_->GetTransform()->SetPositionY(posY);

        break;
    }

    return ActionBase::State::Run;
}

// ----- �Ђ�ݍs�� -----
const ActionBase::State FlinchAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:// ������
        // �Ђ�݃A�j���[�V�����Đ�
        owner_->PlayAnimation(Enemy::TamamoAnimation::Filnch, false, 1.0f);

        owner_->SetStep(1);
        break;
    case 1:
        // �A�j���[�V�����Đ��I��
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->SetStep(0);
            return ActionBase::State::Failed;
        }

        break;
    }

    return ActionBase::State::Run;
}

#pragma region ��퓬
// ----- ��퓬�ҋ@�s�� -----
const ActionBase::State NonBattleIdleAction::Run(const float& elapsedTime)
{
    // �A�j���[�V�����Đ� ( Idle )
    //owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Walk, Enemy::TamamoAnimation::Idle, true);

    return ActionBase::State::Run;
}

// ----- ��퓬�����s�� -----
const ActionBase::State NonBattleWalkAction::Run(const float& elapsedTime)
{

    DirectX::XMFLOAT3 ownerPos = owner_->GetTransform()->GetPosition();

    switch (owner_->GetStep())
    {
    case 0:
    {
        // �A�j���[�V�����Đ�
        if (owner_->GetCurrentBlendAnimationIndex() < 0)
        {
            owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Idle, Enemy::TamamoAnimation::Walk, true);
            owner_->SetWeight(1.0f);
        }
        else
        {
            owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Walk, true);
            owner_->SetWeight(0.0f);
        }

        DirectX::XMFLOAT3 movePos = owner_->SetTargetPosition();

        owner_->SetMovePosition(movePos);

        owner_->SetStep(1);
    }
        break;
    case 1:
    {
        // �퓬��ԂɂȂ�
        if (owner_->SearchPlayer())
        {
            owner_->SetStep(0);
            //return ActionBase::State::Complete;
            return ActionBase::State::Failed;
        }

        owner_->CollisionCharacterVsStage();

        owner_->AddWeight(elapsedTime * 2.0f);

        DirectX::XMFLOAT3 movePos = owner_->GetMovePosition();

        // �i�s���������߂�
        DirectX::XMFLOAT3 direction = movePos - ownerPos;

        // �ړI�n�ɒ�����������
        float length = XMFloat3Length(direction);
        if (length <= 1.0f)
        {
            // �ړI�n�ɒ������̂ŏI��
            owner_->SetStep(0);
            return ActionBase::State::Failed;
        }

        // �i�ޑ��x���v�Z
        direction = XMFloat3Normalize(direction);
        float speed = owner_->GetWalkSpeed() * elapsedTime;
        direction = direction * speed;

        owner_->GetTransform()->AddPosition(direction);

        // ���񏈗�
        owner_->Turn(elapsedTime, owner_->GetMovePosition());

    }
        break;
    }

    return ActionBase::State::Run;
}

#pragma endregion ��퓬

#pragma region �퓬

#pragma region �ҋ@�n 
// ----- �����s�� -----
const ActionBase::State WalkAction::Run(const float& elapsedTime)
{
    DirectX::XMFLOAT2 ownerFront = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
    DirectX::XMFLOAT2 ownerPos = { owner_->GetTransform()->GetPositionX(), owner_->GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 playerPos = { PlayerManager::Instance().GetTransform()->GetPositionX(), PlayerManager::Instance().GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 vec = playerPos - ownerPos;

    switch (owner_->GetStep())
    {
    case 0:
    {
        // �O�ςŃv���C���[�����E�ǂ����ɂ��邩����
        float cross = XMFloat2Cross(ownerFront, vec);
        // ���݂̕�����ۑ�����
        playerSide_ = (cross > 0) ? Side::Right : Side::Left;

        // ���E�ōĐ�����A�j���[�V������؂�ւ���
        if (playerSide_ == Side::Left)
        {
            // ����ݒ肷��A�j���[�V�������O��Ɠ����łȂ���Ώ���������
            if (owner_->PlayBlendAnimation(Enemy::TamamoAnimation::WalkRight, true))
            {
                // �E�F�C�g�l������
                owner_->SetWeight(0.0f);
            }
        }
        else
        {
            // ����ݒ肷��A�j���[�V�������O��Ɠ����łȂ���Ώ���������
            if (owner_->PlayBlendAnimation(Enemy::TamamoAnimation::WalkLeft, true))
            {
                // �E�F�C�g�l������
                owner_->SetWeight(0.0f);
            }
        }

        // �^�C�}�[������
        actionTimer_ = 2.0f;

        owner_->SetStep(1);
    }
        break;
    case 1:
    {
        owner_->CollisionCharacterVsStage();

        vec = XMFloat2Normalize(vec);
        // �v���C���[���E�ɂ���
        if (playerSide_ == Side::Right)
        {
            // ���x�N�g���Z�o
            vec = { -vec.y, vec.x };
        }
        // �v���C���[�����ɂ���
        if (playerSide_ == Side::Left)
        {
            // �E�x�N�g���Z�o
            vec = { vec.y, -vec.x };
        }

        owner_->GetTransform()->AddPositionX(vec.x * elapsedTime * 2.0f);
        owner_->GetTransform()->AddPositionZ(vec.y * elapsedTime * 2.0f);
    }


        // ���񏈗�
        owner_->Turn(elapsedTime, PlayerManager::Instance().GetTransform()->GetPosition());

        owner_->AddWeight(elapsedTime * 2.0f);
        actionTimer_ -= elapsedTime;
        if (actionTimer_ <= 0.0f)
        {
            // �v�C���C���[���͈͊O�ɏo����
            if (owner_->SearchPlayer() == false)
            {
                owner_->SetStep(0);
                return ActionBase::State::Complete;
            }
            else
            {
                owner_->SetStep(0);
                return ActionBase::State::Failed;
            }
        }

        break;
    }

    return ActionBase::State::Run;
}

// ----- �X�e�b�v�s�� -----
const ActionBase::State StepAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        break;
    case 1:
        break;
    }

    return ActionBase::State::Run;
}

#pragma endregion �ҋ@�n

#pragma region �U���n
// ----- ���݂��s�� -----
const ActionBase::State BiteAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        // �A�j���[�V�����ݒ� ( ���݂� )
        owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Bite, false);

        // ���݂��U������L����
        owner_->SetBiteAttackFlag();

        // �ϐ�������
        isAttackCollisionEnd_ = false;
        isAddForce_ = false;    

        owner_->SetWeight(0.0f);

        owner_->SetStep(1);
        break;
    case 1:
        owner_->AddWeight(2.0f * elapsedTime);

        // �A�j���[�V�����ɍ��킹�čU������𖳌�������
        if (owner_->GetBlendAnimationSeconds() > attackCollisionEndFrame_ &&
            isAttackCollisionEnd_ == false)
        {
            // ��x������������悤�ɐ��䂷��
            isAttackCollisionEnd_ = true;

            // ���݂��U�����薳����
            owner_->SetBiteAttackFlag(false);
        }

        // �A�j���[�V�����ɍ��킹�ď����O�i����
        if (owner_->GetBlendAnimationSeconds() > addForceFrame_ &&
            isAddForce_ == false)
        {
            isAddForce_ = true;
            owner_->AddForce(owner_->GetTransform()->CalcForward(), 0.3f);
        }

        // �A�j���[�V�������I�������I��
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->SetStep(0);
            return ActionBase::State::Failed;
        }

        break;
    }

    return ActionBase::State::Run;
}

// ----- �Ђ������s�� -----
const ActionBase::State SlashAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        // �A�j���[�V�����ݒ� ( �Ђ����� )
        owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Slash, false);

        // �Ђ������U������ݒ�
        owner_->SetSlashAttackFlag();

        // �ϐ�������
        isAttackCollisionEnd_ = false;

        owner_->SetStep(1);

        break;
    case 1:

        // �A�j���[�V�����ɍ��킹�čU������𖳌�������
        if (owner_->GetBlendAnimationSeconds() > attackCollisionEndFrame_ &&
            isAttackCollisionEnd_ == false)
        {            
            // ��x���������������Ȃ��悤�ɐ��䂷��
            isAttackCollisionEnd_ = true;

            // �Ђ������U������𖳌�������
            owner_->SetSlashAttackFlag(false);
        }


        // �A�j���[�V�������I�������I��
        if (owner_->IsPlayAnimation() == false)
        {
            owner_->SetStep(0);
            return ActionBase::State::Failed;
        }

        break;
    }

    return ActionBase::State::Run;
}

// ----- �K���s�� -----
const ActionBase::State TailSwipeAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        // �A�j���[�V�����ݒ� ( �K�� )
        owner_->PlayBlendAnimation(Enemy::TamamoAnimation::TailSwipe, false);

        // �K���U������ݒ�
        owner_->SetTailSwipeAttackFlag();

        owner_->SetStep(1);
        break;
    case 1:


        // �A�j���[�V�������I�������I��
        if (owner_->IsPlayAnimation() == false)
        {
            // �U�����胊�Z�b�g
            owner_->SetTailSwipeAttackFlag(false);

            owner_->SetStep(0);
            return ActionBase::State::Failed;
        }

        break;
    }

    return ActionBase::State::Run;
}

// ----- ���������s�� -----
const ActionBase::State SlamAction::Run(const float& elapsedTime)
{
    DirectX::XMFLOAT3 playerPosition = PlayerManager::Instance().GetTransform()->GetPosition();
    DirectX::XMFLOAT3 ownerPosition = owner_->GetTransform()->GetPosition();
    DirectX::XMFLOAT2 ownerPos = { ownerPosition.x, ownerPosition.z };
    DirectX::XMFLOAT2 targetPos = { targetPosition_.x, targetPosition_.z };

    // �A�j���[�V�����ɍ��킹�čU�������L�����A�������̐؂�ւ�
    UpdateAttackCollision();

    owner_->AddWeight(2.0f * elapsedTime);

    switch (static_cast<Step>(owner_->GetStep()))
    {
    case Step::Initialize:// �����ݒ�
        // �A�j���[�V�����Đ� ( �������� )
        owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Slam, false);
        owner_->SetWeight(0.0f);
        
        // �ϐ�������
        isAttackCollisionStart_ = false;
        isAttackCollisionEnd_ = false;

        SetStep(Step::Search);
        break;
    case Step::Search:// �_�����߂�

        // ��]����
        owner_->Turn(elapsedTime, playerPosition);

        // �w�肵�Ă���A�j���[�V�����̃t���[���𒴂���Ǝ��̏�ԂɈڂ�
        if (owner_->GetBlendAnimationSeconds() > stateChangeFrame_)
        {
            // �Ƃт�����ʒu��ݒ肷��
            targetPosition_ = playerPosition;

            targetPos = { targetPosition_.x, targetPosition_.z };

            // �������g�ƃ^�[�Q�b�g�̋������Z�o
            length_ = XMFloat2Length(targetPos - ownerPos);

            SetStep(Step::Ascend);
        }        

        break;
    case Step::Ascend:// �㏸����
    {
        // Y �����ړ�
        easingTimer_ += elapsedTime;
        ownerPosition.y = Easing::InCirc(easingTimer_, ascendEndFrame_ - stateChangeFrame_, maxAscend_, 0.0f);
        owner_->GetTransform()->SetPositionY(ownerPosition.y);
                
        // XZ �����ړ�
        DirectX::XMFLOAT2 moveVec = XMFloat2Normalize(targetPos - ownerPos);
        float moveLength = (length_ / 3) / (ascendEndFrame_ - stateChangeFrame_) * elapsedTime;
        moveVec = moveVec * moveLength;
        owner_->GetTransform()->AddPositionX(moveVec.x);
        owner_->GetTransform()->AddPositionZ(moveVec.y);

        // ��]����
        owner_->Turn(elapsedTime, targetPosition_);

        // �w�肵�Ă���A�j���[�V�����̃t���[���𒴂���Ǝ��̏�ԂɈڂ�
        if (owner_->GetBlendAnimationSeconds() > ascendEndFrame_)
        {
            // �^�C�}�[���Z�b�g
            easingTimer_ = 0.0f;

            SetStep(Step::Attack);
        }
    }
        break;
    case Step::Attack:// ���ۂɂ��������U���ɓ���
    {
        easingTimer_ += elapsedTime;
        ownerPosition.y = Easing::InCubic(easingTimer_, attackEndFrame_ - ascendEndFrame_, 0.0f, maxAscend_);
        owner_->GetTransform()->SetPositionY(ownerPosition.y);

        // XZ �����ړ�
        DirectX::XMFLOAT2 moveVec = XMFloat2Normalize(targetPos - ownerPos);
        float moveLength = (length_ / 3)  / (attackEndFrame_ - ascendEndFrame_) * elapsedTime;
        moveVec = moveVec * moveLength;
        owner_->GetTransform()->AddPositionX(moveVec.x);
        owner_->GetTransform()->AddPositionZ(moveVec.y);

        // ��]����
        owner_->Turn(elapsedTime, targetPosition_);

        if (owner_->GetBlendAnimationSeconds() > attackEndFrame_)
        {
            // �J�����V�F�C�N������
            Camera::Instance().ScreenVibrate(0.1f, 1.0f);

            owner_->InitializeStones();

            // �ʒu���O�ɐݒ�
            owner_->GetTransform()->SetPositionY(0.0f);
            
            // �^�C�}�[���Z�b�g
            easingTimer_ = 0.0f;

            SetStep(Step::Wait);
        }

    }
        break;
    case Step::Wait:

        // �A�j���[�V�������I�������I��
        if (owner_->IsPlayAnimation() == false)
        {
            SetStep(Step::Initialize);

            return ActionBase::State::Failed;
        }

        break;
    }

    return ActionBase::State();
}

// ----- �A�j���[�V�����ɍ��킹�čU�������L�����A�������̐؂�ւ� -----
void SlamAction::UpdateAttackCollision()
{
    // �A�j���[�V�����ɍ��킹�čU�������L��������
    if (owner_->GetBlendAnimationSeconds() > attackCollisionStartFrame_ &&
        isAttackCollisionStart_ == false)
    {
        // ����t���O�𗧂Ă�
        isAttackCollisionStart_ = true;

        // ���������U�������L��������
        owner_->SetSlamAttackFlag();
    }
    // �A�j���[�V�����ɍ��킹�čU������𖳌�������
    else if (owner_->GetBlendAnimationSeconds() > attackCollisionEndFrame_ &&
        isAttackCollisionEnd_ == false)
    {
        // ����t���O�𗧂Ă�
        isAttackCollisionEnd_ = true;

        // ���������U������𖳌�������
        owner_->SetSlamAttackFlag(false);
    }
}

#pragma endregion �U���n

#pragma region ���Ԍn
// ----- ���K�s�� -----
const ActionBase::State RoarAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        //owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Roar, false);
        owner_->SetStep(1);
        break;
    case 1:

        if (!owner_->IsPlayAnimation())
        {
            owner_->SetStep(0);
            return ActionBase::State::Failed;
        }

        break;
    }

    return ActionBase::State::Run;
}

// ----- �Њd�s�� -----
const ActionBase::State IntimidateAction::Run(const float& elapsedTime)
{
    return ActionBase::State::Run;
}

#pragma endregion ���Ԍn

#pragma endregion �퓬


