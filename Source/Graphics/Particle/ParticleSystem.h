#pragma once
#include "ParticleData.h"

// マネージャーに登録用データ
class ParticleSystem
{
public:
    ParticleSystem(size_t particleCount);
    virtual ~ParticleSystem() {}

    virtual void Update(const float& deltaTime)     = 0;
    virtual void Render()                           = 0;
    virtual void DrawDebug()                        = 0;    

    ParticleData* GetParticleData() { return &particleData_; }

private:
    ParticleData particleData_;
};