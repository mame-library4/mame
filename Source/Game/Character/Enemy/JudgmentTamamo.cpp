#include "JudgmentTamamo.h"
#include "../Other/MathHelper.h"
#include "../Player/PlayerManager.h"

// ----- €–S”»’è -----
const bool DeathJudgment::Judgment()
{
    // HP‚ª‚O‚æ‚è¬‚³‚©‚Á‚½‚ç€‚ñ‚Å‚¢‚é
    if (owner_->GetHealth() <= 0) return true;

    return false;
}

// ----- ‹¯‚İˆÚs”»’è -----
const bool FlinchJudgment::Judgment()
{
    // ‚Ğ‚é‚İƒtƒ‰ƒO‚ª‚½‚Á‚Ä‚¢‚ê‚ÎA"‚Ğ‚é‚İ"
    return owner_->GetIsFlinch();
}

#pragma region ”ñí“¬
// ----- ”ñí“¬”»’è -----
const bool NonBattleJudgment::Judgment()
{
    // ƒvƒŒƒCƒ„[‚Ü‚Å‚Ì‹——£‚ğŒvZ‚·‚é
    float length = owner_->CalcPlayerDistance();

    // í“¬”ÍˆÍ‚É‚¢‚È‚©‚Á‚½‚ç”ñí“¬
    if (length > owner_->GetBattleRadius()) 
        return true;
    else
        return false;
}

// ----- ”ñí“¬‘Ò‹@ -----
const bool NonBattleIdleJudgment::Judgment()
{
    return false;
}

// ----- ”ñí“¬•à‚« -----
const bool NonBattleWalkJudgment::Judgment()
{
    return true;
}

#pragma endregion ”ñí“¬

#pragma region í“¬

#pragma region ‘Ò‹@Œn
// •à‚«”»’è
const bool WalkJudgment::Judgment()
{
    return true;
}

#pragma endregion ‘Ò‹@Œn

#pragma region í“¬Œn
// UŒ‚”»’è
const bool AttackJudgment::Judgment()
{
    // ƒvƒŒƒCƒ„[‚Ü‚Å‚Ì‹——£‚ğŒvZ‚·‚é
    float length = owner_->CalcPlayerDistance();

    // UŒ‚”ÍˆÍ‚É‚¢‚é‚©
    if (length < owner_->GetAFarAttackRadius())
        return true;
    else
        return false;
}

#pragma region ‹ß‹——£
// ‹ß‹——£”»’è
const bool NearAttackJudgment::Judgment()
{
    // ƒvƒŒƒCƒ„[‚Ü‚Å‚Ì‹——£‚ğŒvZ‚·‚é
    float length = owner_->CalcPlayerDistance();

    // UŒ‚”ÍˆÍ‚É‚¢‚é‚©
    if (length < owner_->GetNearAttackRadius())
    {
        return true;
    }
    else
    {
        return false;
    }
}

#pragma endregion ‹ß‹——£

#pragma region ‰“‹——£
// ‰“‹——£”»’è
const bool FarAttackJudgment::Judgment()
{
    return false;
}

#pragma endregion ‰“‹——£

#pragma region ‹©‚ÔŒn
// ‹©‚ÔŒn”»’è
const bool ShoutJudgment::Judgment()
{
    // ˜A‘±UŒ‚‰ñ”‚ªˆê’è”’´‚¦‚½‚ç‹©‚ÔB
    if (owner_->GetAttackComboCount() > shoutThreshold_)
    {
        return true;
    }

    return false;
}

// ™ôšK”»’è
const bool RoarJudgment::Judgment()
{
    return true;
}

// ˆĞŠd”»’è
const bool IntimidateJudgment::Judgment()
{
    return false;
}

#pragma endregion ‹©‚ÔŒn

#pragma endregion í“¬Œn

#pragma endregion í“¬

