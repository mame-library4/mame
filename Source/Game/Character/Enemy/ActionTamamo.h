#pragma once
#include "../../../AI/BehaviorTree/ActionBase.h"
#include "Enemy.h"

// ���S�s��
class DeathAction : public ActionBase
{
public:
    DeathAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �Ђ�ݍs��
class FlinchAction : public ActionBase
{
public:
    FlinchAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma region ��퓬
// ��퓬�ҋ@�s��
class NonBattleIdleAction : public ActionBase
{
public:
    NonBattleIdleAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// ��퓬�����s��
class NonBattleWalkAction : public ActionBase
{
public:
    NonBattleWalkAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma endregion ��퓬

#pragma region �퓬

#pragma region �ҋ@�n
// �����s��
class WalkAction : public ActionBase
{
public:
    WalkAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    float actionTimer_ = 0.0f;
};

class StepAction : public ActionBase
{
public:
    StepAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma endregion �ҋ@�n

#pragma region �U���n
// ���݂��s��
class BiteAction : public ActionBase
{
public:
    BiteAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �Ђ������s��
class SlashAction : public ActionBase
{
public:
    SlashAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �K���s��
class TailSwipeAction : public ActionBase
{
public:
    TailSwipeAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma endregion �U���n

#pragma region ���Ԍn
// ���K�s��
class RoarAction : public ActionBase
{
public:
    RoarAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �Њd�s��
class IntimidateAction : public ActionBase
{
public:
    IntimidateAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma endregion ���Ԍn

#pragma endregion �퓬