#include "JudgmentTamamo.h"
#include "../Other/MathHelper.h"
#include "../Player/PlayerManager.h"

// ----- ���S���� -----
const bool DeathJudgment::Judgment()
{
    // HP���O��菬���������玀��ł���
    if (owner_->GetHealth() <= 0) return true;

    return false;
}

// ----- ���݈ڍs���� -----
const bool FlinchJudgment::Judgment()
{
    // �Ђ�݃t���O�������Ă���΁A"�Ђ��"
    return owner_->GetIsFlinch();
}

#pragma region ��퓬
// ----- ��퓬���� -----
const bool NonBattleJudgment::Judgment()
{
    // �v���C���[�܂ł̋������v�Z����
    float length = owner_->CalcPlayerDistance();

    // �퓬�͈͂ɂ��Ȃ��������퓬
    if (length > owner_->GetBattleRadius()) 
        return true;
    else
        return false;
}

// ----- ��퓬���ҋ@ -----
const bool NonBattleIdleJudgment::Judgment()
{
    return false;
}

// ----- ��퓬������ -----
const bool NonBattleWalkJudgment::Judgment()
{
    return true;
}

#pragma endregion ��퓬

#pragma region �퓬

#pragma region �ҋ@�n
// ��������
const bool WalkJudgment::Judgment()
{
    return true;
}

#pragma endregion �ҋ@�n

#pragma region �퓬�n
// �U������
const bool AttackJudgment::Judgment()
{
    // �v���C���[�܂ł̋������v�Z����
    float length = owner_->CalcPlayerDistance();

    // �U���͈͂ɂ��邩
    if (length < owner_->GetAFarAttackRadius())
        return true;
    else
        return false;
}

#pragma region �ߋ���
// �ߋ�������
const bool NearAttackJudgment::Judgment()
{
    // �v���C���[�܂ł̋������v�Z����
    float length = owner_->CalcPlayerDistance();

    // �U���͈͂ɂ��邩
    if (length < owner_->GetNearAttackRadius())
    {
        return true;
    }
    else
    {
        return false;
    }
}

#pragma endregion �ߋ���

#pragma region ������
// ����������
const bool FarAttackJudgment::Judgment()
{
    return false;
}

#pragma endregion ������

#pragma region ���Ԍn
// ���Ԍn����
const bool ShoutJudgment::Judgment()
{
    // �A���U���񐔂���萔�������狩�ԁB
    if (owner_->GetAttackComboCount() > shoutThreshold_)
    {
        return true;
    }

    return false;
}

// ���K����
const bool RoarJudgment::Judgment()
{
    return true;
}

// �Њd����
const bool IntimidateJudgment::Judgment()
{
    return false;
}

#pragma endregion ���Ԍn

#pragma endregion �퓬�n

#pragma endregion �퓬

