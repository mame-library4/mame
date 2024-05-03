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

private:
    DirectX::XMFLOAT3 tailPosition = {};

    bool isCylinder_ = true;
    bool isSphere_ = true;
};

