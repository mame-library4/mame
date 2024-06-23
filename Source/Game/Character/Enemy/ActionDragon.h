#pragma once
#include "BehaviorTree/ActionBase.h"
#include "Enemy.h"

// ���S�s��
class DeathAction : public ActionBase
{
public:
    DeathAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    float timer_ = 0.0f;
};

// �Ђ�ݍs��
class FlinchAction : public ActionBase
{
public:
    FlinchAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// ��퓬�ҋ@�s��
class NonBattleIdleAction : public ActionBase
{
public:
    NonBattleIdleAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    float timer_ = 0.0f;
};

// ��퓬�����s��
class NonBattleWalkAction : public ActionBase
{
public:
    NonBattleWalkAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// ���K�s��
class RoarAction : public ActionBase
{
public:
    RoarAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    float vibrationStartFrame_[3] = { 1.0f, 1.55f, 3.0f };
    bool isVibration_[3] = {};

    // ----- Blur -----
    float   blurStartFrame_ = 0.0f; // �u���[�J�n�t���[��
    float   blurEndFrame_   = 0.0f; // �u���[�I���t���[��
    float   maxBlurPower_   = 0.0f; // �u���[�̋���
    float   blurTimer_      = 0.0f; // �u���[��Easing�p�^�C�}�[
    float   maxBlurTime_    = 0.0f; // �u���[�p

};

// �o�b�N�X�e�b�v���K�s��
class BackStepRoarAction : public ActionBase
{
public:
    BackStepRoarAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �o�b�N�X�e�b�v�s��
class BackStepAction : public ActionBase
{
public:
    BackStepAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// ��эU���s��
class FlyAttackAction : public ActionBase
{
public:
    FlyAttackAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �m�b�N�o�b�N�s��
class KnockBackAction : public ActionBase
{
public:
    KnockBackAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// ���������s��
class SlamAction : public ActionBase
{
public:
    SlamAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �O�����U���s��
class FrontAttackAction : public ActionBase
{
public:
    FrontAttackAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �R���{���������s��
class ComboSlamAction : public ActionBase
{
public:
    ComboSlamAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �R���{�`���[�W�s��
class ComboChargeAction : public ActionBase
{
public:
    ComboChargeAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// ��]�U���s��   
class TurnAttackAction : public ActionBase
{
public:
    TurnAttackAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �^�b�N���s��
class TackleAction : public ActionBase
{
public:
    TackleAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �㏸�U���s��
class RiseAttackAction : public ActionBase
{
public:
    RiseAttackAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �ړ���]�s��
class MoveTurnAction : public ActionBase
{
public:
    MoveTurnAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �ړ��U���s��
class MoveAttackAction : public ActionBase
{
public:
    MoveAttackAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};