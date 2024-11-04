#include "SuperNovaParticle.h"
#include "Graphics.h"
#include "Texture.h"
#include "Misc.h"

// ----- コンストラクタ -----
SuperNovaParticle::SuperNovaParticle()
	: ParticleSystem(1000), lavaCrawlerParticle_(3000), chargeParticle_(3000)
{
	GetParticleData()->CreateParticleData(sizeof(Particle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/SuperNovaParticlePS.cso",
		"./Resources/Shader/CoreBurstGS.cso", "./Resources/Shader/CoreBurstInitializeCS.cso", "./Resources/Shader/CoreBurstUpdateCS.cso");
	lavaCrawlerParticle_.CreateParticleData(sizeof(Particle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/LavaCrawlerPS.cso",
		"./Resources/Shader/LavaCrawlerGS.cso", "./Resources/Shader/LavaCrawlerInitializeCS.cso", "./Resources/Shader/LavaCrawlerUpdateCS.cso");
	chargeParticle_.CreateParticleData(sizeof(ChargeParticle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso",
		"./Resources/Shader/SuperNovaChargeGS.cso", "./Resources/Shader/SuperNovaChargeInitializeCS.cso", "./Resources/Shader/SuperNovaChargeUpdateCS.cso");

	constants_.coreBurstParticleSpeed_ = 20.0f;
	constants_.lavaCrawlerParticleSpeed_ = 10.0f;
}

// ----- 更新 -----
void SuperNovaParticle::Update(const float& elapsedTime)
{
    constants_.deltaTime_ = elapsedTime;

    GetParticleData()->Update(csSlot_, cbSlot_, &constants_);
	
    lavaCrawlerParticle_.Update(csSlot_, cbSlot_, &constants_);

	chargeParticle_.Update(csSlot_, cbSlot_, &constants_);
}

// ----- 描画 -----
void SuperNovaParticle::Render()
{	
    GetParticleData()->Render(gsSlot_, cbSlot_, &constants_);
	
    lavaCrawlerParticle_.Render(gsSlot_, cbSlot_, &constants_);
    
	chargeParticle_.Render(gsSlot_, cbSlot_, &constants_);
}

// ----- ImGui用 -----
void SuperNovaParticle::DrawDebug()
{
	if (ImGui::TreeNode("SuperNovaParticle"))
	{
		ImGui::DragFloat3("Center", &constants_.chargeParticleCenter_.x);
		ImGui::DragFloat("Radius", &constants_.radius_);
		ImGui::DragFloat3("RotationAxis", &constants_.rotationAxis_.x);
		ImGui::DragFloat("RotationSpeed", &constants_.rotationSpeed_);

        ImGui::DragFloat("CoreBurstParticleSpeed", &constants_.coreBurstParticleSpeed_);
        ImGui::DragFloat("LavaCrawlerParticleSpeed", &constants_.lavaCrawlerParticleSpeed_);

		ImGui::TreePop();
	}
}

// ----- 再生 -----
void SuperNovaParticle::PlayLavaCrawlerParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition)
{
	constants_.emitterPosition_ = emitterPosition;
	constants_.deltaTime_ = elapsedTime;
	lavaCrawlerParticle_.PlayParticle(csSlot_, cbSlot_, &constants_);
}

// ----- 再生 -----
void SuperNovaParticle::PlayCoreBurstParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition)
{
	constants_.emitterPosition_ = emitterPosition;
	constants_.deltaTime_ = elapsedTime;
	GetParticleData()->PlayParticle(csSlot_, cbSlot_, &constants_);
}

void SuperNovaParticle::PlayChargeParticle(const DirectX::XMFLOAT3& emitterPosition)
{
	constants_.emitterPosition_ = emitterPosition;
	chargeParticle_.PlayParticle(csSlot_, cbSlot_, &constants_);
}
