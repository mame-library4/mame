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

// ----- ‚Ð‚é‚Ý”»’è -----
const bool FlinchJudgment::Judgment()
{
    //return true;
    return owner_->GetIsFlinch();
}

// ----- ’Êí‚Ð‚é‚Ý”»’è -----
const bool NormalFlinchJudgment::Judgment()
{
    // ƒhƒ‰ƒSƒ“‚ÌY’l‚ª‚O‚Å‚È‚¢ê‡‹ó’†‚É‚¢‚é‚Ì‚Å false
    if (owner_->GetTransform()->GetPositionY() != 0.0f) return false;

    // ’n–Ê‚É‚¢‚é true
    return true;
}

// ----- ”ñí“¬”»’è -----
const bool NonBattleJudgment::Judgment()
{
    //return false;
    //return true;

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

// ----- ”ñí“¬Žž‘Ò‹@”»’è -----
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
    //return true;
    return false;
    return owner_->GetIsRoar() == false;
}

const bool NearJudgment::Judgment()
{
    return true;

    const DirectX::XMFLOAT3 ownerPos = owner_->GetTransform()->GetPosition();
    const DirectX::XMFLOAT3 playerPos = PlayerManager::Instance().GetTransform()->GetPosition();
    const float length = XMFloat3Length(ownerPos - playerPos);

    if (length < owner_->GetNearAttackRadius())
    {
        return true;
    }
    else
    {
        return false;
    }
}

const bool ComboFlySlamJudgment::Judgment()
{
    return false;

    const float distance = owner_->CalcDistanceToPlayer();

    if (distance > owner_->GetComboFlyAttackRadius()) return true;

    return false;
}
