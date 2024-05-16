#pragma once
#include "Enemy.h"

class EnemyTamamo : public Enemy
{
public:// --- �萔 ---
#pragma region �萔
#if 0
    enum class STATE
    {
        BiteState,          // ���݂�
        SlashState,         // �Ђ�����
        TailSwipeState,     // �K��
        SpinAttackState,    // ��]
        SpineShotState,     // ���U��
        TackleState,        // �ːi
        PounceState,        // ��т�����i�}�P�j
        RoarState,          // ���K
        IntimidataState,    // �Њd
        FilnchState,        // �Ђ��
        WalkState,          // ����
        StepState,          // �X�e�b�v
    };
#endif

#pragma endregion �萔

public:
    EnemyTamamo();
    ~EnemyTamamo() override;

    void Initialize()                       override;
    void Finalize()                         override;
    void Update(const float& elapsedTime)   override;
    void Render()                           override;
    
    void DrawDebug()                        override;
    void DebugRender(DebugRenderer* debugRenderer);

public:// --- �U������ݒ�֐� ---
    void SetAllAttackFlag(const bool& activeFlag = false) override;
    void SetBiteAttackFlag(const bool& activeFlag = true) override;
    void SetSlashAttackFlag(const bool& activeFlag = true) override;
    void SetTailSwipeAttackFlag(const bool& activeFlag = true) override;
    void SetSlamAttackFlag(const bool& activeFlag = true) override;

private:
    void RegisterCollisionData(); // collisionData�o�^

private:
    bool isCollisionSphere_ = true;
    bool isAttackSphere_ = true;
    bool isDamageSphere_ = true;
};

