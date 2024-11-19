#pragma once
#include "BehaviorTree/JudgmentBase.h"
#include "Enemy.h"

namespace ActionDragon
{
    // ----- DeathJudgment -----
    class DeathJudgment : public JudgmentBase
    {
    public:
        DeathJudgment(Enemy* owner) : JudgmentBase(owner) {}
        const bool Judgment() override;
        void DrawDebug() override {}
    };

    // ----- DownJudgment -----
    class DownJudgment : public JudgmentBase
    {
    public:
        DownJudgment(Enemy* owner) : JudgmentBase(owner) {}
        const bool Judgment() override;
        void DrawDebug() override {}
    };

    // ----- KnockDownJudgment -----
    class KnockDownJudgment : public JudgmentBase
    {
    public:
        KnockDownJudgment(Enemy* owner) : JudgmentBase(owner) {}
        const bool Judgment() override;
        void DrawDebug() override {}
    };

    // ----- LongRangeJudgment -----
    class LongRangeJudgment : public JudgmentBase
    {
    public:
        LongRangeJudgment(Enemy* owner) : JudgmentBase(owner) {}
        const bool Judgment() override;
        void DrawDebug() override {}
    };

    // ----- TackleAttackJudgment -----
    class TackleAttackJudgment : public JudgmentBase
    {
    public:
        TackleAttackJudgment(Enemy* owner) : JudgmentBase(owner) {}
        const bool Judgment() override;
        void DrawDebug() override;

    private:
        int maxCount_ = 3;
        int counter_ = 0;
    };
}

#if 0
// ‚Ğ‚é‚İ”»’è
class FlinchJudgment : public JudgmentBase
{
public:
    FlinchJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// ’Êí‚Ì‹¯‚İ”»’è
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

// ----- ‚R˜A‘±‚½‚½‚«‚Â‚¯UŒ‚ -----
class ComboFlySlamJudgment : public JudgmentBase
{
public:
    ComboFlySlamJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};
#endif  