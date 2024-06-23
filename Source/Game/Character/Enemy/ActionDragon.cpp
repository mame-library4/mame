#include "ActionDragon.h"
#include "Input.h"

// ----- 死亡行動 -----
const ActionBase::State DeathAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:// 初期化
        // アニメーション設定
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::Death, false);

        owner_->SetStep(1);
        break;
    case 1:
        // アニメーションが最後まで再生されたら
        if (owner_->IsPlayAnimation() == false)
        {
            // 死亡ループモーションを流す
            owner_->PlayBlendAnimation(Enemy::DragonAnimation::DeathLoop, false);

            owner_->SetStep(2);
            break;
        }

        break;
    case 2:
        // 死亡しているのでここで放置
        break;
    }

    return ActionBase::State::Run;
}

// ----- 怯み行動 -----
const ActionBase::State FlinchAction::Run(const float& elapsedTime)
{
    return ActionBase::State();
}

// ----- 非戦闘時待機 -----
const ActionBase::State NonBattleIdleAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:// 初期化
        // アニメーション設定
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::Idle0, true);

        owner_->SetStep(1);
        break;
    case 1:
        break;
    }

    return ActionBase::State();
}

// ----- 非戦闘時歩き -----
const ActionBase::State NonBattleWalkAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:// 初期化
        // アニメーション設定
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::Walk, true);

        owner_->SetStep(1);
        break;
    case 1:
        break;
    }

    return ActionBase::State();
}

const ActionBase::State RoarAction::Run(const float& elapsedTime)
{
    switch (owner_->GetStep())
    {
    case 0:// 初期化
        // アニメーション設定
        owner_->PlayBlendAnimation(Enemy::DragonAnimation::Roar, true);

        owner_->SetStep(1);
        break;
    case 1:

        if (owner_->IsPlayAnimation() == false)
        {
            owner_->SetStep(0);
            return ActionBase::State::Complete;
        }

        break;
    }

    return ActionBase::State::Run;
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
