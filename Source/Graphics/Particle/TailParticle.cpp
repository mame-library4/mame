#include "TailParticle.h"
#include "ParticleManager.h"
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

    tailTrailParticle_.CreateParticleData(sizeof(TrailParticle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso",
        "./Resources/Shader/TailTrailGS.cso", "./Resources/Shader/TailTrailInitializeCS.cso", "./Resources/Shader/TailTrailUpdateCS.cso");
}

void TailParticle::Update(const float& elapsedTime)
{
    lifeTimer_ -= elapsedTime;

    if (lifeTimer_ <= 0.0f)
    {
        ParticleManager::Instance().Remove(this);
        return;
    }

    // 尻尾のパーティクルが有効な場合
    if (GetParticleData()->GetIsActive())
    {
        const float speed = 3.5f;
        if (state_ == 0)
        {
            lerpTimer_ += speed * elapsedTime;
            lerpTimer_ = min(lerpTimer_, 1.0f);

            const float radius = XMFloatLerp(0.0f, 0.7f, lerpTimer_);

            constants_.radius_.x = radius;
            if (lerpTimer_ == 1.0f)
            {
                lerpTimer_ = 0.0f;
                state_ = 1;
            }
        }
        else if (state_ == 1)
        {
            lerpTimer_ += speed * elapsedTime;
            lerpTimer_ = min(lerpTimer_, 1.0f);

            const float radius = XMFloatLerp(0.0f, 0.65f, lerpTimer_);

            constants_.radius_.y = radius;
            if (lerpTimer_ == 1.0f)
            {
                lerpTimer_ = 0.0f;
                state_ = 2;
            }
        }
        else if (state_ == 2)
        {
            lerpTimer_ += speed * elapsedTime;
            lerpTimer_ = min(lerpTimer_, 1.0f);

            const float radius = XMFloatLerp(0.0f, 0.55f, lerpTimer_);

            constants_.radius_.z = radius;
            if (lerpTimer_ == 1.0f)
            {
                lerpTimer_ = 0.0f;
                state_ = 3;
            }
        }
        else
        {
            lerpTimer_ += speed * elapsedTime;
            lerpTimer_ = min(lerpTimer_, 1.0f);

            const float radius = XMFloatLerp(0.0f, 0.45f, lerpTimer_);

            constants_.radius_.w = radius;
        }
    }


    constants_.deltaTime_ = elapsedTime;
    constants_.tailTrailTimer_ += elapsedTime;

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
        ImGui::DragFloat4("Radius", &constants_.radius_.x, 0.01f, 0.0f, 1.0f);

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
    constants_.tailTrailTimer_ = 0.0f;
    tailTrailParticle_.PlayParticle(csUAVSlot_, cbSlot_, &constants_);
}

// ----- 尻尾の位置更新 -----
void TailParticle::UpdateJointPosition(const std::vector<DirectX::XMFLOAT3>& jointPosition)
{
    if (jointPosition.size() != maxJointNum_) return;

    for (int i = 0; i < maxJointNum_; ++i)
    {
        DirectX::XMFLOAT3 tailPosition = jointPosition.at(i);
        constants_.jointPosition_[i] = DirectX::XMFLOAT4(tailPosition.x, tailPosition.y, tailPosition.z, 1.0f);
    }

    float heignt[4] = {};
    for (int i = 0; i < 4; ++i)
    {
        DirectX::XMFLOAT3 vec = XMFloat3Normalize(jointPosition.at(i + 2) - jointPosition.at(i + 1));
        DirectX::XMFLOAT4 direction = { vec.x, vec.y, vec.z, 1.0f };
        constants_.direction_[i] = direction;
        heignt[i] = XMFloat3Length(vec);
    }
    constants_.height_ = DirectX::XMFLOAT4(heignt[0], heignt[1], heignt[2], heignt[3]);
}

void TailParticle::Remove()
{
    lifeTimer_ = 1.0f;
    constants_.tailParticleState_ = 1;
}
