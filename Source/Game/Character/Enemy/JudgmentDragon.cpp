#include "JudgmentDragon.h"
#include "MathHelper.h"
#include "Character/Player/PlayerManager.h"

// ----- €–S”»’è -----
namespace ActionDragon
{
    const bool DeathJudgment::Judgment()
    {
        // HP‚ª‚O‚æ‚è¬‚³‚©‚Á‚½‚ç€‚ñ‚Å‚¢‚é
        if (owner_->GetHealth() <= 0) return true;

        return false;
    }
}

#pragma region ---------- Down ----------
namespace ActionDragon
{
    // ----- DownJudgment -----
    const bool DownJudgment::Judgment()
    {
        if (owner_->GetIsStagger() == false) return false;

        return true;
    }

    // ----- KnockDownJudgment -----
    const bool KnockDownJudgment::Judgment()
    {
        const Player::STATE currentState = PlayerManager::Instance().GetPlayer()->GetCurrentState();
        if (currentState == Player::STATE::RushAttack &&
            owner_->GetStaggerPartIndex() == static_cast<int>(Enemy::PartName::Leg))
        {
            return true;
        }

        return false;
    }
}

#pragma endregion ---------- Down ----------

// ----- LongRangeJudgment -----
namespace ActionDragon
{
    const bool LongRangeJudgment::Judgment()
    {
        if (owner_->CalcDistanceToPlayerNoConsiderationY() > owner_->GetLongRangeRadius())
        {
            return true;
        }

        return false;
    }
}

// ----- TackleAttackJudgment -----
namespace ActionDragon
{
    const bool TackleAttackJudgment::Judgment()
    {
        ++counter_;

        if (counter_ <= maxCount_) return false;
        counter_ = 0;

        return true;
    }

    // ----- ImGui—p -----
    void TackleAttackJudgment::DrawDebug()
    {
        if (ImGui::TreeNodeEx("TackleAttackJudgment", ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragInt("MaxCount", &maxCount_);
            ImGui::DragInt("Counter", &counter_);

            ImGui::TreePop();
        }
    }
}

// ----- RoarJudgment -----
namespace ActionDragon
{
    const bool RoarJudgment::Judgment()
    {
        ++counter_;

        if (counter_ <= maxCount_) return false;
        counter_ = 0;

        return true;
    }

    void RoarJudgment::DrawDebug()
    {
        if (ImGui::TreeNodeEx("RoarJudgment", ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragInt("MaxCount", &maxCount_);
            ImGui::DragInt("Counter", &counter_);

            ImGui::TreePop();
        }
    }
}

// ----- SuperNovaJudgment -----
namespace ActionDragon
{
    const bool SuperNovaJudgment::Judgment()
    {
        const float halfHealth = owner_->GetMaxHealth() * 0.5f;
        if (owner_->GetHealth() > halfHealth) return false;

        // ‚S‰ñ‚É‚P‰ñ‚µ‚©’Ê‚ç‚È‚¢‚Ì‚Å˜A‘±‚µ‚Äo‚·‚±‚Æ‚ğ”ğ‚¯‚é
        ++counter_;
        if (counter_ <= maxCount_) return false;
        counter_ = 0;

        return true;
    }

    // ----- ImGui—p -----
    void SuperNovaJudgment::DrawDebug()
    {
        if (ImGui::TreeNodeEx("SuperNovaJudgment", ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragInt("MaxCount", &maxCount_);
            ImGui::DragInt("Counter", &counter_);

            ImGui::TreePop();
        }
    }
}

// ----- PowerAttackJudgment -----
namespace ActionDragon
{
    const bool PowerAttackJudgment::Judgment()
    {
        if (isFirstTime_ == false) return false;

        const float halfHealth = owner_->GetMaxHealth() * 0.5f;
        if (owner_->GetHealth() > halfHealth) return false;

        isFirstTime_ = false;
        return true;
    }

    // ----- ImGui—p -----
    void PowerAttackJudgment::DrawDebug()
    {
        if (ImGui::TreeNodeEx("PowerAttackJudgment", ImGuiTreeNodeFlags_Framed))
        {
            ImGui::Checkbox("IsFirstTime", &isFirstTime_);

            ImGui::TreePop();
        }
    }
}

#if 0
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


// ----- ‹©‚Ô”»’è -----
const bool ShoutJudgment::Judgment()
{
    //return true;
    return false;

    return owner_->GetIsRoar() == false;
}

const bool NearJudgment::Judgment()
{
    //return false;
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
#endif

