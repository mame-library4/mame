#pragma once
#include "BehaviorTree/JudgmentBase.h"
#include "Enemy.h"

// €–S”»’è
class DeathJudgment : public JudgmentBase
{
public:
    DeathJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// ‚Ğ‚é‚İ”»’è
class FlinchJudgment : public JudgmentBase
{
public:
    FlinchJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// ”ñí“¬”»’è
class NonBattleJudgment : public JudgmentBase
{
public:
    NonBattleJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;

private:
    int num_ = 0;
};

// ”ñí“¬‘Ò‹@”»’è
class NonBattleIdleJudgment : public JudgmentBase
{
public:
    NonBattleIdleJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// ™ôšK”»’è
class RoarJudgment : public JudgmentBase
{
public:
    RoarJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// ‹©‚Ñ”»’è
class ShoutJudgment : public JudgmentBase
{
public:
    ShoutJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// ‹ß‹——£UŒ‚”»’è
class NearJudgment : public JudgmentBase
{
public:
    NearJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;

private:
    int num_ = 0;
};