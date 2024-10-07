#pragma once
#include <memory>
#include "../Character.h"
#include "Graphics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorData.h"
#include "BehaviorTree/NodeBase.h"

class Enemy : public Character
{
public:
    enum class DragonAnimation
    {
        Idle0,          // �ҋ@0
        Idle1,          // �ҋ@�P
        Walk,           // ����
        Run,            // ����
        
        // ----- �_���[�W�H�炢 -----
        GetHitStart,    // �H�炢�n��    
        GetHitLoop,     // �H�炢���[�v
        GetHitEnd,      // �H�炢�I���

        // ----- ���S -----
        Death,          // ���S
        DeathLoop,      // ���S���[�v

        Roar,           // ���K
        
        AttackSlam0,
        AttackSlam1,

        AttackStep,

        // ----- �ːi�U�� -----
        AttackTackle0,  // �\������
        AttackTackle1,  // �ːi�n�܂�
        AttackTackle2,  // �ːi
        AttackTackle3,  // �㌄

        BackStep,       // �o�b�N�X�e�b�v

        // ----- �㏸�U�� -----
        AttackRise,     // �㏸
        AttackRiseLoop, // �㏸���[�v
        AttackRiseEnd,  // �U��

        // ----- �󒆂��炽�������U�� -----
        AttackFly0,     // �󒆂֔��
        AttackFly1,     // �󒆑ҋ@
        AttackFly2,     // ���������U��

        FireBreathFront,

        BackStepRoar,   // �o�b�N�X�e�b�v��,���K


        // ----- ������΂��U�� -----
        AttackKnockBackStart,
        AttackKnockBackLoop,
        AttackKnockBackEnd0,
        AttackKnockBackEnd1,

        SleepEnd,

        // ----- �N���e�B�J�� -----
        CriticalStart,
        CriticalLoop,
        CriticalEnd,

        // ----- �R���{���������U�� -----
        AttackComboSlam0,
        AttackComboSlam1,
        AttackComboSlamEnd,

        // ----- ���уR���{ -----
        ComboRoarStart,
        ComboRoarLoop,
        ComboRoarEnd0,
        ComboRoarEnd1,

        BackStepAttack,

        // ----- �R���{���ߍU�� -----
        AttackComboCharge0,
        AttackComboCharge1,
        AttackComboCharge2,
        AttackComboCharge3,
        AttackComboCharge4,
        AttackComboCharge5,

        // ----- ��]�U�� -----
        AttackTurnStart,
        AttackTurn,
        AttackTurnEnd,

        AttackMove0,
        AttackMove1,
        AttackMove2,
        
        FireBreathLeft,
        FireBreathRight,

        Idle2,
        Damage,
        Fly,

        Nova1,
        FlyTurn,
        Nova2,
    };

    enum class PartName { Head, Chest, Body, Leg, Tail, Wings, Max };

public:
    Enemy(const std::string& filename, const float& scaleFactor);
    ~Enemy() override {}

    virtual void Initialize()                       = 0;
    virtual void Finalize()                         = 0;
    virtual void Update(const float& elapsedTime)   = 0;
    virtual void Render(ID3D11PixelShader* psShader = nullptr) = 0;
    virtual void DrawDebug()                        = 0;
    virtual void DebugRender(DebugRenderer* debugRenderer) = 0;

    void Turn(const float& elapsedTime, const DirectX::XMFLOAT3& targetPos);

    [[nodiscard]] const bool SearchPlayer();

    // ---------- BehaviorTree ----------
    void UpdateNode(const float& elapsedTime);
    virtual bool CheckStatusChange() = 0;

    // ---------- �A�j���[�V�����֘A ----------
    void PlayAnimation(const DragonAnimation& index, const bool& loop, const float& speed = 1.0f) { Object::PlayAnimation(static_cast<int>(index), loop, speed); }
    void PlayBlendAnimation(const DragonAnimation& index, const bool& loop, const float& speed = 1.0f, const float& blendAnimationFrame = 0.0f) { Object::PlayBlendAnimation(static_cast<int>(index), loop, speed, blendAnimationFrame); }

    // ---------- �v���C���[�܂ł̋������Z�o ----------
    [[nodiscard]] const float CalcDistanceToPlayer();

    // ---------- �������g����v���C���[�ւ̃x�N�g�� ----------
    [[nodiscard]] const DirectX::XMFLOAT3 CalcDirectionToPlayer();

    // ---------- �_���[�W ----------
    void AddDamage(const float& damage, const int& dataIndex);

public:// --- �擾�E�ݒ� ---
#pragma region [Get, Set] Function
    // ----- BehaviorTree�p -----
    [[nodiscard]] const int GetStep() const { return step_; }
    void SetStep(const int& step) { step_ = step; }

    // ----- �Ђ�ݔ��� -----
    [[nodiscard]] const bool GetIsFlinch() const { return isFlinch_; }
    void SetIsFlinch(const bool& flinch) { isFlinch_ = flinch; }

    // ----- �퓬�J�n�͈� -----
    [[nodiscard]] const float GetBattleRadius() const { return battleRadius_; }
    void SetBattleRadius(const float& radius) { battleRadius_ = radius; }

    // ----- �U���J�n�͈� -----
    [[nodiscard]] const float GetNearAttackRadius() const { return nearAttackRadius_; }
    void SetNearAttackRadius(const float& radius) { nearAttackRadius_ = radius; }

    [[nodiscard]] const float GetComboFlyAttackRadius() const { return comboFlyAttackRadius_; }
    void SetComboFlyAttackRadius(const float& radius) { comboFlyAttackRadius_ = radius; }

    // ----- �ړ���ʒu -----
    [[nodiscard]] const DirectX::XMFLOAT3 GetMovePosition() const { return movePosition_; }
    void SetMovePosition(const DirectX::XMFLOAT3& movePos) { movePosition_ = movePos; }

    // ----- �A���U���� -----
    [[nodiscard]] const int GetAttackComboCount() const { return attackComboCount_; }
    void SetAttackComboCount(const int& count) { attackComboCount_ = count; }
    void AddAttackComboCount() { ++attackComboCount_; }

    // ----- ���s���x -----
    [[nodiscard]] const float GetWalkSpeed() const { return walkSpeed_; }
    void SetWalkSpeed(const float& speed) { walkSpeed_ = speed; }

    // ----- �U�����肪�L���� -----
    [[nodiscard]] const bool GetIsAttackActive() const { return isAttackActive_; }
    void SetIsAttackActive(const bool& flag) { isAttackActive_ = flag; }

    [[nodiscard]] const bool GetIsRoar() const { return isRoar_; }
    void SetIsRoar(const bool& flag) { isRoar_ = flag; }

    // ----- �U���_���[�W -----
    [[nodiscard]] const float GetAttackDamage() const { return attackDamage_; }
    void SetAttackDamage(const float& damage) { attackDamage_ = damage; }

#pragma endregion [Get, Set] Function

    [[nodiscard]] const std::string GetActiveNodeName() const { return (activeNode_ != nullptr) ? activeNode_->GetName() : ""; }

    // ---------- �U������ ----------
    virtual void ResetAllAttackActiveFlag()                             = 0; // �S�U�����薳����
    virtual void SetTurnAttackActiveFlag(const bool& flag = true)       = 0; // ��]�U��
    virtual void SetTackleAttackActiveFlag(const bool& flag = true)     = 0; // �ːi�U��
    virtual void SetFlyAttackActiveFlag(const bool& flag = true)        = 0; // �㏸�U��
    virtual void SetComboSlamAttackActiveFlag(const bool& flag = true)  = 0; // ���������U��
    virtual void SetKnockBackAttackActiveFalg(const bool& flag = true)  = 0; // ������΂��U��

    // ---------- �����o������ ----------
    virtual void SetDownCollisionActiveFlag(const bool& flag = true) = 0;

    [[nodiscard]] const bool GetIsStageCollisionJudgement() const { return isStageCollisionJudgement_; }
    void SetIsStageCollisionJudgement(const bool& flag) { isStageCollisionJudgement_ = flag; }

    // ---------- ���ݔ���p�f�[�^ ----------
    [[nodiscard]] const int GetFlinchDetectionDataCount() const { return flinchDetectionData_.size(); }
    std::vector<AttackDetectionData> GetFlinchDetectionData() { return flinchDetectionData_; }
    AttackDetectionData& GetFlinchDetectionData(const int& index) { return flinchDetectionData_.at(index); }

    [[nodiscard]] const bool GetIsPartDestruction(const PartName& partName) const { return isPartDestruction_[static_cast<int>(partName)]; }

private:
    // ---------- ���ʔj�� ----------
    virtual void AddDamagePart(const float& damage, const int& dataIndex) = 0;
    void CheckPartDestruction(); // ���ʔj�󔻒�


protected:
    std::unique_ptr<BehaviorTree>   behaviorTree_;
    std::unique_ptr<BehaviorData>   behaviorData_;
    NodeBase*                       activeNode_ = nullptr;

    std::vector<AttackDetectionData> flinchDetectionData_; // ���ݔ���p�f�[�^

    int     step_       = 0;        // �s���X�e�b�v
    bool    isFlinch_   = false;    // �Ђ�݃t���O

    // ---------- �퓬����͈͕ϐ� ----------
    float               battleRadius_       = 20.0f;// �퓬�J�n�͈�
    
    float               nearAttackRadius_       = 15.0f; // �ߋ����U���J�n�͈�
    float               comboFlyAttackRadius_   = 8.5f;  // �R�A�����������U��   

    float attackDamage_ = 0.0f; // �U���_���[�W

    DirectX::XMFLOAT3 movePosition_ = {}; // �ړ���ʒu

    int     attackComboCount_ = 0; // �A���U����

    float walkSpeed_ = 0.0f; // ���s���x

    // ---------- �󋵊Ǘ��t���O ----------
    bool isAttackActive_    = false; // �U�����肪�L����
    bool isRoar_            = false; // ���K������

    bool isStageCollisionJudgement_ = false; // �X�e�[�W�Ƃ̔�������邩

    // ---------- ���ʔj�� ----------
    float partHealth_[static_cast<int>(PartName::Max)]        = {}; // ���ʂ��Ƃ̗̑�
    bool  isPartDestruction_[static_cast<int>(PartName::Max)] = {}; // ���ʔj�󔻒�t���O

};

