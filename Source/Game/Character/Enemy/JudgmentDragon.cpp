#include "JudgmentDragon.h"
#include "MathHelper.h"
#include "Character/Player/PlayerManager.h"

// ----- 死亡判定 -----
const bool DeathJudgment::Judgment()
{
    // HPが０より小さかったら死んでいる
    if (owner_->GetHealth() <= 0) return true;

    return false;
}

const bool FlinchJudgment::Judgment()
{
    return true;

    return false;
}

const bool NonBattleJudgment::Judgment()
{
    return false;
}

const bool NonBattleIdleJudgment::Judgment()
{
    return false;
}

const bool RoarJudgment::Judgment()
{
    return false;
}

const bool ShoutJudgment::Judgment()
{
    return false;
}

const bool NearJudgment::Judgment()
{
    return false;
}
