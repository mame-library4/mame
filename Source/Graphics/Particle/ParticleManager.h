#pragma once
#include "ParticleSystem.h"
#include <vector>
#include <set>

class SuperNovaParticle;

class ParticleManager
{
private:
    ParticleManager() {}
    ~ParticleManager() {}

public:
    static ParticleManager& Instance()
    {
        static ParticleManager instance;
        return instance;
    }

    void Finalize();
    void Update(const float& elapsedTime);
    void Render();
    void DrawDebug();

    void Register(ParticleSystem* particleSystem);
    void Remove(ParticleSystem* particleSystem);
    void Clear();

private:
    std::vector<ParticleSystem*>    particleSystems_;
    std::set<ParticleSystem*>       removes_;
    std::set<ParticleSystem*>       generates_;
};

