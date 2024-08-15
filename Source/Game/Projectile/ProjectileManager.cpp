#include "ProjectileManager.h"
#include "Common.h"

// ----- 初期化 -----
void ProjectileManager::Initialize()
{
    for (Projectile*& projectile : projectiles_)
    {
        projectile->Initialize();
    }
}

// ----- 終了化 -----
void ProjectileManager::Finalize()
{
    for (Projectile*& projectile : projectiles_)
    {
        projectile->Finalize();
    }
    Clear();
}

// ----- 更新 -----
void ProjectileManager::Update(const float& elapsedTime)
{
    // -------------------------
    //          生成
    // -------------------------
    for (Projectile* projectile : generates_)
    {
        projectiles_.emplace_back(projectile);
        projectile->Initialize();
    }
    generates_.clear();


    // -------------------------
    //          更新
    // -------------------------
    for (Projectile*& projectile : projectiles_)
    {
        projectile->Update(elapsedTime);
    }

    // -------------------------
    //          破棄
    // -------------------------
    for (Projectile* projectile : removes_)
    {
        auto it = std::find(projectiles_.begin(), projectiles_.end(), projectile);

        if (it != projectiles_.end())
        {
            projectiles_.erase(it);
        }

        SafeDeletePtr(projectile);
    }
    removes_.clear();
}

// ----- 描画 -----
void ProjectileManager::Render(ID3D11PixelShader* psShader)
{
    for (Projectile*& projectile : projectiles_)
    {
        projectile->Render(psShader);
    }
}

// ----- ImGui用 -----
void ProjectileManager::DrawDebug()
{
    if (ImGui::BeginMenu("ProjectileManager"))
    {
        for (Projectile*& projectile : projectiles_)
        {
            projectile->DrawDebug();
        }

        ImGui::EndMenu();
    }
}

// ----- 登録 -----
void ProjectileManager::Register(Projectile* projectile)
{
    generates_.insert(projectile);
}

// ----- 削除 -----
void ProjectileManager::Remove(Projectile* projectile)
{
    removes_.insert(projectile);
}

// ----- 全削除 -----
void ProjectileManager::Clear()
{
    for (Projectile*& projectile : projectiles_)
    {
        SafeDeletePtr(projectile);
    }
    projectiles_.clear();
    projectiles_.shrink_to_fit();
}
