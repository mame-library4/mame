#include "ParticleSystem.h"
#include "ParticleManager.h"

// ----- �R���X�g���N�^ -----
ParticleSystem::ParticleSystem(size_t particleCount)
    : maxParticleCount_(particleCount)
{
    // �}�l�[�W���[�ɓo�^����
    ParticleManager::Instance().Register(this);
}