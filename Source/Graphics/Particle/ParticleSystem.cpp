#include "ParticleSystem.h"
#include "ParticleManager.h"

// ----- コンストラクタ -----
ParticleSystem::ParticleSystem(size_t particleCount)
    : maxParticleCount_(particleCount)
{
    // マネージャーに登録する
    ParticleManager::Instance().Register(this);
}