#include "JudgmentDragon.h"
#include "MathHelper.h"
#include "Character/Player/PlayerManager.h"

// ----- €–S”»’è -----
const bool DeathJudgment::Judgment()
{
    // HP‚ª‚O‚æ‚è¬‚³‚©‚Á‚½‚ç€‚ñ‚Å‚¢‚é
    if (owner_->GetHealth() <= 0) return true;

    return false;
}

// ----- ‚Ğ‚é‚İ”»’è -----
const bool FlinchJudgment::Judgment()
{
    //return true;
    return owner_->GetIsFlinch();
}

// ----- ”ñí“¬”»’è -----
const bool NonBattleJudgment::Judgment()
{
    return true;

    if (num_ == 0)
    {
        ++num_;
        return true;
    }
    else
    {
        num_ = 0;
        return false;
    }
}

// ----- ”ñí“¬‘Ò‹@”»’è -----
const bool NonBattleIdleJudgment::Judgment()
{
    return true;

    return false;
}

// ----- ™ôšK”»’è -----
const bool RoarJudgment::Judgment()
{
    return true;
}

// ----- ‹©‚Ô”»’è -----
const bool ShoutJudgment::Judgment()
{
    return false;
    return owner_->GetIsRoar() == false;
}

const bool NearJudgment::Judgment()
{
    //if (num_ == 0)
    //{
    //    num_ = 1;
    //    return true;
    //}
    //else
    //{
    //    num_ = 0;
    //    return false;
    //}

    return true;
    return false;

}
