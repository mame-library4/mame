#pragma once
#include "Enemy.h"
#include "Stone/Stone.h"
#include "sprite.h"

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


private:
    void RegisterBehaviorNode();        // behavior“o˜^
    void RegisterCollisionData();   // collisionData“o˜^

private:
    bool isCollisionSphere_ = true;
    bool isAttackSphere_ = true;
    bool isDamageSphere_ = true;

    static const int maxStoneNum_ = 15;
    std::unique_ptr<Stone> stones_[maxStoneNum_];

    std::unique_ptr<Sprite> hpSprite_;
    std::unique_ptr<Sprite> hpBackSprite_;
};

