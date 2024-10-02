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
    bool flinch = false;

    if (owner_->GetIsFlinch()) flinch = true;

    // ‹r‚ª•”ˆÊ”j‰ó‚³‚ê‚Ä‚¢‚ÄA“ËiUŒ‚‚ğ‚µ‚½ê‡‚Ğ‚é‚Ş
    if (owner_->GetIsPartDestruction(Enemy::PartName::Leg))
    {
        if (owner_->GetAnimationIndex() == static_cast<int>(Enemy::DragonAnimation::AttackTackle3))
        {
            flinch = true;
        }
    }

    return flinch;
}

// ----- ’Êí‚Ğ‚é‚İ”»’è -----
const bool NormalFlinchJudgment::Judgment()
{
    // ƒhƒ‰ƒSƒ“‚ÌY’l‚ª‚O‚Å‚È‚¢ê‡‹ó’†‚É‚¢‚é‚Ì‚Å false
    if (owner_->GetTransform()->GetPositionY() > 0.0f) return false;

    // ’n–Ê‚É‚¢‚é true
    return true;
}

const bool PartDestructionFlinchJudgment::Judgment()
{
    // ‹r‚ª•”ˆÊ”j‰ó‚³‚ê‚Ä‚¢‚ÄA“ËiUŒ‚‚ğ‚µ‚½ê‡‚Ğ‚é‚Ş
    if (owner_->GetIsPartDestruction(Enemy::PartName::Leg))
    {
        if (owner_->GetAnimationIndex() == static_cast<int>(Enemy::DragonAnimation::AttackTackle3))
        {
            return true;
        }
    }

    return false;
}

// ----- ”ñí“¬”»’è -----
const bool NonBattleJudgment::Judgment()
{
    return false;
    //return true;

    if (num_ == 0)
    {
        ++num_;
        return true;
    }
    else if (num_ < 3)
    {
        ++num_;
        return false;
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
    //return true;

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
