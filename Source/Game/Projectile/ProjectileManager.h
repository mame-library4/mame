#pragma once
#include "Projectile.h"
#include <vector>
#include <set>

class ProjectileManager
{
private:
    ProjectileManager() {}
    ~ProjectileManager() {}
    
public:
    static ProjectileManager& Instance()
    {
        static ProjectileManager instance;
        return instance;
    }

    void Initialize();                                  // 初期化
    void Finalize();                                    // 終了化
    void Update(const float& elapsedTime);              // 更新
    void Render(ID3D11PixelShader* psShader = nullptr); // 描画
    void DrawDebug();                                   // ImGui用

    void Register(Projectile* projectile);
    void Remove(Projectile* projectile);
    void Clear();

private:
    std::vector<Projectile*>    projectiles_;
    std::set<Projectile*>       generates_;
    std::set<Projectile*>       removes_;
};

