#pragma once
#include "Projectile.h"
#include <vector>
#include <set>
#include "Graphics.h"

class ProjectileManager
{
private:
    ProjectileManager() {}
    ~ProjectileManager() {}

public:
    enum class DrawType
    {
        Normal,
        GBuffer,
    };
    enum class AttackType
    {
        All,
        Player,
        Enemy,
        None,
    };

public:
    static ProjectileManager& Instance()
    {
        static ProjectileManager instance;
        return instance;
    }

    void Initialize();                                  // èâä˙âª
    void Finalize();                                    // èIóπâª
    void Update(const float& elapsedTime);              // çXêV
    void Render(ID3D11PixelShader* psShader = nullptr); // ï`âÊ

    void DrawDebug();                                   // ImGuióp
    void DebugRender(DebugRenderer* debugRenderer);

    void Register(Projectile* projectile);
    void Remove(Projectile* projectile);
    void Clear();

    [[nodiscard]] const int GetProjectileCount() const { return projectiles_.size(); }

    std::vector<Projectile*> GetProjectiles() { return projectiles_; }
    Projectile* GetProjectile(const int& id);

    // ----- IDìoò^óp -----
    [[nodiscard]] const int GetMyID() { return idCounter_++; }

private:
    std::vector<Projectile*>    projectiles_;
    std::set<Projectile*>       generates_;
    std::set<Projectile*>       removes_;

    int idCounter_ = 0;
};

