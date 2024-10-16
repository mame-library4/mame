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
    bool flinch = false;

    if (owner_->GetIsFlinch()) flinch = true;

    // 脚が部位破壊されていて、突進攻撃をした場合ひるむ
    if (owner_->GetIsPartDestruction(Enemy::PartName::Leg))
    {
        if (owner_->GetAnimationIndex() == static_cast<int>(Enemy::DragonAnimation::AttackTackle3))
        {
            flinch = true;
        }
    }

    return flinch;
}

// ----- 通常ひるみ判定 -----
const bool NormalFlinchJudgment::Judgment()
{
    // ドラゴンのY値が０でない場合空中にいるので false
    if (owner_->GetTransform()->GetPositionY() > 0.0f) return false;

    // 地面にいる true
    return true;
}

const bool PartDestructionFlinchJudgment::Judgment()
{
    // 脚が部位破壊されていて、突進攻撃をした場合ひるむ
    if (owner_->GetIsPartDestruction(Enemy::PartName::Leg))
    {
        if (owner_->GetAnimationIndex() == static_cast<int>(Enemy::DragonAnimation::AttackTackle3))
        {
            return true;
        }
    }

    return false;
}

// ----- 非戦闘判定 -----
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
