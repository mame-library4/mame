#pragma once
#include "../../../AI/BehaviorTree/ActionBase.h"
#include "Enemy.h"

// 死亡行動
class DeathAction : public ActionBase
{
public:
    DeathAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// ひるみ行動
class FlinchAction : public ActionBase
{
public:
    FlinchAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma region 非戦闘
// 非戦闘待機行動
class NonBattleIdleAction : public ActionBase
{
public:
    NonBattleIdleAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// 非戦闘歩き行動
class NonBattleWalkAction : public ActionBase
{
public:
    NonBattleWalkAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma endregion 非戦闘

#pragma region 戦闘

#pragma region 待機系
// 歩き行動
class WalkAction : public ActionBase
{
public:
    WalkAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

    enum class Side
    {
        None,   // なし
        Right,  // 右
        Left,   // 左
    };

private:
    float   actionTimer_ = 0.0f;
    Side    playerSide_ = Side::None; // プレイヤーが左右どっちにいるか
};

class StepAction : public ActionBase
{
public:
    StepAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma endregion 待機系

#pragma region 攻撃系
// 噛みつき行動
class BiteAction : public ActionBase
{
public:
    BiteAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    float   attackCollisionEndFrame_    = 2.0f;     // 攻撃判定無効フレーム
    bool    isAttackCollisionEnd_       = false;    // 制御フラグ

};

// ひっかき行動
class SlashAction : public ActionBase
{
public:
    SlashAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    float   attackCollisionEndFrame_    = 1.5f;     // 攻撃判定無効になるアニメーションフレーム
    bool    isAttackCollisionEnd_       = false;    // 複数回処理しないようにする制御フラグ
};

// 尻尾行動
class TailSwipeAction : public ActionBase
{
public:
    TailSwipeAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// たたきつけ行動
class SlamAction : public ActionBase
{
public:
    SlamAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    enum class Step
    {
        Initialize, // 初期化
        Search,     // 狙い定める
        Ascend,     // とびかかる
        Attack,     // 攻撃
        Wait, // 攻撃終了後の時間
    };
    void SetStep(const Step& step) { owner_->SetStep(static_cast<int>(step)); }

    void UpdateAttackCollision();

private:
    // ---------- 攻撃判定 ---------------
    float   attackCollisionStartFrame_  = 1.9f;     // 攻撃判定有効フレーム
    float   attackCollisionEndFrame_    = 3.0f;     // 攻撃判定無効フレーム
    bool    isAttackCollisionStart_     = false;    // 制御フラグ
    bool    isAttackCollisionEnd_       = false;    // 制御フラグ


    float               stateChangeFrame_   = 1.3f; // ステートを切り替えるアニメーションのフレーム
    float               ascendEndFrame_     = 1.9f; // 上昇終了フレーム

    float attackEndFrame_ = 2.2f; // たたきつけ終了フレーム

    DirectX::XMFLOAT3   targetPosition_     = {};   // とびかかる位置

    float maxAscend_ = 2.0f; // 最大上昇値

    float easingTimer_ = 0.0f;

    float length_ = 0.0f;

};

#pragma endregion 攻撃系

#pragma region 叫ぶ系
// 咆哮行動
class RoarAction : public ActionBase
{
public:
    RoarAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// 威嚇行動
class IntimidateAction : public ActionBase
{
public:
    IntimidateAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma endregion 叫ぶ系

#pragma endregion 戦闘