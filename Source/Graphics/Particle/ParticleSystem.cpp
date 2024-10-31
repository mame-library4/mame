#include "ParticleSystem.h"
#include "ParticleManager.h"

// ----- コンストラクタ -----
ParticleSystem::ParticleSystem(size_t particleCount)
    : particleData_(particleCount)
{
    // マネージャーに登録する
    ParticleManager::Instance().Register(this);
}