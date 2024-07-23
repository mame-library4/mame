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

// ----- ひるみ判定 -----
const bool FlinchJudgment::Judgment()
{
    //return true;
    return owner_->GetIsFlinch();
}

// ----- 非戦闘判定 -----
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

// ----- 非戦闘時待機判定 -----
const bool NonBattleIdleJudgment::Judgment()
{
    return true;

    return false;
}

// ----- 咆哮判定 -----
const bool RoarJudgment::Judgment()
{
    return true;
}

// ----- 叫ぶ判定 -----
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
