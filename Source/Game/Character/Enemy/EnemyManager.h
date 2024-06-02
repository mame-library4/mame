#pragma once
#include "Enemy.h"
#include <vector>
#include <set>
#include "../Graphics/Graphics.h"

class EnemyManager
{
private:
    EnemyManager() {}
    ~EnemyManager() {}

public:
    static EnemyManager& Instance()
    {
        static EnemyManager instance;
        return instance;
    }

    void Initialize();
    void Finalize();
    void Update(const float& elapsedTime);
    void Render(ID3D11PixelShader* psShader = nullptr);
    void RenderUserInterface();
    void DrawDebug();
    void DebugRender(DebugRenderer* debugRenderer);

    void Register(Enemy* enemy);    // “o˜^
    void Remove(Enemy* enemy);      // íœ
    void Clear();                   // ‘Síœ

    const int GetEnemyCount() const { return enemies_.size(); }
    Enemy* GetEnemy(const int& index) { return enemies_.at(index); }

private:
    std::vector<Enemy*> enemies_;
    std::set<Enemy*>    removes_;
    std::set<Enemy*>     generates_;
};

