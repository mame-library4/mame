#pragma once
#include "Enemy.h"

class EnemyTamamo : public Enemy
{
public:// --- ’è” ---
#pragma region ’è”
#if 0
    enum class STATE
    {
        BiteState,          // Šš‚İ‚Â‚«
        SlashState,         // ‚Ğ‚Á‚©‚«
        TailSwipeState,     // K”ö
        SpinAttackState,    // ‰ñ“]
        SpineShotState,     // ™UŒ‚
        TackleState,        // “Ëi
        PounceState,        // ”ò‚Ñ‚©‚©‚éi‹}Pj
        RoarState,          // ™ôšK
        IntimidataState,    // ˆĞŠd
        FilnchState,        // ‚Ğ‚é‚İ
        WalkState,          // •à‚«
        StepState,          // ƒXƒeƒbƒv
    };
#endif

#pragma endregion ’è”

public:
    EnemyTamamo();
    ~EnemyTamamo() override;

    void Initialize()                       override;
    void Finalize()                         override;
    void Update(const float& elapsedTime)   override;
    void Render()                           override;
    
    void DrawDebug()                        override;
    void DebugRender(DebugRenderer* debugRenderer);

public:// --- UŒ‚”»’èİ’èŠÖ” ---
    void SetAllAttackFlag(const bool& activeFlag = false) override;
    void SetBiteAttackFlag(const bool& activeFlag = true) override;
    void SetSlashAttackFlag(const bool& activeFlag = true) override;
    void SetTailSwipeAttackFlag(const bool& activeFlag = true) override;
    void SetSlamAttackFlag(const bool& activeFlag = true) override;

private:
    void RegisterCollisionData(); // collisionData“o˜^

private:
    bool isCollisionSphere_ = true;
    bool isAttackSphere_ = true;
    bool isDamageSphere_ = true;
};

