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
    // プレイヤーまでの距離を計算する
    float length = owner_->CalcPlayerDistance();

    // 戦闘範囲にいなかったら非戦闘
    if (length > owner_->GetBattleRadius()) 
        return true;
    else
        return false;
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
    // プレイヤーまでの距離を計算する
    float length = owner_->CalcPlayerDistance();

    // 攻撃範囲にいるか
    if (length < owner_->GetAFarAttackRadius())
        return true;
    else
        return false;
}

#pragma region 近距離
// 近距離判定
const bool NearAttackJudgment::Judgment()
{
    // プレイヤーまでの距離を計算する
    float length = owner_->CalcPlayerDistance();

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

#pragma endregion 近距離

#pragma region 遠距離
// 遠距離判定
const bool FarAttackJudgment::Judgment()
{
    return false;
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

