#pragma once
#include "../../../AI/BehaviorTree/JudgmentBase.h"
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

#pragma region 非戦闘
// 非戦闘判定
class NonBattleJudgment : public JudgmentBase
{
public:
    NonBattleJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// 非戦闘時待機判定
class NonBattleIdleJudgment : public JudgmentBase
{
public:
    NonBattleIdleJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// 非戦闘時歩き判定
class NonBattleWalkJudgment : public JudgmentBase
{
public:
    NonBattleWalkJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

#pragma endregion 非戦闘

#pragma region 戦闘

#pragma region 待機系
// 歩き判定
class WalkJudgment : public JudgmentBase
{
public:
    WalkJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

#pragma endregion 待機系

#pragma region 攻撃系
// 攻撃判定
class AttackJudgment : public JudgmentBase
{
public:
    AttackJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};


#pragma region 近距離
// 近距離攻撃判定
class NearAttackJudgment : public JudgmentBase
{
public:
    NearAttackJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// 尻尾攻撃判定
class TailSwipeJudgment : public JudgmentBase
{
public:
    TailSwipeJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

#pragma endregion 近距離


#pragma region 遠距離
// 遠距離攻撃判定
class FarAttackJudgment : public JudgmentBase
{
public:
    FarAttackJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

#pragma endregion 遠距離

#pragma region 叫ぶ系
// 叫ぶ系判定
class ShoutJudgment : public JudgmentBase
{
public:
    ShoutJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;

private:
    int shoutThreshold_ = 5; // 叫びが可能になる回数
};

// 咆哮判定
class RoarJudgment : public JudgmentBase
{
public:
    RoarJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// 威嚇判定
class IntimidateJudgment : public JudgmentBase
{
public:
    IntimidateJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

#pragma endregion 叫ぶ系

#pragma endregion 攻撃系

#pragma endregion 戦闘