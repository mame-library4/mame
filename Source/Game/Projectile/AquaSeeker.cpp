#include "AquaSeeker.h"
#include "ProjectileManager.h"
#include "Character/Enemy/EnemyManager.h"
#include "Character/Player/PlayerManager.h"

// ----- コンストラクタ -----
AquaSeeker::AquaSeeker()
    : Projectile("./Resources/Model/Sphere.gltf", 1.0f, "AquaSeeker",
        static_cast<int>(ProjectileManager::DrawType::Normal), static_cast<int>(ProjectileManager::AttackType::Enemy))
{
    computeParticleEmitter_.SetEmitParameter("AquaBulletTrail");
}

// ----- 初期化 -----
void AquaSeeker::Initialize()
{
    SetDamage(30.0f);
}

// ----- 終了化 -----
void AquaSeeker::Finalize()
{
}

// ----- 更新 -----
void AquaSeeker::Update(const float& elapsedTime)
{
    // 敵にホーミングする
    if (EnemyManager::Instance().GetEnemyCount() <= 0) return;

    const DirectX::XMFLOAT3 enemyPosition = EnemyManager::Instance().GetEnemy(0)->GetJointPosition("Dragon15_spine2");
    const DirectX::XMFLOAT3 position = GetTransform()->GetPosition();
    const DirectX::XMFLOAT3 moveDirection = XMFloat3Normalize(enemyPosition - position);

    DirectX::XMFLOAT3 moveValue = {};

    if (moveState_ == 0)
    {
        moveValue = moveDirection_ * moveSpeed_ * elapsedTime;

        timer_ += elapsedTime;

        if (timer_ >= 0.5f) moveState_ = 1;
    }
    else
    {
        moveValue = moveDirection * moveSpeed_ * elapsedTime;
    }
    GetTransform()->AddPosition(moveValue);

    computeParticleEmitter_.SetEmitPosition(GetTransform()->GetPosition());
    computeParticleEmitter_.EmitParticle();
}

// ----- 描画 -----
void AquaSeeker::Render(ID3D11PixelShader* psShader)
{
}

// ----- ImGui用 -----
void AquaSeeker::DrawDebug()
{
    if (ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {


        Projectile::DrawDebug();

        ImGui::TreePop();
    }
}

// ----- 当たった時に呼ばれる -----
void AquaSeeker::OnHit(const DirectX::XMFLOAT3& hitPosition)
{

    // 自分自身を削除する
    ProjectileManager::Instance().Remove(this);
}

// ----- 発射方向を決定する -----
void AquaSeeker::SetMoveType(const int& moveType)
{
    const DirectX::XMFLOAT3 playerForward = XMFloat3Normalize(PlayerManager::Instance().GetTransform()->CalcForward());
    const DirectX::XMFLOAT3 playerRight = XMFloat3Normalize(PlayerManager::Instance().GetTransform()->CalcRight());
    const DirectX::XMFLOAT3 playerUp = XMFloat3Normalize(PlayerManager::Instance().GetTransform()->CalcUp());

    const DirectX::XMFLOAT3 moveDirection[3] =
    {
        // 前 右斜め上
        { XMFloat3Normalize(playerForward + playerRight + playerUp) },
        // 前 左斜め上
        { XMFloat3Normalize(playerForward + (playerRight * -1.0f) + playerUp) },
        // 前斜め上
        { XMFloat3Normalize(playerForward + playerUp) },
    };

    moveDirection_ = moveDirection[moveType];
}
