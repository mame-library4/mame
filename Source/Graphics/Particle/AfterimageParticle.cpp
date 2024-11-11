#include "AfterimageParticle.h"
#include "Graphics.h"

// ----- コンストラクタ -----
AfterimageParticle::AfterimageParticle()
    : ParticleSystem(10000)
{
    GetParticleData()->CreateParticleData(sizeof(Particle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso", 
        "./Resources/Shader/AfterimageGS.cso", "./Resources/Shader/AfterimageInitializeCS.cso", "./Resources/Shader/AfterimageUpdateCS.cso");
}

// ----- 更新 -----
void AfterimageParticle::Update(const float& elapsedTime)
{
    constants_.deltaTime_ = elapsedTime;

    GetParticleData()->Update(csUAVSlot_, cbSlot_, &constants_);
}

// ----- 描画 -----
void AfterimageParticle::Render()
{
    GetParticleData()->Render(gsSRVSlot_, cbSlot_, &constants_);
}

// ----- ImGui用 -----
void AfterimageParticle::DrawDebug()
{
    if (ImGui::TreeNode("AfterimageParticle"))
    {


        ImGui::TreePop();
    }
}

// ----- 再生 -----
void AfterimageParticle::Play()
{
    GetParticleData()->PlayParticle(csUAVSlot_, cbSlot_, &constants_);
}

// ----- ジョイント位置更新 -----
void AfterimageParticle::UpdateJointPosition(const std::vector<DirectX::XMFLOAT3>& jointPosition)
{
    if (jointPosition.size() != maxJointNum_) return;

    for (int i = 0; i < maxJointNum_; ++i)
    {
        DirectX::XMFLOAT3 position = jointPosition.at(i);
        constants_.jointPosition_[i] = DirectX::XMFLOAT4(position.x, position.y, position.z, 1.0f);
    }
}
