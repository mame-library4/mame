#include "HailBolt.h"
#include "MathHelper.h"
#include "ProjectileManager.h"
#include "ComputeParticle/ComputeParticleEmitter.h"

// ----- コンストラクタ -----
HailBolt::HailBolt()
    : Projectile("./Resources/Model/Sphere.gltf", 1.0f, "HailBolt")
{
}

// ----- 初期化 ----
void HailBolt::Initialize()
{
    GetTransform()->SetScaleFactor(0.1f);

    lifeTimer_ = 10.0f;
}

// ----- 終了化 -----
void HailBolt::Finalize()
{
}

// ----- 更新 -----
void HailBolt::Update(const float& elapsedTime)
{
    // 座標更新
    GetTransform()->AddPosition(moveDirection_ * moveSpeed_ * elapsedTime);

    // エフェクト生成
    if (isEffectGeneratable_)
    {
        ComputeParticleEmitter computeParticleEmitter = {};
        computeParticleEmitter.SetEmitPosition(GetTransform()->GetPosition());
        computeParticleEmitter.EmitParticle("HailBoltTrail");
    }

    lifeTimer_ -= elapsedTime;
    if (lifeTimer_ <= 0.0f)
    {
        ProjectileManager::Instance().Remove(this);
    }
}

// ----- ImGui用 -----
void HailBolt::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        Projectile::DrawDebug();

        ImGui::DragFloat3("MoveDirection", &moveDirection_.x, 0.01f);
        ImGui::DragFloat("MoveSpeed", &moveSpeed_, 0.01f);
        ImGui::Checkbox("IsEffectGeneratable", &isEffectGeneratable_);

        ImGui::TreePop();
    }
}

void HailBolt::OnHit()
{
}

// ----- 発射 -----
void HailBolt::Launch(const DirectX::XMFLOAT3& emitPosition, const DirectX::XMFLOAT3& direction, const float& speed)
{
    GetTransform()->SetPosition(emitPosition);

    moveDirection_ = direction;
    moveSpeed_ = speed;
}
