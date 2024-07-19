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
    void RegisterBehaviorNode();    // behavior�o�^
    bool CheckStatusChange() override;

    // ---------- Collision ----------
    void RegisterCollisionData();   // collisionData�o�^
    void UpdateCollisions(const float& elapsedTime) override;

    // ---------- �U������ ----------
    void SetTurnAttackActiveFlag(const bool& flag = true)   override;
    void SetTackleAttackActiveFlag(const bool& flag = true) override;
    void SetFlyAttackActiveFlag(const bool& flag = true)    override;

private:
    Object circle_;

    // ----- DebugRenderer�\���p�t���O -----
    bool isCollisionSphere_ = true;
    bool isAttackSphere_    = true;
    bool isDamageSphere_    = true;

    bool isUpdateAnimation_ = true;
    //bool isUpdateAnimation_ = false;

    float oldHealth_ = 0.0f;

private:
    enum AttackData
    {
        TrunAttackStart     = 0,
        TrunAttackEnd       = 5,
        TackleAttackStart   = 6,
        TackleAttackEnd     = 10,
        FlyAttackStart      = 11,
        FlyAttackEnd        = 12,
    };
};

