#include "ActionDragon.h"
#include "Input.h"

// ----- ���S�s�� -----
const ActionBase::State DeathAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:
        // �A�j���[�V�����ݒ�
        //owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, Enemy::DragonAnimation::Death, false);
        

        owner_->SetStep(1);
        break;
    case 1:
        // �A�j���[�V�������Ō�܂ōĐ����ꂽ��
        if (owner_->IsPlayAnimation() == false)
        {
            // ���S���[�v���[�V�����𗬂�
          //  owner_->PlayBlendAnimation(Enemy::DragonAnimation::DeathLoop, false);
            

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
    return ActionBase::State::Complete;

    return ActionBase::State();
}

const ActionBase::State NonBattleIdleAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State NonBattleWalkAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State RoarAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State BackStepRoarAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

const ActionBase::State MoveRoarAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
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
