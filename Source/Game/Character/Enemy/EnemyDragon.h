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

public:
    enum class PartName { Head, Chest, Body, Leg, Tail, Wings, Max };

private:
    // ---------- BehaviorTree ----------
    void RegisterBehaviorNode();    // behaviorìoò^
    bool CheckStatusChange() override;

    // ---------- Collision ----------
    void RegisterCollisionData();   // collisionDataìoò^
    void UpdateCollisions(const float& elapsedTime) override;

    // ---------- çUåÇîªíË ----------
    void ResetAllAttackActiveFlag()                             override;
    void SetTurnAttackActiveFlag(const bool& flag = true)       override;
    void SetTackleAttackActiveFlag(const bool& flag = true)     override;
    void SetFlyAttackActiveFlag(const bool& flag = true)        override;
    void SetComboSlamAttackActiveFlag(const bool& flag = true)  override;
    void SetKnockBackAttackActiveFalg(const bool& flag = true)  override;

    // ---------- âüÇµèoÇµîªíË ----------
    void SetDownCollisionActiveFlag(const bool& flag = true) override;    

    // ---------- É_ÉÅÅ[ÉW ----------
    void AddDamage(const float& damage, const int& dataIndex) override;
    void AddDamagePart(const float& damage, const int& dataIndex);

private:
    Object circle_;

    float partHealth_[static_cast<int>(PartName::Max)]; // ïîà Ç≤Ç∆ÇÃëÃóÕ
    int partIndex_ = 0;


    // ----- DebugRendererï\é¶ópÉtÉâÉO -----
    bool isCollisionSphere_ = true;
    bool isAttackSphere_    = true;
    //bool isAttackSphere_    = false;
    //bool isDamageSphere_    = true;
    bool isDamageSphere_    = false;

    bool isUpdateAnimation_ = false;
    //bool isUpdateAnimation_ = true;
    

    float oldHealth_ = 0.0f;

private:
    enum CollisionData
    {
        DownStart   = 0,
        DownEnd     = 25,
    };
    enum AttackData
    {
        TrunAttackStart         = 0,
        TrunAttackEnd           = 5,
        TackleAttackStart       = 6,
        TackleAttackEnd         = 10,
        FlyAttackStart          = 11,
        FlyAttackEnd            = 12,
        ComboSlamAttackStart    = 13,
        ComboSlamAttackEnd      = 15,
        KnockBackAttackStart    = 16,
        KnockBackAttackEnd      = 29,
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

