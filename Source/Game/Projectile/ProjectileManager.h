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

    void Initialize();                                  // ������
    void Finalize();                                    // �I����
    void Update(const float& elapsedTime);              // �X�V
    void Render(ID3D11PixelShader* psShader = nullptr); // �`��
    void DrawDebug();                                   // ImGui�p

    void Register(Projectile* projectile);
    void Remove(Projectile* projectile);
    void Clear();

private:
    std::vector<Projectile*>    projectiles_;
    std::set<Projectile*>       generates_;
    std::set<Projectile*>       removes_;
};

