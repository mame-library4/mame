#pragma once
#include "../../../AI/BehaviorTree/ActionBase.h"
#include "Enemy.h"

#if 0
// collision�̗L����,�������𐧌䂷��\����
struct CollisionState
{
    float   startFrame_ = 0.0f;
    float   endFrame_ = 0.0f;   
    bool    isStart_ = false;
    bool    isEnd_ = false;

    void Initialize(const float& startFrame, const float& endFrame, const bool& isStart = false, const bool& isEnd = false);
};



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
        Right,  // �E
        Left,   // ��
    };

private:
    float   actionTimer_ = 0.0f;
    Side    playerSide_ = Side::Right; // �v���C���[�����E�ǂ����ɂ��邩
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

    // ----- �O�i���邽�߂̕ϐ� -----
    float addForceFrame_ = 1.345f;
    bool isAddForce_ = false;

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
    // ---------- ����t���O ---------------
    CollisionState collisionDetection_;
    CollisionState attackDetection_;
    float   disableCollisionPushStartFrame_ = 1.6f; // �����o�����薳���t���[��


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

private:
    float   blurStartFrame_ = 0.0f; // �u���[�J�n�t���[��
    float   blurEndFrame_   = 0.0f; // �u���[�I���t���[��
    float   maxBlurPower_   = 0.0f; // �u���[�̋���
    float   blurTimer_      = 0.0f; // �u���[��Easing�p�^�C�}�[
    float   maxBlurTime_    = 0.0f; // �u���[�p

    bool    isVibration_    = false;// �R���g���[���[�U��
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

#endif