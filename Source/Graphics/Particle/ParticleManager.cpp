#include "ParticleManager.h"
#include "Graphics.h"
#include "Common.h"

// ----- 終了化 -----
void ParticleManager::Finalize()
{
    Clear();
}

// ----- 更新 -----
void ParticleManager::Update(const float& elapsedTime)
{
    // 生成
    for (ParticleSystem* particleSystem : generates_)
    {
        particleSystems_.emplace_back(particleSystem);
        particleSystem->Initialize(elapsedTime);
    }
    generates_.clear();

    // 更新
    for (ParticleSystem*& particleSystem : particleSystems_)
    {
        particleSystem->Update(elapsedTime);
    }

    if (particleSystems_.size() == 0) return;

    // 破棄
    for (ParticleSystem* particleSystem : removes_)
    {
        auto it = std::find(particleSystems_.begin(), particleSystems_.end(), particleSystem);

        if (it != particleSystems_.end())
        {
            particleSystems_.erase(it);
        }

        SafeDeletePtr(particleSystem);
    }
    removes_.clear();
}

// ----- 描画 -----
void ParticleManager::Render()
{
    for (ParticleSystem*& particleSystem : particleSystems_)
    {
        particleSystem->Render();
    }
}

// ----- ImGui用 -----
void ParticleManager::DrawDebug()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("ParticleManager"))
        {
            for (ParticleSystem*& particleSystem : particleSystems_)
            {
                particleSystem->DrawDebug();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

// ----- 登録 -----
void ParticleManager::Register(ParticleSystem* particleSystem)
{
    generates_.insert(particleSystem);
}

// ----- 削除 -----
void ParticleManager::Remove(ParticleSystem* particleSystem)
{
    removes_.insert(particleSystem);
}

// ----- 全削除 -----
void ParticleManager::Clear()
{
    for (ParticleSystem*& particleSystem : particleSystems_)
    {
        SafeDeletePtr(particleSystem);
    }
    particleSystems_.clear();
    particleSystems_.shrink_to_fit();
}
