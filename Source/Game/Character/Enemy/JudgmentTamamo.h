#pragma once
#include "../../../AI/BehaviorTree/JudgmentBase.h"
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

#pragma region ”ñí“¬
// ”ñí“¬”»’è
class NonBattleJudgment : public JudgmentBase
{
public:
    NonBattleJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// ”ñí“¬‘Ò‹@”»’è
class NonBattleIdleJudgment : public JudgmentBase
{
public:
    NonBattleIdleJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// ”ñí“¬•à‚«”»’è
class NonBattleWalkJudgment : public JudgmentBase
{
public:
    NonBattleWalkJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

#pragma endregion ”ñí“¬

#pragma region í“¬

#pragma region ‘Ò‹@Œn
// •à‚«”»’è
class WalkJudgment : public JudgmentBase
{
public:
    WalkJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

#pragma endregion ‘Ò‹@Œn

#pragma region UŒ‚Œn
// UŒ‚”»’è
class AttackJudgment : public JudgmentBase
{
public:
    AttackJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};


#pragma region ‹ß‹——£
// ‹ß‹——£UŒ‚”»’è
class NearAttackJudgment : public JudgmentBase
{
public:
    NearAttackJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// K”öUŒ‚”»’è
class TailSwipeJudgment : public JudgmentBase
{
public:
    TailSwipeJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

#pragma endregion ‹ß‹——£


#pragma region ‰“‹——£
// ‰“‹——£UŒ‚”»’è
class FarAttackJudgment : public JudgmentBase
{
public:
    FarAttackJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

#pragma endregion ‰“‹——£

#pragma region ‹©‚ÔŒn
// ‹©‚ÔŒn”»’è
class ShoutJudgment : public JudgmentBase
{
public:
    ShoutJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;

private:
    int shoutThreshold_ = 5; // ‹©‚Ñ‚ª‰Â”\‚É‚È‚é‰ñ”
};

// ™ôšK”»’è
class RoarJudgment : public JudgmentBase
{
public:
    RoarJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// ˆĞŠd”»’è
class IntimidateJudgment : public JudgmentBase
{
public:
    IntimidateJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

#pragma endregion ‹©‚ÔŒn

#pragma endregion UŒ‚Œn

#pragma endregion í“¬