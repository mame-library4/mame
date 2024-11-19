#include "SuperNovaParticle.h"
#include "Graphics.h"
#include "Texture.h"
#include "Misc.h"
#include "ParticleManager.h"

// ----- コンストラクタ -----
SuperNovaParticle::SuperNovaParticle()
	: ParticleSystem(10000), lavaCrawlerParticle_(10000), chargeParticle_(10000)
{
	GetParticleData()->CreateParticleData(sizeof(Particle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/SuperNovaParticlePS.cso",
		"./Resources/Shader/CoreBurstGS.cso", "./Resources/Shader/CoreBurstInitializeCS.cso", "./Resources/Shader/CoreBurstUpdateCS.cso");
	lavaCrawlerParticle_.CreateParticleData(sizeof(Particle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/LavaCrawlerPS.cso",
		"./Resources/Shader/LavaCrawlerGS.cso", "./Resources/Shader/LavaCrawlerInitializeCS.cso", "./Resources/Shader/LavaCrawlerUpdateCS.cso");
	chargeParticle_.CreateParticleData(sizeof(ChargeParticle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso",
		"./Resources/Shader/SuperNovaChargeGS.cso", "./Resources/Shader/SuperNovaChargeInitializeCS.cso", "./Resources/Shader/SuperNovaChargeUpdateCS.cso");
}

// ----- 更新 -----
void SuperNovaParticle::Update(const float& elapsedTime)
{
	lifeTimer_ -= elapsedTime;

	if (lifeTimer_ <= 0.0f)
	{
		ParticleManager::Instance().Remove(this);
		return;
	}

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
		ImGui::DragFloat("Lifetimer", &lifeTimer_);

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
	constants_.state_ = 1;
	GetParticleData()->PlayParticle(csSlot_, cbSlot_, &constants_);
}

void SuperNovaParticle::PlayChargeParticle(const DirectX::XMFLOAT3& emitterPosition)
{
	constants_.emitterPosition_ = emitterPosition;
	chargeParticle_.PlayParticle(csSlot_, cbSlot_, &constants_);
}

// ----- 削除命令 -----
void SuperNovaParticle::Remove(const float& lifeTime, const float& speed)
{
	isRemove_ = true;

	lifeTimer_ = lifeTime;

	constants_.fadeOutSpeed_ = speed;

	constants_.removeState_ = 1;
}
