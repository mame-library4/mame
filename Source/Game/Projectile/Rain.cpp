#include "Rain.h"
#include "ProjectileManager.h"
#include "MathHelper.h"

// ----- コンストラクタ -----
Rain::Rain()
    : Projectile("./Resources/Model/Sphere.gltf", 1.0f, "Rain",
        static_cast<int>(ProjectileManager::DrawType::Normal), static_cast<int>(ProjectileManager::AttackType::Enemy))
{
}

// ----- 初期化 -----
void Rain::Initialize()
{
    SetRadius(1.0f);
}

// ----- 終了化 -----
void Rain::Finalize()
{
}

// ----- 更新 -----
void Rain::Update(const float& elapsedTime)
{
    GetTransform()->AddPosition(moveDirection_ * moveSpeed_ * elapsedTime);

    if (GetTransform()->GetPositionY() <= 0.0f)
    {
        ProjectileManager::Instance().Remove(this);
    }
}

// ----- ImGui用 -----
void Rain::DrawDebug()
{
    if (ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::TreePop();
    }
}

// ----- 当たった時に呼ばれる -----
void Rain::OnHit(const DirectX::XMFLOAT3& hitPosition)
{
    ProjectileManager::Instance().Remove(this);
}

// ----- 発射 -----
void Rain::Launch(const DirectX::XMFLOAT3& moveDirection, const float& moveSpeed)
{
    moveDirection_  = moveDirection;
    moveSpeed_      = moveSpeed;
}
