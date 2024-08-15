#pragma once
#include "BehaviorTree/ActionBase.h"
#include "Enemy.h"

namespace ActionDragon
{
    struct GamePadVibration
    {
    public:
        void Initialize(const float& startFrame, const float& time, const float& power);
        void Update(const float& animationFrame);

    private:
        float startFrame_   = 0.0f;
        float time_         = 0.0f;
        float power_        = 0.0f;
        bool  isVibraion_   = false;
    };

    struct AddForceData
    {
    public:
        void Initialize(const float& addForceFrame, const float& force, const float& decelerationForce = 2.0f);
        bool Update(const float& animationFrame);

        [[nodiscard]] const float GetForce() const { return force_; }
        [[nodiscard]] const float GetDecelerationForce() const { return decelerationForce_; }
        [[nodiscard]] const bool  GetIsAddForce() const { return isAddforce_; }

    private:
        float   addForceFrame_      = 0.0f;
        float   force_              = 0.0f;
        float   decelerationForce_  = 0.0f;
        bool    isAddforce_         = false;
    };
}

namespace ActionDragon
{
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

    private:
        enum class STATE
        {
            Initialize,
            FlinchStart,
            FlinchLoop,
            FlinchEnd,
        };

        void SetAnimationSpeed();
        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        AddForceData addForceData_;

        int loopCounter_ = 0; // ���[�v�����񐔂𐔂���
        int maxLoopNum_  = 0; // �ő僋�[�v��
    };

    // ��s�����ݍs��
    class FlyFlinchAction : public ActionBase
    {
    public:
        FlyFlinchAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        enum class STATE
        {
            Initialize,
            FlinchStart,
            FlinchLoop,
            FlinchEnd,
        };

        void SetAnimationSpeed();
        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        float oldPositionY_ = 0.0f;
        float easingTimer_  = 0.0f; // Easing�p

        int loopCounter_    = 0; // ���[�v�����񐔂𐔂���
        int maxLoopNum_     = 0; // �ő僋�[�v��
    };

    // ��퓬�ҋ@�s��
    class NonBattleIdleAction : public ActionBase
    {
    public:
        NonBattleIdleAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        void SetAnimation();

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
        void UpdateBlur(const float& elapsedTime);

    private:
        GamePadVibration gamePadVibration_;
        float blurTimer_ = 0.0f;
    };

    // ���K�s��(�������)
    class RoarLongAction : public ActionBase
    {
    public:
        RoarLongAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        void UpdateBlur(const float& elapsedTime);

    private:
        static const int vibrationNum_ = 3;
        GamePadVibration gamePadVibration_[vibrationNum_];

        // ----- Blur -----
        float   blurStartFrame_ = 0.0f; // �u���[�J�n�t���[��
        float   blurEndFrame_   = 0.0f; // �u���[�I���t���[��
        float   maxBlurPower_   = 0.0f; // �u���[�̋���
        float   blurTimer_      = 0.0f; // �u���[��Easing�p�^�C�}�[
        float   maxBlurTime_    = 0.0f; // �u���[�p

        bool isPlayerFilnch_ = false;
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

    private:
        enum MoveDirection
        {
            UpBack,
            DownForward,
            Max,
        };

        enum class STATE
        {
            Initialize, // ������
            FlyStart,   // ��юn��
            PreAction,  // �\������
            FlyAttack,  // �U��
        };
        
        void SetStep(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        AddForceData addForceData_[MoveDirection::Max];

        float slowAnimationSpeed_       = 0.0f;
        float slowAnimationEndFrame_    = 0.0f;

        // ----- �\������p -----
        float savePositionY_ = 0;
        float addPositionY_ = 0;
        float easingTimer_ = 0.0f;
        bool isDown_ = false;
        bool isRise_ = false;
    };

    // �m�b�N�o�b�N�s��
    class KnockBackAction : public ActionBase
    {
    public:
        KnockBackAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        enum class STATE
        {
            Initialize, // ������
            Guard,      // �K�[�h
            Loop,       // �K�[�h���[�v
            LoopInit,   // ���[�v������
            Attack,     // �U������
            Failed,     // �U�����s
        };

        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        int loopMax_ = 0;
        int loopCounter_ = 0;
    };

    // ���������s��
    class SlamAction : public ActionBase
    {
    public:
        SlamAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
    };

    // �u���X
    class FireBreath : public ActionBase
    {
    public:
        FireBreath(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        bool isCreateFireball_ = false;
    };

    // �u���X ( �R�A�� )
    class FireBreathCombo : public ActionBase
    {
    public:
        FireBreathCombo(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        void Launch(const float& launchFrame);

    private:
        bool isCreateFireball_ = false;
    };

    // �R���{���������s��
    class ComboSlamAction : public ActionBase
    {
    public:
        ComboSlamAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        enum class STATE
        {
            Initialize, // ������
            Attack0,    // �U���ꔭ��
            Attack1,    // �U���񔭖�
            Recovery,   // �㌄
        };

        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        AddForceData addForceData_;
    };

    // �R���{���������U��(�����킹���Ă���)
    class ComboFlySlamAction : public ActionBase
    {
    public:
        ComboFlySlamAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        enum class STATE
        {
            Initialize, // ������
            Attack,     // ���������U��
            ComboJudge, // �R���{�U�����邩����
            Recovery,   // �㌄
        };

        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        AddForceData    addForceData_;
        int             comboNum_       = 0;
        const int       maxComboNum_    = 2; // �ő�R���{��
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

    private:
        enum class STATE
        {
            Initialize, // ������
            PreAction,  // �\������
            Attack,     // �U��
        };

        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        AddForceData addForceData_;
    };

    // �^�b�N���s��
    class TackleAction : public ActionBase
    {
    public:
        TackleAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        enum class STATE
        {
            Initialize, // ������
            PreAction,  // �\������
            Tackle,     // �^�b�N��
            Recovery,   // �㌄
        };

        void SetState(const STATE& state) { owner_->SetStep(static_cast<int>(state)); }

    private:
        AddForceData addForceData_;
        float easingTimer_ = 0.0f;
    };

    // �㏸�U���s��
    class RiseAttackAction : public ActionBase
    {
    public:
        RiseAttackAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;

    private:
        float riseTimer_ = 0.0f;
        bool isCameraSet_ = false; // �J�������Z�b�g�p
        bool isCameraReset_ = false; // �J�������Z�b�g�p

        AddForceData addForceData_;
    };

    // �ړ���]�s��
    class MoveTurnAction : public ActionBase
    {
    public:
        MoveTurnAction(Enemy* owner) : ActionBase(owner) {}
        const ActionBase::State Run(const float& elapsedTime) override;
    };
}