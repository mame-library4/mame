#pragma once
#include "../../../AI/BehaviorTree/JudgmentBase.h"
#include "Enemy.h"



#pragma region �퓬

#pragma region �ҋ@�n
// ��������
class WalkJudgment : public JudgmentBase
{
public:
    WalkJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

#pragma endregion �ҋ@�n

#pragma region �U���n
// �U������
class AttackJudgment : public JudgmentBase
{
public:
    AttackJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};


#pragma region �ߋ���
// �ߋ����U������
class NearAttackJudgment : public JudgmentBase
{
public:
    NearAttackJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

class BiteJudgment : public JudgmentBase
{
public:
    BiteJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// �K���U������
class TailSwipeJudgment : public JudgmentBase
{
public:
    TailSwipeJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

#pragma endregion �ߋ���


#pragma region ������
// �������U������
class FarAttackJudgment : public JudgmentBase
{
public:
    FarAttackJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

#pragma endregion ������

#pragma region ���Ԍn
// ���Ԍn����
class ShoutJudgment : public JudgmentBase
{
public:
    ShoutJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;

private:
    int shoutThreshold_ = 5; // ���т��\�ɂȂ��
};

// ���K����
class RoarJudgment : public JudgmentBase
{
public:
    RoarJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

// �Њd����
class IntimidateJudgment : public JudgmentBase
{
public:
    IntimidateJudgment(Enemy* owner) : JudgmentBase(owner) {}
    const bool Judgment() override;
};

#pragma endregion ���Ԍn

#pragma endregion �U���n

#pragma endregion �퓬