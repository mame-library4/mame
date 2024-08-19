#include "ProjectileManager.h"
#include "Common.h"

// ----- ‰Šú‰» -----
void ProjectileManager::Initialize()
{
    for (Projectile*& projectile : projectiles_)
    {
        projectile->Initialize();
    }
}

// ----- I—¹‰» -----
void ProjectileManager::Finalize()
{
    for (Projectile*& projectile : projectiles_)
    {
        projectile->Finalize();
    }
    Clear();
}

// ----- XV -----
void ProjectileManager::Update(const float& elapsedTime)
{
    // -------------------------
    //          ¶¬
    // -------------------------
    for (Projectile* projectile : generates_)
    {
        projectiles_.emplace_back(projectile);
        projectile->Initialize();
    }
    generates_.clear();


    // -------------------------
    //          XV
    // -------------------------
    for (Projectile*& projectile : projectiles_)
    {
        projectile->Update(elapsedTime);
    }

    // -------------------------
    //          ”jŠü
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

// ----- •`‰æ -----
void ProjectileManager::Render(ID3D11PixelShader* psShader)
{
    for (Projectile*& projectile : projectiles_)
    {
        projectile->Render(psShader);
    }
}

// ----- ImGui—p -----
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

void ProjectileManager::DebugRender(DebugRenderer* debugRenderer)
{
    for (Projectile*& projectile : projectiles_)
    {
        debugRenderer->DrawSphere(projectile->GetTransform()->GetPosition(), projectile->GetRadius(), { 1, 0, 0, 1 });
        
        debugRenderer->DrawSphere(projectile->GetTransform()->GetPosition(), projectile->GetCounterRadius(), { 0, 0, 1, 1 });
    }
}

// ----- “o˜^ -----
void ProjectileManager::Register(Projectile* projectile)
{
    generates_.insert(projectile);
}

// ----- íœ -----
void ProjectileManager::Remove(Projectile* projectile)
{
    removes_.insert(projectile);
}

// ----- ‘Síœ -----
void ProjectileManager::Clear()
{
    for (Projectile*& projectile : projectiles_)
    {
        SafeDeletePtr(projectile);
    }
    projectiles_.clear();
    projectiles_.shrink_to_fit();
}

Projectile* ProjectileManager::GetProjectile(const int& id)
{
    for (Projectile*& projectile : projectiles_)
    {
        if (projectile->GetId() == id) return projectile;
    }

    return nullptr;
}
