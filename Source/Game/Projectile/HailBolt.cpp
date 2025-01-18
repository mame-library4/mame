#include "HailBolt.h"
#include "MathHelper.h"
#include "ProjectileManager.h"
#include "AudioManager.h"

// ----- コンストラクタ -----
HailBolt::HailBolt()
    : Projectile("./Resources/Model/Sphere.gltf", 1.0f, "HailBolt",
        static_cast<int>(ProjectileManager::DrawType::Normal), static_cast<int>(ProjectileManager::AttackType::Enemy))
{
}

// ----- 初期化 ----
void HailBolt::Initialize()
{
    GetTransform()->SetScaleFactor(0.1f);

    lifeTimer_ = 3.0f;

    // 攻撃力設定
    SetDamage(20.0f);

    computeParticleEmitter_.SetEmitParameter("HailBoltTrail");
    hitEffectEmitter_.SetEmitParameter("HailBoltHitEffect");
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
        computeParticleEmitter_.SetEmitPosition(GetTransform()->GetPosition());
        computeParticleEmitter_.EmitParticle();
    }

    lifeTimer_ -= elapsedTime;
    if (lifeTimer_ <= 0.0f)
    {
        ProjectileManager::Instance().Remove(this);
    }
}

// ----- 描画 -----
void HailBolt::Render(ID3D11PixelShader* psShader)
{
}

// ----- ImGui用 -----
void HailBolt::DrawDebug()
{
    if (ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::DragFloat("LifeTimer", &lifeTimer_);

        Projectile::DrawDebug();

        ImGui::DragFloat3("MoveDirection", &moveDirection_.x, 0.01f);
        ImGui::DragFloat("MoveSpeed", &moveSpeed_, 0.01f);
        ImGui::Checkbox("IsEffectGeneratable", &isEffectGeneratable_);

        ImGui::TreePop();
    }
}

// ----- 当たった時に呼ばれる -----
const bool HailBolt::OnHit(const DirectX::XMFLOAT3& hitPosition)
{
    // Hitエフェクト再生
    hitEffectEmitter_.SetEmitParameter("HailBoltHitEffect");
    hitEffectEmitter_.SetEmitPosition(hitPosition);
    hitEffectEmitter_.EmitParticle();

    // 効果音を再生
    //AudioManager::Instance().PlaySE()

    // 自分自身を削除する
    ProjectileManager::Instance().Remove(this);

    return true;
}

// ----- 発射 -----
void HailBolt::Launch(const DirectX::XMFLOAT3& emitPosition, const DirectX::XMFLOAT3& direction, const float& speed)
{
    GetTransform()->SetPosition(emitPosition);

    moveDirection_ = direction;
    moveSpeed_ = speed;

    // 発射SEを再生
    AudioManager::Instance().PlaySE(SE::HailBoltLaunch);
}
