#include "RockParticle.h"
#include "Graphics.h"
#include "Texture.h"

// ----- コンストラクタ -----
RockParticle::RockParticle()
    : ParticleSystem(3000)
{
    GetParticleData()->CreateParticleData(sizeof(Particle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/RockSmokePS.cso",
        "./Resources/Shader/RockSmokeGS.cso", "./Resources/Shader/RockSmokeInitializeCS.cso", "./Resources/Shader/RockSmokeUpdateCS.cso");

    D3D11_TEXTURE2D_DESC desc = {};
    Texture::Instance().LoadTexture(L"./Resources/Effect/Texture/Smoke.png", shaderResourceView_.GetAddressOf(), &desc);
}

// ----- 更新 -----
void RockParticle::Update(const float& elapsedTime)
{
    constants_.deltaTime_ = elapsedTime;
    GetParticleData()->Update(csUAVSlot_, cbSlot_, &constants_);
}

// ----- 描画 -----
void RockParticle::Render()
{
    Graphics::Instance().GetDeviceContext()->PSSetShaderResources(0, 1, shaderResourceView_.GetAddressOf());

    GetParticleData()->Render(gsSRVSlot_, cbSlot_, &constants_);
}

// ----- ImGui用 -----
void RockParticle::DrawDebug()
{
}

// ----- 再生 -----
void RockParticle::PlaySmokeParticle()
{
    GetParticleData()->PlayParticle(gsSRVSlot_, cbSlot_, &constants_);
}