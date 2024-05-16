#pragma once
#include "Enemy.h"
#include "Stone/Stone.h"

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

    void InitializeStones();
    void UpdateStones(const float& elapsedTime);

public:// --- 攻撃判定設定関数 ---
    void SetAllAttackFlag(const bool& activeFlag = false) override;
    void SetBiteAttackFlag(const bool& activeFlag = true) override;
    void SetSlashAttackFlag(const bool& activeFlag = true) override;
    void SetTailSwipeAttackFlag(const bool& activeFlag = true) override;
    void SetSlamAttackFlag(const bool& activeFlag = true) override;

private:
    void RegisterCollisionData(); // collisionData登録

private:
    bool isCollisionSphere_ = true;
    bool isAttackSphere_ = true;
    bool isDamageSphere_ = true;

    static const int maxStoneNum_ = 15;
    std::unique_ptr<Stone> stones_[maxStoneNum_];
};

