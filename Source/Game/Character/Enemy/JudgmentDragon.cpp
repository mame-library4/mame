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

// ----- �Ђ�ݔ��� -----
const bool FlinchJudgment::Judgment()
{
    bool flinch = false;

    if (owner_->GetIsFlinch()) flinch = true;

    // �r�����ʔj�󂳂�Ă��āA�ːi�U���������ꍇ�Ђ��
    if (owner_->GetIsPartDestruction(Enemy::PartName::Leg))
    {
        if (owner_->GetAnimationIndex() == static_cast<int>(Enemy::DragonAnimation::AttackTackle3))
        {
            flinch = true;
        }
    }

    return flinch;
}

// ----- �ʏ�Ђ�ݔ��� -----
const bool NormalFlinchJudgment::Judgment()
{
    // �h���S����Y�l���O�łȂ��ꍇ�󒆂ɂ���̂� false
    if (owner_->GetTransform()->GetPositionY() > 0.0f) return false;

    // �n�ʂɂ��� true
    return true;
}

const bool PartDestructionFlinchJudgment::Judgment()
{
    // �r�����ʔj�󂳂�Ă��āA�ːi�U���������ꍇ�Ђ��
    if (owner_->GetIsPartDestruction(Enemy::PartName::Leg))
    {
        if (owner_->GetAnimationIndex() == static_cast<int>(Enemy::DragonAnimation::AttackTackle3))
        {
            return true;
        }
    }

    return false;
}

// ----- ��퓬���� -----
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

// ----- ��퓬���ҋ@���� -----
const bool NonBattleIdleJudgment::Judgment()
{
    return true;

    return false;
}

// ----- ���K���� -----
const bool RoarJudgment::Judgment()
{
    return true;
}

// ----- ���Ԕ��� -----
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
