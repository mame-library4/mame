#pragma once
#include "ParticleSystem.h"
#include "Shader.h"

class MeteorParticle : public ParticleSystem
{
public:
    MeteorParticle();
    ~MeteorParticle() override {}

    void Update(const float& elapsedTime)     override;
    void Render()                             override;
    void DrawDebug()                          override;

    void PlayMeteorGlowEffect(const DirectX::XMFLOAT3& position);

    [[nodiscard]] const bool GetIsPlayMeteorGlowEffect() const { return isUpdatePointLight_; }

private:
    DirectX::XMFLOAT3 targetPosition_ = {};

    // ----- ポイントライト -----
    Shader::PointLights pointLights_;
    float maxIntensity_ = 20.0f;
    float lerpTimer_ = 0.0f;
    bool isUpdatePointLight_ = false;
};

