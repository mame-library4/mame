#include "JudgmentTamamo.h"
#include "../Other/MathHelper.h"
#include "../Player/PlayerManager.h"

// ----- 死亡判定 -----
const bool DeathJudgment::Judgment()
{
    // HPが０より小さかったら死んでいる
    if (owner_->GetHealth() <= 0) return true;

    return false;
}

// ----- 怯み移行判定 -----
const bool FlinchJudgment::Judgment()
{
    // ひるみフラグがたっていれば、"ひるみ"
    return owner_->GetIsFlinch();
}

#pragma region 非戦闘
// ----- 非戦闘判定 -----
const bool NonBattleJudgment::Judgment()
{
    // 戦闘範囲にいなかったら非戦闘
    if (owner_->SearchPlayer())
    {
        return false;
    }
    else
    {
        return true;
    }
}

// ----- 非戦闘時待機 -----
const bool NonBattleIdleJudgment::Judgment()
{
    return false;
}

// ----- 非戦闘時歩き -----
const bool NonBattleWalkJudgment::Judgment()
{
    return true;
}

#pragma endregion 非戦闘

#pragma region 戦闘

#pragma region 待機系
// 歩き判定
const bool WalkJudgment::Judgment()
{
    return true;
}

#pragma endregion 待機系

#pragma region 戦闘系
// 攻撃判定
const bool AttackJudgment::Judgment()
{
#if 0
    // プレイヤーまでの距離を計算する
    float length = owner_->CalcDistanceToPlayer();

    // 攻撃範囲にいるか
    if (length < owner_->GetFarAttackRadius())
        return true;
    else
        return false;
#else
    DirectX::XMFLOAT2 ownerPos = { owner_->GetTransform()->GetPositionX(), owner_->GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 playerPos = { PlayerManager::Instance().GetTransform()->GetPositionX(), PlayerManager::Instance().GetTransform()->GetPositionZ() };
    DirectX::XMFLOAT2 vec = ownerPos - playerPos;
    float dist = sqrtf(vec.x * vec.x + vec.y * vec.y);

    // 遠距離攻撃範囲にいる
    if (dist < owner_->GetFarAttackRadius())
    {
        // 単位ベクトル化
        vec = vec / dist;

        // 方向ベクトル化
        DirectX::XMFLOAT2 frontVec = { owner_->GetTransform()->CalcForward().x, owner_->GetTransform()->CalcForward().z };
        frontVec = XMFloat2Normalize(frontVec);

        // 前後判定
        float dot = XMFloat2Dot(frontVec, vec);
        if (dot < 0.0f)
        {
            return true;
        }
    }

    return false;

#endif
}

#pragma region 近距離
// 近距離判定
const bool NearAttackJudgment::Judgment()
{
    // プレイヤーまでの距離を計算する
    float length = owner_->CalcDistanceToPlayer();

    // 攻撃範囲にいるか
    if (length < owner_->GetNearAttackRadius())
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 尻尾攻撃判定
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


#pragma endregion 近距離

#pragma region 遠距離
// 遠距離判定
const bool FarAttackJudgment::Judgment()
{
    return true;
}

#pragma endregion 遠距離

#pragma region 叫ぶ系
// 叫ぶ系判定
const bool ShoutJudgment::Judgment()
{
    // 連続攻撃回数が一定数超えたら叫ぶ。
    if (owner_->GetAttackComboCount() > shoutThreshold_)
    {
        return true;
    }

    return false;
}

// 咆哮判定
const bool RoarJudgment::Judgment()
{
    return true;
}

// 威嚇判定
const bool IntimidateJudgment::Judgment()
{
    return false;
}

#pragma endregion 叫ぶ系

#pragma endregion 戦闘系

#pragma endregion 戦闘

