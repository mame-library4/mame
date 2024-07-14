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
    void RegisterBehaviorNode();    // behavior“o˜^
    void RegisterCollisionData();   // collisionData“o˜^

    // ---------- Collision ----------
    void UpdateCollisions(const float& elapsedTime) override;

    void SetTurnAttackActiveFlag(const bool& flag = true) override;
    void SetTackleAttackActiveFlag(const bool& flag = true) override;

private:
    Object circle_;

    // ----- DebugRenderer•\Ž¦—pƒtƒ‰ƒO -----
    bool isCollisionSphere_ = true;
    bool isAttackSphere_    = true;
    bool isDamageSphere_    = true;

    bool isUpdateAnimation_ = true;
    //bool isUpdateAnimation_ = false;

private:
    enum AttackData
    {
        TrunAttackStart = 0,
        TrunAttackEnd   = 5,
        TackleStart     = 6,
        TackleEnd       = 10,

    };
};

