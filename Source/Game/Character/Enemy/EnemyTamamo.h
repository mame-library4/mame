#pragma once
#include "Enemy.h"

class EnemyTamamo : public Enemy
{
public:
    EnemyTamamo();
    ~EnemyTamamo() override;

    void Initialize()                       override;
    void Finalize()                         override;
    void Update(const float& elapsedTime)   override;
    void Render(ID3D11PixelShader* psShader = nullptr) override;
    
    void DrawDebug()                        override;
    void DebugRender(DebugRenderer* debugRenderer);

    void InitializeStones();
    void UpdateStones(const float& elapsedTime);

    void SetTurnAttackActiveFlag(const bool& flag = true) override {}
    void SetTackleAttackActiveFlag(const bool& flag = true) override {}

private:
    void RegisterCollisionData();   // collisionData“o˜^

private:
    bool isCollisionSphere_ = true;
    bool isAttackSphere_ = true;
    bool isDamageSphere_ = true;
};

