#include "MeteorParticle.h"
#include "Graphics.h"
#include "MathHelper.h"

// ----- コンストラクタ -----
MeteorParticle::MeteorParticle()
    : ParticleSystem(3000)
{
    // ポイントライト
    pointLights_.position_ = { 0.0f, 1.0f, 0.0f, 1.0f };
    pointLights_.color_ = { 1.0f, 0.0f, 0.0f, 1.0f };
    pointLights_.range_ = 20.0f;
    pointLights_.intensity_ = 0.0f;
}

// ----- 更新 -----
void MeteorParticle::Update(const float& elapsedTime)
{
    if (isUpdatePointLight_)
    {
        lerpTimer_ += elapsedTime;
        lerpTimer_ = min(lerpTimer_, 1.0f);

        pointLights_.intensity_ = XMFloatLerp(0.0f, maxIntensity_, lerpTimer_);

        Graphics::Instance().SetPointLights(0, pointLights_);
    }
}

// ----- 描画 -----
void MeteorParticle::Render()
{
}

// ----- ImGui用 -----
void MeteorParticle::DrawDebug()
{
}

// ----- 地面のエフェクトを再生 -----
void MeteorParticle::PlayMeteorGlowEffect(const DirectX::XMFLOAT3& position)
{
    targetPosition_ = position;

    pointLights_.position_ = DirectX::XMFLOAT4(position.x, 1.0f, position.z, 1.0f);

    isUpdatePointLight_ = true;
}
