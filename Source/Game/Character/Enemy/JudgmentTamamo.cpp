#include "JudgmentTamamo.h"
#include "../Other/MathHelper.h"
#include "../Player/PlayerManager.h"

#if 0
#pragma region ”ñí“¬
// ----- ”ñí“¬”»’è -----
const bool NonBattleJudgment::Judgment()
{
    // í“¬”ÍˆÍ‚É‚¢‚È‚©‚Á‚½‚ç”ñí“¬
    if (owner_->SearchPlayer())
    {
        return false;
    }
    else
    {
        return true;
    }
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
#if 0
    // ƒvƒŒƒCƒ„[‚Ü‚Å‚Ì‹——£‚ğŒvZ‚·‚é
    float length = owner_->CalcDistanceToPlayer();

    // UŒ‚”ÍˆÍ‚É‚¢‚é‚©
    if (length < owner_->GetFarAttackRadius())
        return true;
    else
        return false;
#else
    DirectX::XMFLOAT2 ownerPos = { owner_->GetTransform()->GetPositionX(), owner_->GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 playerPos = { PlayerManager::Instance().GetTransform()->GetPositionX(), PlayerManager::Instance().GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 vec = ownerPos - playerPos;
    float dist = sqrtf(vec.x * vec.x + vec.y * vec.y);

    // ‹ß‹——£UŒ‚”ÍˆÍ‚É‚¢‚é‚Æ‚«‚Í‹­§“I‚ÉUŒ‚ó‘Ô‚É‚È‚é
    if (dist < owner_->GetNearAttackRadius())
    {
        return true;
    }

    // ‰“‹——£UŒ‚”ÍˆÍ‚É‚¢‚é
    if (dist < owner_->GetFarAttackRadius())
    {
        // ’PˆÊƒxƒNƒgƒ‹‰»
        vec = vec / dist;

        // •ûŒüƒxƒNƒgƒ‹‰»
        DirectX::XMFLOAT2 frontVec = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
        frontVec = XMFloat2Normalize(frontVec);

        // ‘OŒã”»’è
        float dot = XMFloat2Dot(frontVec, vec);
        if (dot < 0.0f)
        {
            return true;
        }
    }

    return false;

#endif
}

#pragma region ‹ß‹——£
// ‹ß‹——£”»’è
const bool NearAttackJudgment::Judgment()
{
    // ƒvƒŒƒCƒ„[‚Ü‚Å‚Ì‹——£‚ğŒvZ‚·‚é
    float length = owner_->CalcDistanceToPlayer();

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

// Šš‚İ‚Â‚«UŒ‚”»’è
const bool BiteJudgment::Judgment()
{
    if (rand() % 2 == 0)
    {
        return true;
    }

    return false;
}

// K”öUŒ‚”»’è
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


#pragma endregion ‹ß‹——£

#pragma region ‰“‹——£
// ‰“‹——£”»’è
const bool FarAttackJudgment::Judgment()
{
    return true;
}

#pragma endregion ‰“‹——£

#pragma region ‹©‚ÔŒn
// ‹©‚ÔŒn”»’è
const bool ShoutJudgment::Judgment()
{
    return true;

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


#endif