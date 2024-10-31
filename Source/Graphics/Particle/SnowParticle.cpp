#include "SnowParticle.h"
#include "Graphics.h"
#include "Misc.h"
#include "Texture.h"

// ----- コンストラクタ -----
SnowParticle::SnowParticle()
    : ParticleSystem(1000)
{
    GetParticleData()->CreateParticleData(sizeof(Particle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso",
        "./Resources/Shader/ParticleGS.cso", "./Resources/Shader/SnowParticleInitCS.cso", "./Resources/Shader/SnowParticleCS.cso");
}

// ----- 更新 -----
void SnowParticle::Update(const float& deltaTime)
{
    constants_.deltaTime_ = deltaTime;
    GetParticleData()->Update(9, 9, &constants_);
}

// ----- 描画 -----
void SnowParticle::Render()
{
    GetParticleData()->Render(9, 9, &constants_);
}

void SnowParticle::DrawDebug()
{
    if (ImGui::TreeNode("SnowParticle"))
    {
        ImGui::ColorEdit4("Color", &constants_.color_.x);

        ImGui::TreePop();
    }
}

void SnowParticle::Play(const float& elapsedTime)
{
    constants_.deltaTime_ = elapsedTime;
    GetParticleData()->PlayParticle(0, 9, &constants_);
}
