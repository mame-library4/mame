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

private:
    // ----- DebugRenderer•\Ž¦—pƒtƒ‰ƒO -----
    bool isCollisionSphere_ = true;
    bool isAttackSphere_    = true;
    bool isDamageSphere_    = true;
};

