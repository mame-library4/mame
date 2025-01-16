#include "RainClouds.h"
#include "ProjectileManager.h"
#include "Rain.h"
#include "Character/Player/PlayerManager.h"

// ----- コンストラクタ -----
RainClouds::RainClouds()
    : Projectile("./Resources/Model/Sphere.gltf", 1.0f, "AquaBullet",
        static_cast<int>(ProjectileManager::DrawType::Normal), static_cast<int>(ProjectileManager::AttackType::None))
{
}

// ----- 初期化 -----
void RainClouds::Initialize()
{
    // エフェクト読み込み
    rainEffectEmitter_.SetEmitParameter("Rain");
    rainCloudsEffectEmitter_.SetEmitParameter("RainClouds");
    // 生成位置を算出
    const DirectX::XMFLOAT3 playerPosition = PlayerManager::Instance().GetTransform()->GetPosition();
    const DirectX::XMFLOAT3 playerForward = PlayerManager::Instance().GetTransform()->CalcForward();
    effectEmitPosition_ = playerPosition + XMFloat3Normalize(playerForward) * 5.0f;
    effectEmitPosition_.y = 10.0f;
}

// ----- 終了化 -----
void RainClouds::Finalize()
{
}

// ----- 更新 -----
void RainClouds::Update(const float& elapsedTime)
{
    effectCreateTimer_ += elapsedTime;
    if (effectCreateTimer_ >= effectCreateTime_)
    {
        effectCreateTimer_ = 0.0f;

        // 雨雲エフェクトを生成
        rainCloudsEffectEmitter_.SetEmitPosition(effectEmitPosition_);
        rainCloudsEffectEmitter_.EmitParticle();

        // 雨エフェクトを生成
        rainEffectEmitter_.SetEmitPosition(effectEmitPosition_);
        rainEffectEmitter_.EmitParticle();
    }

    // 雨 (攻撃判定) を生成する
    if (true)
    {
        DirectX::XMFLOAT3 emitPosition = effectEmitPosition_;
        emitPosition.x = emitPosition.x + XMFloatRandomRange(-5.0f, 5.0f);
        emitPosition.y = emitPosition.y;
        emitPosition.z = emitPosition.z + XMFloatRandomRange(-5.0f, 5.0f);

        Rain* rain = new Rain();
        rain->GetTransform()->SetPosition(emitPosition);
        rain->Launch({ 0.0f, -1.0f, 0.0f }, 30.0f);
    }

    // 生存時間管理
    lifeTimer_ -= elapsedTime;
    if (lifeTimer_ <= 0.0f)
    {
        ProjectileManager::Instance().Remove(this);
    }
}

// ----- ImGui用 -----
void RainClouds::DrawDebug()
{
    if (ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {

        ImGui::TreePop();
    }
}
