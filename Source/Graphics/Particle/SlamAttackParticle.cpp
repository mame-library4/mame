#include "SlamAttackParticle.h"
#include "Graphics.h"
#include "Easing.h"

// ----- コンストラクタ -----
SlamAttackParticle::SlamAttackParticle()
    : ParticleSystem(2000), chargeParticleData_(500)
{
    chargeParticleData_.CreateParticleData(sizeof(ChargeParticle), sizeof(ChargeParticleConstants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso",
        "./Resources/Shader/SlamChargeGS.cso", "./Resources/Shader/SlamChargeInitializeCS.cso", "./Resources/Shader/SlamChargeUpdateCS.cso");
}

// ----- 更新 -----
void SlamAttackParticle::Update(const float& elapsedTime)
{
    chargeParticleConstants_.deltaTime_ = elapsedTime;

    const float totalFrame = 0.3f;
    easingTimer_ += elapsedTime;
    easingTimer_ = min(easingTimer_, totalFrame);
    const float speed = Easing::OutCirc(easingTimer_, totalFrame, 5.0f, 20.0f);

    chargeParticleConstants_.speed_ = speed;


    chargeParticleData_.Update(csSlot_, cbSlot_, &chargeParticleConstants_);
}

// ----- 描画 -----
void SlamAttackParticle::Render()
{
    chargeParticleData_.Render(gsSlot_, cbSlot_, &chargeParticleConstants_);
}

// ----- ImGui用 -----
void SlamAttackParticle::DrawDebug()
{
    ImGui::DragFloat("Speed", &chargeParticleConstants_.speed_);
}

// ----- 再生 -----
void SlamAttackParticle::PlayChargeParticle(const DirectX::XMFLOAT3& handPosition)
{
    chargeParticleConstants_.handPosition_ = handPosition;
    chargeParticleData_.PlayParticle(csSlot_, cbSlot_, &chargeParticleConstants_);
}

void SlamAttackParticle::UpdateHandPosition(const DirectX::XMFLOAT3& handPosition)
{
    chargeParticleConstants_.handPosition_ = handPosition;
}
