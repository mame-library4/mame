#include "TailParticle.h"
#include "ParticleManager.h"
#include "Graphics.h"
#include "Texture.h"
#include "Misc.h"
#include "MathHelper.h"
#include "AudioManager.h"

// ----- コンストラクタ -----
TailParticle::TailParticle()
    : ParticleSystem(6000), tailTrailParticle_(3000)
{
    GetParticleData()->CreateParticleData(sizeof(Particle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso",
        "./Resources/Shader/TailParticleGS.cso", "./Resources/Shader/TailParticleInitializeCS.cso", "./Resources/Shader/TailParticleUpdateCS.cso");

    tailTrailParticle_.CreateParticleData(sizeof(TrailParticle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/ParticlePS.cso",
        "./Resources/Shader/TailTrailGS.cso", "./Resources/Shader/TailTrailInitializeCS.cso", "./Resources/Shader/TailTrailUpdateCS.cso");

    for (int i = 0; i < maxPointLights_; ++i)
    {
        pointLights_[i].position_   = {};
        pointLights_[i].color_      = { 1.0f, 0.9f, 0.0f, 1.0f };
        pointLights_[i].range_      = 5.0f;
        pointLights_[i].intensity_  = 0;
    }
}

void TailParticle::Update(const float& elapsedTime)
{
    lifeTimer_ -= elapsedTime;

    if (lifeTimer_ <= 0.0f)
    {
        ParticleManager::Instance().Remove(this);
        return;
    }

    // 尻尾のパーティクル半径更新
    UpdateTailRadius(elapsedTime);

    constants_.deltaTime_ = elapsedTime;
    constants_.tailTrailTimer_ += elapsedTime;

    GetParticleData()->Update(csUAVSlot_, cbSlot_, &constants_);
    tailTrailParticle_.Update(csUAVSlot_, cbSlot_, &constants_);

    // 消去準備
    if (isRemove_)
    {
        for (int i = 0; i < maxPointLights_; ++i)
        {
            pointLights_[i].intensity_ -= fadeOutSpeed_ * elapsedTime;
            //pointLights_[i].intensity_ -= 60 * elapsedTime;
            pointLights_[i].intensity_ = max(pointLights_[i].intensity_, 0.0f);
        }
    }

    // ポイントライト位置更新 & 設定
    for (int i = 0; i < maxPointLights_; ++i)
    {
        pointLights_[i].position_ = constants_.jointPosition_[i + 2];

        Graphics::Instance().SetPointLights(i, pointLights_[i]);
    }
}

void TailParticle::Render()
{
    GetParticleData()->Render(gsSRVSlot_, cbSlot_, &constants_);

    tailTrailParticle_.Render(gsSRVSlot_, cbSlot_, &constants_);
}

void TailParticle::DrawDebug()
{
    if (ImGui::TreeNode("TailParticle"))
    {
        ImGui::DragFloat("LifeTimer", &lifeTimer_);

        ImGui::DragFloat4("Radius", &constants_.radius_.x, 0.01f, 0.0f, 1.0f);

        ImGui::Text("---------- PointLights ----------");
        ImGui::DragFloat("FadeOutSpeed", &fadeOutSpeed_);
        ImGui::DragFloat("MaxIntensity", &maxIntensity_);

        ImGui::TreePop();
    }
}

// ----- 再生 -----
void TailParticle::PlayTailParticle()
{
    GetParticleData()->PlayParticle(csUAVSlot_, cbSlot_, &constants_);
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
    isRemove_ = true;

    lifeTimer_ = 1.0f;
    constants_.tailParticleState_ = 1;
}

// ----- 尻尾のパーティクル半径更新 -----
void TailParticle::UpdateTailRadius(const float& elapsedTime)
{
    // 消去準備しているのでここは通らない
    if (isRemove_) return;

    // 尻尾のパーティクルが有効ではない
    if (GetParticleData()->GetIsActive() == false) return;

    if (state_ >= 4) return;

    const float maxRadius[4] = { 0.7f, 0.65f, 0.6f, 0.55f };
    const float speed = 3.5f;

    // チャージSEを再生する
    if (lerpTimer_ == 0.0f)
    {
        AudioManager::Instance().PlaySE(SE::TailCharge);
    }

    lerpTimer_ += speed * elapsedTime;
    lerpTimer_ = min(lerpTimer_, 1.0f);

    const float radius = XMFloatLerp(0.0f, maxRadius[state_], lerpTimer_);

    if (state_ == 0) constants_.radius_.x = radius;
    else if (state_ == 1)constants_.radius_.y = radius;
    else if (state_ == 2)constants_.radius_.z = radius;
    else if (state_ == 3)constants_.radius_.w = radius;    

    // ポイントライト設定
    pointLights_[state_].intensity_ = XMFloatLerp(0.0f, maxIntensity_, lerpTimer_);
    //pointLights_[state_].intensity_ = XMFloatLerp(0.0f, 30.0f, lerpTimer_);

    if (lerpTimer_ == 1.0f && state_ < 4)
    {
        lerpTimer_ = 0.0f;
        ++state_; // 次に進む
    }
}
