#include "JudgmentDragon.h"
#include "MathHelper.h"
#include "Character/Player/PlayerManager.h"

// ----- Ž€–S”»’è -----
const bool DeathJudgment::Judgment()
{
    // HP‚ª‚O‚æ‚è¬‚³‚©‚Á‚½‚çŽ€‚ñ‚Å‚¢‚é
    if (owner_->GetHealth() <= 0) return true;

    return false;
}

const bool FlinchJudgment::Judgment()
{
    return false;
}

// ----- ”ñí“¬”»’è -----
const bool NonBattleJudgment::Judgment()
{
    return false;
    return true;

}

// ----- ”ñí“¬Žž‘Ò‹@”»’è -----
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
