#include "JudgmentTamamo.h"
#include "../Other/MathHelper.h"
#include "../Player/PlayerManager.h"

#if 0
#pragma region ��퓬
// ----- ��퓬���� -----
const bool NonBattleJudgment::Judgment()
{
    // �퓬�͈͂ɂ��Ȃ��������퓬
    if (owner_->SearchPlayer())
    {
        return false;
    }
    else
    {
        return true;
    }
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
#if 0
    // �v���C���[�܂ł̋������v�Z����
    float length = owner_->CalcDistanceToPlayer();

    // �U���͈͂ɂ��邩
    if (length < owner_->GetFarAttackRadius())
        return true;
    else
        return false;
#else
    DirectX::XMFLOAT2 ownerPos = { owner_->GetTransform()->GetPositionX(), owner_->GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 playerPos = { PlayerManager::Instance().GetTransform()->GetPositionX(), PlayerManager::Instance().GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 vec = ownerPos - playerPos;
    float dist = sqrtf(vec.x * vec.x + vec.y * vec.y);

    // �ߋ����U���͈͂ɂ���Ƃ��͋����I�ɍU����ԂɂȂ�
    if (dist < owner_->GetNearAttackRadius())
    {
        return true;
    }

    // �������U���͈͂ɂ���
    if (dist < owner_->GetFarAttackRadius())
    {
        // �P�ʃx�N�g����
        vec = vec / dist;

        // �����x�N�g����
        DirectX::XMFLOAT2 frontVec = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
        frontVec = XMFloat2Normalize(frontVec);

        // �O�㔻��
        float dot = XMFloat2Dot(frontVec, vec);
        if (dot < 0.0f)
        {
            return true;
        }
    }

    return false;

#endif
}

#pragma region �ߋ���
// �ߋ�������
const bool NearAttackJudgment::Judgment()
{
    // �v���C���[�܂ł̋������v�Z����
    float length = owner_->CalcDistanceToPlayer();

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

// ���݂��U������
const bool BiteJudgment::Judgment()
{
    if (rand() % 2 == 0)
    {
        return true;
    }

    return false;
}

// �K���U������
const bool TailSwipeJudgment::Judgment()
{
    DirectX::XMFLOAT3 float3Vec = owner_->CalcDirectionToPlayer();
    DirectX::XMFLOAT2 float2Vec = XMFloat2Normalize({ float3Vec.x, float3Vec.z });
    DirectX::XMFLOAT3 float3Forward = owner_->GetTransform()->CalcForward();
    DirectX::XMFLOAT2 float2Forward = XMFloat2Normalize({ float3Forward.x, float3Forward.z });
    float dot = XMFloat2Dot(float2Vec, float2Forward);

    if (dot > 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}


#pragma endregion �ߋ���

#pragma region ������
// ����������
const bool FarAttackJudgment::Judgment()
{
    return true;
}

#pragma endregion ������

#pragma region ���Ԍn
// ���Ԍn����
const bool ShoutJudgment::Judgment()
{
    return true;

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


#endif