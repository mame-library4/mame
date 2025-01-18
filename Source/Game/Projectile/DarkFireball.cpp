#include "DarkFireball.h"
#include "ProjectileManager.h"
#include "MathHelper.h"

// ----- コンストラクタ -----
DarkFireball::DarkFireball()
    : Projectile("./Resources/Model/Sphere.gltf", 1.0f, "DarkFireball", static_cast<int>(ProjectileManager::DrawType::Normal))
{
    computeParticleEmitter_.SetEmitParameter("DarkFireball");
}

// ----- 初期化 -----
void DarkFireball::Initialize()
{
}

// ----- 終了化 -----
void DarkFireball::Finalize()
{
}

// ----- 更新 -----
void DarkFireball::Update(const float& elapsedTime)
{
    if (isLaunched_ == false) return;

    computeParticleEmitter_.SetEmitPosition(GetTransform()->GetPosition());
    computeParticleEmitter_.EmitParticle();

    stateChangeTimer_ -= elapsedTime;
    if (state_ == 0)
    {

        // タイマーが0になったらステート変更
        if (stateChangeTimer_ <= 0.0f)
        {
            const DirectX::XMFLOAT3 ownerPosition = GetTransform()->GetPosition();
            moveDirection_ = XMFloat3Normalize(targetPosition_ - ownerPosition);

            state_ = 1;
        }
    }
    else
    {
        const DirectX::XMFLOAT3 ownerPosition = GetTransform()->GetPosition();
        const float length = XMFloat3Length(targetPosition_ - ownerPosition);
        if (length <= deleteRange_)
        {
            ProjectileManager::Instance().Remove(this);
        }
    }

    GetTransform()->AddPosition(moveDirection_ * moveSpeed_ * elapsedTime);
}

// ----- 描画　-----
void DarkFireball::Render(ID3D11PixelShader* psShader)
{
}

// ----- ImGui用 -----
void DarkFireball::DrawDebug()
{
}

const bool DarkFireball::OnHit(const DirectX::XMFLOAT3& hitPosition)
{
    return true;
}

// ----- 発射 -----
void DarkFireball::Launch(const DirectX::XMFLOAT3& emitPosition, const DirectX::XMFLOAT3& moveDirection, const DirectX::XMFLOAT3& targetPosition, const float& moveSpeed, const float& changeTime)
{
    GetTransform()->SetPosition(emitPosition);

    moveDirection_      = moveDirection;
    moveSpeed_          = moveSpeed;
    targetPosition_     = targetPosition;
    stateChangeTimer_   = changeTime;
    isLaunched_         = true;
}
