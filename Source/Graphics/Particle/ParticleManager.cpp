#include "ParticleManager.h"
#include "Graphics.h"
#include "Common.h"

// ----- �I���� -----
void ParticleManager::Finalize()
{
    Clear();
}

// ----- �X�V -----
void ParticleManager::Update(const float& elapsedTime)
{
    // ����
    for (ParticleSystem* particleSystem : generates_)
    {
        particleSystems_.emplace_back(particleSystem);
        particleSystem->Initialize(elapsedTime);
    }
    generates_.clear();

    // �X�V
    for (ParticleSystem*& particleSystem : particleSystems_)
    {
        particleSystem->Update(elapsedTime);
    }

    if (particleSystems_.size() == 0) return;

    // �j��
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

// ----- �`�� -----
void ParticleManager::Render()
{
    for (ParticleSystem*& particleSystem : particleSystems_)
    {
        particleSystem->Render();
    }
}

// ----- ImGui�p -----
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

// ----- �o�^ -----
void ParticleManager::Register(ParticleSystem* particleSystem)
{
    generates_.insert(particleSystem);
}

// ----- �폜 -----
void ParticleManager::Remove(ParticleSystem* particleSystem)
{
    removes_.insert(particleSystem);
}

// ----- �S�폜 -----
void ParticleManager::Clear()
{
    for (ParticleSystem*& particleSystem : particleSystems_)
    {
        SafeDeletePtr(particleSystem);
    }
    particleSystems_.clear();
    particleSystems_.shrink_to_fit();
}
