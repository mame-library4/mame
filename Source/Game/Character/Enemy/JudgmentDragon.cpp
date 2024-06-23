#include "JudgmentDragon.h"
#include "MathHelper.h"
#include "Character/Player/PlayerManager.h"

// ----- ���S���� -----
const bool DeathJudgment::Judgment()
{
    // HP���O��菬���������玀��ł���
    if (owner_->GetHealth() <= 0) return true;

    return false;
}

const bool FlinchJudgment::Judgment()
{
    return false;
}

// ----- ��퓬���� -----
const bool NonBattleJudgment::Judgment()
{
    return false;
    return true;

}

// ----- ��퓬���ҋ@���� -----
const bool NonBattleIdleJudgment::Judgment()
{
    return true;

    return false;
}

const bool RoarJudgment::Judgment()
{
    return false;
}

const bool ShoutJudgment::Judgment()
{
    return true;

    return false;
}

const bool NearJudgment::Judgment()
{
    return false;
}
