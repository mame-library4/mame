#pragma once
#include "../../../AI/BehaviorTree/ActionBase.h"
#include "Enemy.h"

// ���S�s��
class DeathAction : public ActionBase
{
public:
    DeathAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �Ђ�ݍs��
class FlinchAction : public ActionBase
{
public:
    FlinchAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma region ��퓬
// ��퓬�ҋ@�s��
class NonBattleIdleAction : public ActionBase
{
public:
    NonBattleIdleAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// ��퓬�����s��
class NonBattleWalkAction : public ActionBase
{
public:
    NonBattleWalkAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma endregion ��퓬

#pragma region �퓬

#pragma region �ҋ@�n
// �����s��
class WalkAction : public ActionBase
{
public:
    WalkAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

    enum class Side
    {
        None,   // �Ȃ�
        Right,  // �E
        Left,   // ��
    };

private:
    float   actionTimer_ = 0.0f;
    Side    playerSide_ = Side::None; // �v���C���[�����E�ǂ����ɂ��邩
};

class StepAction : public ActionBase
{
public:
    StepAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma endregion �ҋ@�n

#pragma region �U���n
// ���݂��s��
class BiteAction : public ActionBase
{
public:
    BiteAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    float   attackCollisionEndFrame_    = 2.0f;     // �U�����薳���t���[��
    bool    isAttackCollisionEnd_       = false;    // ����t���O

};

// �Ђ������s��
class SlashAction : public ActionBase
{
public:
    SlashAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    float   attackCollisionEndFrame_    = 1.5f;     // �U�����薳���ɂȂ�A�j���[�V�����t���[��
    bool    isAttackCollisionEnd_       = false;    // �����񏈗����Ȃ��悤�ɂ��鐧��t���O
};

// �K���s��
class TailSwipeAction : public ActionBase
{
public:
    TailSwipeAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// ���������s��
class SlamAction : public ActionBase
{
public:
    SlamAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;

private:
    enum class Step
    {
        Initialize, // ������
        Search,     // �_����߂�
        Ascend,     // �Ƃт�����
        Attack,     // �U��
        Wait, // �U���I����̎���
    };
    void SetStep(const Step& step) { owner_->SetStep(static_cast<int>(step)); }

    void UpdateAttackCollision();

private:
    // ---------- �U������ ---------------
    float   attackCollisionStartFrame_  = 1.9f;     // �U������L���t���[��
    float   attackCollisionEndFrame_    = 3.0f;     // �U�����薳���t���[��
    bool    isAttackCollisionStart_     = false;    // ����t���O
    bool    isAttackCollisionEnd_       = false;    // ����t���O


    float               stateChangeFrame_   = 1.3f; // �X�e�[�g��؂�ւ���A�j���[�V�����̃t���[��
    float               ascendEndFrame_     = 1.9f; // �㏸�I���t���[��

    float attackEndFrame_ = 2.2f; // ���������I���t���[��

    DirectX::XMFLOAT3   targetPosition_     = {};   // �Ƃт�����ʒu

    float maxAscend_ = 2.0f; // �ő�㏸�l

    float easingTimer_ = 0.0f;

    float length_ = 0.0f;

};

#pragma endregion �U���n

#pragma region ���Ԍn
// ���K�s��
class RoarAction : public ActionBase
{
public:
    RoarAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

// �Њd�s��
class IntimidateAction : public ActionBase
{
public:
    IntimidateAction(Enemy* owner) : ActionBase(owner) {}
    const ActionBase::State Run(const float& elapsedTime) override;
};

#pragma endregion ���Ԍn

#pragma endregion �퓬