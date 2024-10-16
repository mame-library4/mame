#pragma once
#include "BehaviorTree/JudgmentBase.h"
#include "Enemy.h"

// 死亡判定
class DeathJudgment : public JudgmentBase
{
public:
    DeathJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// ひるみ判定
class FlinchJudgment : public JudgmentBase
{
public:
    FlinchJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// 通常の怯み判定
class NormalFlinchJudgment : public JudgmentBase
{
public:
    NormalFlinchJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

class PartDestructionFlinchJudgment : public JudgmentBase
{
public:
    PartDestructionFlinchJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// 非戦闘判定
class NonBattleJudgment : public JudgmentBase
{
public:
    NonBattleJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;

private:
    int num_ = 0;
};

// 非戦闘時待機判定
class NonBattleIdleJudgment : public JudgmentBase
{
public:
    NonBattleIdleJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// 咆哮判定
class RoarJudgment : public JudgmentBase
{
public:
    RoarJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// 叫び判定
class ShoutJudgment : public JudgmentBase
{
public:
    ShoutJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// 近距離攻撃判定
class NearJudgment : public JudgmentBase
{
public:
    NearJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;

private:
    int num_ = 0;
};

// ----- ３連続たたきつけ攻撃 -----
class ComboFlySlamJudgment : public JudgmentBase
{
public:
    ComboFlySlamJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};