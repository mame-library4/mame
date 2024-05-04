#pragma once
#include "Enemy.h"

class EnemyTamamo : public Enemy
{
public:// --- 定数 ---
#pragma region 定数
#if 0
    enum class STATE
    {
        BiteState,          // 噛みつき
        SlashState,         // ひっかき
        TailSwipeState,     // 尻尾
        SpinAttackState,    // 回転
        SpineShotState,     // 棘攻撃
        TackleState,        // 突進
        PounceState,        // 飛びかかる（急襲）
        RoarState,          // 咆哮
        IntimidataState,    // 威嚇
        FilnchState,        // ひるみ
        WalkState,          // 歩き
        StepState,          // ステップ
    };
#endif

#pragma endregion 定数

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
    void RegisterCollisionData(); // collisionData登録

private:
    bool isCylinder_ = true;
    bool isSphere_ = true;
};

