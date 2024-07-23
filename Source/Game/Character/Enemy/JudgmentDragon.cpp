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
    return owner_->GetIsFlinch();
}

// ----- �ʏ�Ђ�ݔ��� -----
const bool NormalFlinchJudgment::Judgment()
{
    // �h���S����Y�l���O�łȂ��ꍇ�󒆂ɂ���̂� false
    if (owner_->GetTransform()->GetPositionY() != 0.0f) return false;

    // �n�ʂɂ��� true
    return true;
}

// ----- ��퓬���� -----
const bool NonBattleJudgment::Judgment()
{
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


