#include "SuperNovaParticle.h"
#include "Graphics.h"
#include "Texture.h"
#include "Misc.h"

// ----- コンストラクタ -----
SuperNovaParticle::SuperNovaParticle()
	: ParticleSystem(1000), lavaCrawlerParticle_(3000)
{
	GetParticleData()->CreateParticleData(sizeof(Particle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/SuperNovaParticlePS.cso",
		"./Resources/Shader/CoreBurstGS.cso", "./Resources/Shader/CoreBurstInitializeCS.cso", "./Resources/Shader/CoreBurstUpdateCS.cso");
	lavaCrawlerParticle_.CreateParticleData(sizeof(Particle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/LavaCrawlerPS.cso",
		"./Resources/Shader/LavaCrawlerGS.cso", "./Resources/Shader/LavaCrawlerInitializeCS.cso", "./Resources/Shader/LavaCrawlerUpdateCS.cso");

	constants_.speed_ = 20.0f;
	constants_.particleSize_ = 0.25f;

	lavaCrawlerParticleConstants_.speed_ = 10.0f;
	lavaCrawlerParticleConstants_.particleSize_ = 0.05f;
}

// ----- 更新 -----
void SuperNovaParticle::Update(const float& elapsedTime)
{
    constants_.time_ += elapsedTime;
    constants_.deltaTime_ = elapsedTime;
    GetParticleData()->Update(static_cast<int>(CSShaderSlot::CoreBurstParticle), static_cast<int>(CBSlot::CoreBurstParticle), &constants_);
	
    lavaCrawlerParticleConstants_.time_ += elapsedTime;
    lavaCrawlerParticleConstants_.deltaTime_ = elapsedTime;
    lavaCrawlerParticle_.Update(static_cast<int>(CSShaderSlot::LavaCrawlerParticle), static_cast<int>(CBSlot::LavaCrawlerParticle), &lavaCrawlerParticleConstants_);
}

// ----- 描画 -----
void SuperNovaParticle::Render()
{	
    GetParticleData()->Render(9, static_cast<int>(CBSlot::CoreBurstParticle), &constants_);
	
    lavaCrawlerParticle_.Render(static_cast<int>(CBSlot::LavaCrawlerParticle), static_cast<int>(CBSlot::LavaCrawlerParticle), &lavaCrawlerParticleConstants_);
}

// ----- ImGui用 -----
void SuperNovaParticle::DrawDebug()
{
	if (ImGui::TreeNode("SuperNovaParticle"))
	{
		if (ImGui::TreeNode("LavaCrawlerParticle"))
		{
			ImGui::DragFloat("Speed", &lavaCrawlerParticleConstants_.speed_);
			ImGui::DragFloat("Size", &lavaCrawlerParticleConstants_.particleSize_);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("CoreBurstParticle"))
		{
			ImGui::DragFloat("Speed", &constants_.speed_);
			ImGui::DragFloat("Size", &constants_.particleSize_);

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

// ----- 再生 -----
void SuperNovaParticle::PlayLavaCrawlerParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition)
{
	lavaCrawlerParticleConstants_.emitterPosition_ = emitterPosition;
	lavaCrawlerParticleConstants_.time_ += elapsedTime;
	lavaCrawlerParticleConstants_.deltaTime_ = elapsedTime;
	lavaCrawlerParticle_.PlayParticle(static_cast<int>(CSShaderSlot::LavaCrawlerParticle), static_cast<int>(CBSlot::LavaCrawlerParticle), &lavaCrawlerParticleConstants_);
}

// ----- 再生 -----
void SuperNovaParticle::PlayCoreBurstParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition)
{
	constants_.emitterPosition_ = emitterPosition;
	constants_.time_ += elapsedTime;
	constants_.deltaTime_ = elapsedTime;
	GetParticleData()->PlayParticle(static_cast<int>(CSShaderSlot::CoreBurstParticle), static_cast<int>(CBSlot::CoreBurstParticle), &constants_);
}
