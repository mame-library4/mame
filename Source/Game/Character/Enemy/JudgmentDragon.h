#pragma once
#include "BehaviorTree/JudgmentBase.h"
#include "Enemy.h"

// ���S����
class DeathJudgment : public JudgmentBase
{
public:
    DeathJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// �Ђ�ݔ���
class FlinchJudgment : public JudgmentBase
{
public:
    FlinchJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// ��퓬����
class NonBattleJudgment : public JudgmentBase
{
public:
    NonBattleJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;

private:
    int num_ = 0;
};

// ��퓬���ҋ@����
class NonBattleIdleJudgment : public JudgmentBase
{
public:
    NonBattleIdleJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// ���K����
class RoarJudgment : public JudgmentBase
{
public:
    RoarJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// ���є���
class ShoutJudgment : public JudgmentBase
{
public:
    ShoutJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// �ߋ����U������
class NearJudgment : public JudgmentBase
{
public:
    NearJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;

private:
    int num_ = 0;
};