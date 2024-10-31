#include "TailParticle.h"
#include "Graphics.h"
#include "Texture.h"
#include "Misc.h"
#include "MathHelper.h"

// ----- コンストラクタ -----
TailParticle::TailParticle()
    : ParticleSystem(1000), tailChargeParticle_(3000)
{	
	GetParticleData()->CreateParticleData(sizeof(Particle), sizeof(TailParticleConstants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso",
		"./Resources/Shader/TailParticleGS.cso", "./Resources/Shader/TailParticleInitializeCS.cso", "./Resources/Shader/TailParticleUpdateCS.cso");

	tailChargeParticle_.CreateParticleData(sizeof(ChargeParticleData), sizeof(TailChargeParticleConstants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso",
		"./Resources/Shader/TailChargeGS.cso", "./Resources/Shader/TailChargeInitializeCS.cso", "./Resources/Shader/TailChargeUpdateCS.cso");
}

void TailParticle::Update(const float& elapsedTime)
{
	GetParticleData()->Update(csUAVSlot_, cbSlot_, &tailParticleConstants_);

    tailChargeParticleConstants_.deltaTime_ = elapsedTime;
    tailChargeParticle_.Update(csUAVSlot_, cbSlot_, &tailChargeParticleConstants_);
}

void TailParticle::Render()
{
	GetParticleData()->Render(gsSRVSlot_, cbSlot_, &tailParticleConstants_);

	tailChargeParticle_.Render(gsSRVSlot_, cbSlot_, &tailChargeParticleConstants_);
}

void TailParticle::DrawDebug()
{
	if (ImGui::BeginMenu("TailParticle"))
	{
		ImGui::DragFloat("ParticleSize", &tailParticleConstants_.particleSize_);
		ImGui::ColorEdit3("ParticleColor", &tailParticleConstants_.particleColor_.x);
		ImGui::DragFloat("Radius", &tailParticleConstants_.radius_, 0.1f, 0.0f, 3.0f);
		ImGui::DragFloat("Height", &tailParticleConstants_.height_, 0.1f, 0.0f, 5.0f);

		if (ImGui::TreeNode("Charge"))
		{
			ImGui::DragFloat("Size", &tailChargeParticleConstants_.particleSize_);
			ImGui::DragFloat("Speed", &tailChargeParticleConstants_.speed_);

			ImGui::TreePop();
		}

		ImGui::EndMenu();
	}

}

// ----- 再生 -----
void TailParticle::PlayTailParticle()
{
	GetParticleData()->PlayParticle(csUAVSlot_, cbSlot_, &tailParticleConstants_);
}

void TailParticle::PlayChargeParticle()
{
	tailChargeParticle_.PlayParticle(csUAVSlot_, cbSlot_, &tailChargeParticleConstants_);
}

// ----- 尻尾の位置更新 -----
void TailParticle::UpdateJointPosition(const std::vector<DirectX::XMFLOAT3>& jointPosition)
{
	if (jointPosition.size() != MaxTailNum_) return;

	for (int i = 0; i < MaxTailNum_; ++i)
	{
		DirectX::XMFLOAT3 tailPosition = jointPosition.at(i);
		tailParticleConstants_.tailPosition_[i] = DirectX::XMFLOAT4(tailPosition.x, tailPosition.y, tailPosition.z, 1.0f);
	}

	DirectX::XMFLOAT3 direction = jointPosition.at(1) - jointPosition.at(0);

	tailParticleConstants_.direction_ = XMFloat3Normalize(direction);

	tailParticleConstants_.height_ = XMFloat3Length(direction);
}

void TailParticle::UpdateJointWorldMatrix(const std::vector<DirectX::XMMATRIX>& jointWorldMatrix)
{
	if (jointWorldMatrix.size() != MaxTailNum_) return;

	for (int i = 0; i < MaxTailNum_; ++i)
	{
		DirectX::XMFLOAT4X4 tailWorld = {};
		DirectX::XMStoreFloat4x4(&tailWorld, jointWorldMatrix.at(i));
		tailParticleConstants_.tailWorld_[i] = tailWorld;
	}
}
