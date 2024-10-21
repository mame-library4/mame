#pragma once
#include "Enemy.h"

class EnemyDragon : public Enemy
{
public:
    EnemyDragon();
    ~EnemyDragon() override {};

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override;
    void DrawDebug()                                    override;
    void DebugRender(DebugRenderer* debugRenderer);

private:
    // ---------- BehaviorTree ----------
    void RegisterBehaviorNode();    // behaviorìoò^
    bool CheckStatusChange() override;

    // ---------- Collision ----------
    void RegisterCollisionData();   // collisionDataìoò^
    void UpdateCollisions(const float& elapsedTime) override;

    // ---------- çUåÇîªíË ----------
    void ResetAllAttackActiveFlag()                             override;
    void SetSlamAttackActiveFlag(const bool& flag = true)       override;
    void SetTurnAttackActiveFlag(const bool& flag = true)       override;
    void SetTackleAttackActiveFlag(const bool& flag = true)     override;
    void SetFlyAttackActiveFlag(const bool& flag = true)        override;
    void SetComboSlamAttackActiveFlag(const bool& flag = true)  override;
    void SetKnockBackAttackActiveFalg(const bool& flag = true)  override;

    // ---------- âüÇµèoÇµîªíË ----------
    void SetDownCollisionActiveFlag(const bool& flag = true) override;    

    // ---------- ïîà Ç≤Ç∆ÇÃÉ_ÉÅÅ[ÉWèàóù ----------
    void AddDamagePart(const float& damage, const int& dataIndex) override;

private:
    Object circle_;

    // ----- DebugRendererï\é¶ópÉtÉâÉO -----
    bool isCollisionSphere_ = true;
    bool isAttackSphere_    = true;
    //bool isAttackSphere_    = false;
    //bool isDamageSphere_    = true;
    bool isDamageSphere_    = false;

    //bool isUpdateAnimation_ = false;
    bool isUpdateAnimation_ = true;
    

    float oldHealth_ = 0.0f;

public:
    enum CollisionData
    {
        DownStart   = 0,
        DownEnd     = 25,
    };
    enum AttackData
    {
        SlamAttackStart         = 0,
        SlamAttackEnd           = 1,
        TrunAttackStart         = 2,
        TrunAttackEnd           = 7,
        TackleAttackStart       = 8,
        TackleAttackEnd         = 12,
        FlyAttackStart          = 13,
        FlyAttackEnd            = 14,
        ComboSlamAttackStart    = 15,
        ComboSlamAttackEnd      = 17,
        KnockBackAttackStart    = 18,
        KnockBackAttackEnd      = 31,
    };
    enum class DamageData
    {
        Head    = 0,  // ì™
        Chest   = 1,  // ãπ
        Body    = 2,  // ëÃ
        FrontLeg     = 3,  // ëOãr
        FrontLegEnd  = 6,  // ëOãrÇÃç≈å„
        BackLeg      = 7,  // å„ÇÎãr
        BackLegEnd   = 16, // å„ÇÎãrÇÃç≈å„
        Tail = 17,
        TailEnd = 21,
        Wings = 22,
        WingsEnd = 47,
    };
};

