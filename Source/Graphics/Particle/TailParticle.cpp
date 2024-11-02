#include "TailParticle.h"
#include "Graphics.h"
#include "Texture.h"
#include "Misc.h"
#include "MathHelper.h"

// ----- コンストラクタ -----
TailParticle::TailParticle()
    : ParticleSystem(1000), tailChargeParticle_(3000), tailTrailParticle_(3000)
{	
	GetParticleData()->CreateParticleData(sizeof(Particle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso",
		"./Resources/Shader/TailParticleGS.cso", "./Resources/Shader/TailParticleInitializeCS.cso", "./Resources/Shader/TailParticleUpdateCS.cso");

	tailChargeParticle_.CreateParticleData(sizeof(ChargeParticleData), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso",
		"./Resources/Shader/TailChargeGS.cso", "./Resources/Shader/TailChargeInitializeCS.cso", "./Resources/Shader/TailChargeUpdateCS.cso");

	tailTrailParticle_.CreateParticleData(sizeof(TailParticle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso",
		"./Resources/Shader/TailChargeGS.cso", "./Resources/Shader/TailChargeInitializeCS.cso", "./Resources/Shader/TailChargeUpdateCS.cso");
}

void TailParticle::Update(const float& elapsedTime)
{
	constants_.deltaTime_ = elapsedTime;
	
	GetParticleData()->Update(csUAVSlot_, cbSlot_, &constants_);

    tailChargeParticle_.Update(csUAVSlot_, cbSlot_, &constants_);

	tailTrailParticle_.Update(csUAVSlot_, cbSlot_, &constants_);
}

void TailParticle::Render()
{
	GetParticleData()->Render(gsSRVSlot_, cbSlot_, &constants_);

	tailChargeParticle_.Render(gsSRVSlot_, cbSlot_, &constants_);
	
	tailTrailParticle_.Render(gsSRVSlot_, cbSlot_, &constants_);
}

void TailParticle::DrawDebug()
{
	if (ImGui::BeginMenu("TailParticle"))
	{
		ImGui::DragFloat("ParticleSize", &constants_.particleSize_);
		ImGui::ColorEdit3("ParticleColor", &constants_.particleColor_.x);
		ImGui::DragFloat("Radius", &constants_.radius_, 0.1f, 0.0f, 3.0f);
		ImGui::DragFloat("Height", &constants_.height_, 0.1f, 0.0f, 5.0f);

		if (ImGui::TreeNode("Charge"))
		{
			ImGui::DragFloat("Size", &constants_.particleSize_);
			ImGui::DragFloat("Speed", &constants_.speed_);

			ImGui::TreePop();
		}

		ImGui::EndMenu();
	}

}

// ----- 再生 -----
void TailParticle::PlayTailParticle()
{
	GetParticleData()->PlayParticle(csUAVSlot_, cbSlot_, &constants_);
}

void TailParticle::PlayChargeParticle()
{
	tailChargeParticle_.PlayParticle(csUAVSlot_, cbSlot_, &constants_);
}

void TailParticle::PlayTailTrailParticle()
{
	tailTrailParticle_.PlayParticle(csUAVSlot_, cbSlot_, &constants_);
}

// ----- 尻尾の位置更新 -----
void TailParticle::UpdateJointPosition(const std::vector<DirectX::XMFLOAT3>& jointPosition)
{
	if (jointPosition.size() != MaxTailNum_) return;

	for (int i = 0; i < MaxTailNum_; ++i)
	{
		DirectX::XMFLOAT3 tailPosition = jointPosition.at(i);
		constants_.tailPosition_[i] = DirectX::XMFLOAT4(tailPosition.x, tailPosition.y, tailPosition.z, 1.0f);
	}

	DirectX::XMFLOAT3 direction = jointPosition.at(1) - jointPosition.at(0);

	constants_.direction_ = XMFloat3Normalize(direction);

	constants_.height_ = XMFloat3Length(direction);
}

void TailParticle::UpdateJointWorldMatrix(const std::vector<DirectX::XMMATRIX>& jointWorldMatrix)
{
	if (jointWorldMatrix.size() != MaxTailNum_) return;

	for (int i = 0; i < MaxTailNum_; ++i)
	{
		DirectX::XMFLOAT4X4 tailWorld = {};
		DirectX::XMStoreFloat4x4(&tailWorld, jointWorldMatrix.at(i));
		constants_.tailWorld_[i] = tailWorld;
	}
}
