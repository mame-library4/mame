#include "ActionTamamo.h"
#include "EnemyTamamo.h"
#include "../Other/MathHelper.h"
#include "../Player/PlayerManager.h"

// ----- ���S�s�� -----
const ActionBase::State DeathAction::Run(const float& elapsedTime)
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

// ----- �Ђ�ݍs�� -----
const ActionBase::State FlinchAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:// ������
        // �Ђ�݃A�j���[�V�����Đ�
        //owner_->PlayAnimation(Enemy::TamamoAnimation::Filnch, false, 1.0f);


        owner_->SetStep(1);
        break;
    case 1:
        break;
    }

    return ActionBase::State::Run;
}

#pragma region ��퓬
// ----- ��퓬�ҋ@�s�� -----
const ActionBase::State NonBattleIdleAction::Run(const float& elapsedTime)
{
    // �A�j���[�V�����Đ� ( Idle )
    owner_->PlayAnimation(Enemy::TamamoAnimation::Idle, true);

    return ActionBase::State::Run;
}

// ----- ��퓬�����s�� -----
const ActionBase::State NonBattleWalkAction::Run(const float& elapsedTime)
{
    // �A�j���[�V�����Đ�
    owner_->PlayAnimation(Enemy::TamamoAnimation::Walk, true);

    DirectX::XMFLOAT3 ownerPos = owner_->GetTransform()->GetPosition();

    switch (owner_->GetStep())
    {
    case 0:
    {
        DirectX::XMFLOAT3 movePos = { ownerPos.x + (rand() % 10 - 5), ownerPos.y, ownerPos.z + (rand() % 10 - 5) };
        movePos = owner_->SetTargetPosition(movePos);

        owner_->SetMovePosition(movePos);

        owner_->SetStep(1);
    }
        break;
    case 1:
    {
        owner_->CollisionCharacterVsStage();

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
        direction = direction * elapsedTime * 1.0f;

        owner_->GetTransform()->AddPosition(direction);

        // ���񏈗�
        owner_->Turn(elapsedTime, owner_->GetMovePosition());

    }
        break;
    }

    return ActionBase::State::Run;
}

#pragma endregion ��퓬

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
        // �v���C���[���E�ɂ���
        if (cross > 0)
        {
            // �A�j���[�V�����Đ� ( ������ )
            //owner_->PlayAnimation(Enemy::TamamoAnimation::WalkLeft, true);
            owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Walk, Enemy::TamamoAnimation::WalkLeft, true);
        }
        // �v���C���[�����ɂ���
        else
        {
            // �A�j���[�V�����Đ� ( �����E )
            //owner_->PlayAnimation(Enemy::TamamoAnimation::WalkRight, true);
            owner_->PlayBlendAnimation(Enemy::TamamoAnimation::Walk, Enemy::TamamoAnimation::WalkRight, true);
        }

        // ������
        actionTimer_ = 3.0f;
        owner_->SetWeight(0.0f);

        owner_->SetStep(1);
    }
        break;
    case 1:
    {
        owner_->CollisionCharacterVsStage();

        // �O�ςŃv���C���[�����E�ǂ����ɂ��邩����
        float cross = XMFloat2Cross(ownerFront, vec);
        vec = XMFloat2Normalize(vec);
        // �v���C���[���E�ɂ���
        if (cross > 0)
        {
            // ���x�N�g���Z�o
            vec = { -vec.y, vec.x };
        }
        // �v���C���[�����ɂ���
        else
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
            owner_->SetStep(0);
            return ActionBase::State::Failed;
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
        owner_->PlayAnimation(Enemy::TamamoAnimation::Bite, false);

        owner_->SetStep(1);

        break;
    case 1:
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
        owner_->PlayAnimation(Enemy::TamamoAnimation::Slash, false);

        owner_->SetStep(1);

        break;
    case 1:
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
        owner_->PlayAnimation(Enemy::TamamoAnimation::TailSwipe, false);

        owner_->SetStep(1);

        break;
    case 1:
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

#pragma endregion �U���n

#pragma region ���Ԍn
// ----- ���K�s�� -----
const ActionBase::State RoarAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        //owner_->PlayAnimation(Enemy::TamamoAnimation::Roar, false);
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


