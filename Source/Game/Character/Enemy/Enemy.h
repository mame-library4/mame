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

        AttackFront,

        BackStepRoar,   // �o�b�N�X�e�b�v��,���K

        MoveRoar, // �ړ����Ȃ�����K

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
        AttackTurn0,
        AttackTurn1,
        AttackTurnEnd,

        AttackMove0,
        AttackMove1,
        AttackMove2,
        AttackMove3,
        AttackMove4,
    };

public:
    Enemy(std::string filename);
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

    // ---------- �A�j���[�V�����֘A ----------


    // ---------- �v���C���[�܂ł̋������Z�o ----------
    [[nodiscard]] const float CalcDistanceToPlayer();

    // ---------- �������g����v���C���[�ւ̃x�N�g�� ----------
    [[nodiscard]] const DirectX::XMFLOAT3 CalcDirectionToPlayer();

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
    [[nodiscard]] const float GetFarAttackRadius() const { return farAttackRadius_; }
    void SetFarAttackRadius(const float& radius) { farAttackRadius_ = radius; }

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

#pragma endregion [Get, Set] Function



protected:
    std::unique_ptr<BehaviorTree>   behaviorTree_;
    std::unique_ptr<BehaviorData>   behaviorData_;
    NodeBase*                       activeNode_ = nullptr;

    int     step_       = 0;        // �s���X�e�b�v
    bool    isFlinch_   = false;    // �Ђ�݃t���O

    // ---------- �퓬����͈͕ϐ� ----------
    float               battleRadius_       = 20.0f;// �퓬�J�n�͈�
    float               nearAttackRadius_   = 6.0f; // �ߋ����U���J�n�͈�
    float               farAttackRadius_    = 14.0f;// �������U���J�n�͈�


    DirectX::XMFLOAT3 movePosition_ = {}; // �ړ���ʒu

    int     attackComboCount_ = 0; // �A���U����

    float walkSpeed_ = 0.0f; // ���s���x
};

