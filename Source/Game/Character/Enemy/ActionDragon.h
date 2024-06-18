#pragma once
#include "BehaviorTree/ActionBase.h"
#include "Enemy.h"

// 死亡行動
class DeathAction : public ActionBase
{
public:
    DeathAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    float timer_ = 0.0f;
};

// ひるみ行動
class FlinchAction : public ActionBase
{
public:
    FlinchAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

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

// 咆哮行動
class RoarAction : public ActionBase
{
public:
    RoarAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// バックステップ咆哮行動
class BackStepRoarAction : public ActionBase
{
public:
    BackStepRoarAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// 移動咆哮行動
class MoveRoarAction : public ActionBase
{
public:
    MoveRoarAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// バックステップ行動
class BackStepAction : public ActionBase
{
public:
    BackStepAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// 飛び攻撃行動
class FlyAttackAction : public ActionBase
{
public:
    FlyAttackAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// ノックバック行動
class KnockBackAction : public ActionBase
{
public:
    KnockBackAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// たたきつけ行動
class SlamAction : public ActionBase
{
public:
    SlamAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// 前方向攻撃行動
class FrontAttackAction : public ActionBase
{
public:
    FrontAttackAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// コンボたたきつけ行動
class ComboSlamAction : public ActionBase
{
public:
    ComboSlamAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// コンボチャージ行動
class ComboChargeAction : public ActionBase
{
public:
    ComboChargeAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// 回転攻撃行動   
class TurnAttackAction : public ActionBase
{
public:
    TurnAttackAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// タックル行動
class TackleAction : public ActionBase
{
public:
    TackleAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// 上昇攻撃行動
class RiseAttackAction : public ActionBase
{
public:
    RiseAttackAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// 移動回転行動
class MoveTurnAction : public ActionBase
{
public:
    MoveTurnAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// 移動攻撃行動
class MoveAttackAction : public ActionBase
{
public:
    MoveAttackAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};