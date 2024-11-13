#include "SlamAttackParticle.h"
#include "ParticleManager.h"
#include "Graphics.h"
#include "Easing.h"

// ----- コンストラクタ -----
SlamAttackParticle::SlamAttackParticle()
    : ParticleSystem(2000), chargeParticleData_(500), explosionParticle_(1000)
{
    chargeParticleData_.CreateParticleData(sizeof(ChargeParticle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso",
        "./Resources/Shader/SlamChargeGS.cso", "./Resources/Shader/SlamChargeInitializeCS.cso", "./Resources/Shader/SlamChargeUpdateCS.cso");

    explosionParticle_.CreateParticleData(sizeof(ExplosionParticle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso",
        "./Resources/Shader/SlamExplosionGS.cso", "./Resources/Shader/SlamExplosionInitializeCS.cso", "./Resources/Shader/SlamExplosionUpdateCS.cso");

    pointLights_.position_ = {};
    pointLights_.color_ = { 1.0f, 0.9f, 0.0f, 1.0f };
    pointLights_.range_ = 5.0f;
    pointLights_.intensity_ = 0;
}

// ----- 更新 -----
void SlamAttackParticle::Update(const float& elapsedTime)
{
    lifeTimer_ -= elapsedTime;

    if (lifeTimer_ <= 0.0f)
    {
        ParticleManager::Instance().Remove(this);
        return;
    }

    constants_.deltaTime_ = elapsedTime;

    const float totalFrame = 0.3f;
    easingTimer_ += elapsedTime;
    easingTimer_ = min(easingTimer_, totalFrame);
    const float speed = Easing::OutCirc(easingTimer_, totalFrame, 5.0f, 20.0f);

    constants_.speed_ = speed;


    chargeParticleData_.Update(csSlot_, cbSlot_, &constants_);

    explosionParticle_.Update(csSlot_, cbSlot_, &constants_);

    // ポイントライト設定
    if (isRemove_ == false && chargeParticleData_.GetIsActive())
    {
        pointLights_.intensity_ += 30 * elapsedTime;
        pointLights_.intensity_ = min(pointLights_.intensity_, 15.0f);
    }
    Graphics::Instance().SetPointLights(0, pointLights_);
}

// ----- 描画 -----
void SlamAttackParticle::Render()
{
    chargeParticleData_.Render(gsSlot_, cbSlot_, &constants_);
    
    explosionParticle_.Render(gsSlot_, cbSlot_, &constants_);
}

// ----- ImGui用 -----
void SlamAttackParticle::DrawDebug()
{
    ImGui::DragFloat("Speed", &constants_.speed_);
}

// ----- 削除 -----
void SlamAttackParticle::Remove()
{
    // 削除命令が出たので、削除する準備をする
    isRemove_ = true;

    pointLights_.intensity_ = 0.0f;
}

// ----- 再生 -----
void SlamAttackParticle::PlayChargeParticle(const DirectX::XMFLOAT3& handPosition)
{
    constants_.handPosition_ = handPosition;
    chargeParticleData_.PlayParticle(csSlot_, cbSlot_, &constants_);
}

void SlamAttackParticle::PlayExplosionParticle(const DirectX::XMFLOAT3& emitterPosition)
{
    constants_.emitterPosition_ = emitterPosition;
    explosionParticle_.PlayParticle(csSlot_, cbSlot_, &constants_);
}

void SlamAttackParticle::UpdateHandPosition(const DirectX::XMFLOAT3& handPosition)
{
    constants_.handPosition_ = handPosition;

    pointLights_.position_ = DirectX::XMFLOAT4(handPosition.x, handPosition.y, handPosition.z, 1.0f);
}
