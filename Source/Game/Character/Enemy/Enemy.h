#pragma once
#include <memory>
#include "../Character.h"
#include "../Graphics/Graphics.h"

#if 1
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorData.h"
#include "BehaviorTree/NodeBase.h"
#else
class BehaviorTree;
class BehaviorData;
class NodeBase;
#endif

class Enemy : public Character
{
public:
    enum class TamamoAnimation
    {
        Idle,       // �ҋ@
        Walk,       // ����
        WalkRight,  // �΂ߕ��� (�E)
        WalkLeft,   // �΂ߕ��� (��)
        Bite,       // ���݂�
        Slash,      // �Ђ�����
        TailSwipe,  // �K��
        Roar,       // ���K
        Filnch,     // �Ђ��
        Slam,       // ��������

#if 0
        Bite,           // ���݂�
        Slash,          // �Ђ�����
        TailSwipe,      // �K��
        SpinAttack,     // ��]
        SpineShot,      // ��
        Tackle,         // �ːi
        Pounce,         // �Ƃт�����
        Roar,           // ���K
        Intimidate,     // �Њd
        Filnch,         // �Ђ��
        Walk,           // ����
        Step,           // �X�e�b�v
        Idle,           // �ҋ@
#endif
    };

public:
    Enemy(std::string filename);
    ~Enemy() override {}

    virtual void Initialize()                       = 0;
    virtual void Finalize()                         = 0;
    virtual void Update(const float& elapsedTime)   = 0;
    virtual void Render()                           = 0;
    virtual void RenderUserInterface()              = 0;
    virtual void DrawDebug()                        = 0;
    virtual void DebugRender(DebugRenderer* debugRenderer) = 0;

    void Turn(const float& elapsedTime, const DirectX::XMFLOAT3& targetPos);

    [[nodiscard]] const bool SearchPlayer();

    // ---------- BehaviorTree ----------
    void UpdateNode(const float& elapsedTime);

    // ---------- �A�j���[�V�����֘A ----------
    void PlayAnimation(const TamamoAnimation& index, const bool& loop, const float& speed = 1.0f) { Object::PlayAnimation(static_cast<int>(index), loop, speed); }
    void PlayBlendAnimation(const TamamoAnimation& index1, const TamamoAnimation& index2, const bool& loop, const float& speed = 1.0f) { Object::PlayBlendAnimation(static_cast<int>(index1), static_cast<int>(index2), loop, speed); }
    bool PlayBlendAnimation(const TamamoAnimation& index, const bool& loop, const float& speed = 1.0f);

    // ---------- �v���C���[�܂ł̋������Z�o ----------
    [[nodiscard]] const float CalcDistanceToPlayer();

    // ---------- �������g����v���C���[�ւ̃x�N�g�� ----------
    [[nodiscard]] const DirectX::XMFLOAT3 CalcDirectionToPlayer();

    // TODO:�����B����ق�܂ɂ����
    bool isWin_ = false;

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

#pragma region ����p �U������ݒ�
    virtual void SetAllAttackFlag(const bool& activeFlag = false) {}
    virtual void SetBiteAttackFlag(const bool& activeFlag = true) {}
    virtual void SetSlashAttackFlag(const bool& activeFlag = true) {}
    virtual void SetTailSwipeAttackFlag(const bool& activeFlag = true) {}
    virtual void SetSlamAttackFlag(const bool& activeFlag = true) {}

    virtual void InitializeStones() {}

#pragma endregion ����p �U������ݒ�

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

