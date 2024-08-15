#include "Fireball.h"
#include "ProjectileManager.h"
#include "MathHelper.h"
#include "Effect/EffectManager.h"

// ----- コンストラクタ -----
Fireball::Fireball()
    : Projectile("./Resources/Model/Sphere.gltf", 1.0f)
{
    ProjectileManager::Instance().Register(this);
}

// ----- デストラクタ -----
Fireball::~Fireball()
{
}

// ----- 初期化 -----
void Fireball::Initialize()
{
    GetTransform()->SetScaleFactor(0.5f);

    speed_ = 25.0f;
}

// ----- 終了化 -----
void Fireball::Finalize()
{
}

// ----- 更新 -----
void Fireball::Update(const float& elapsedTime)
{
    GetTransform()->AddPosition(direction_ * speed_ * elapsedTime);

    // エフェクト更新
    ++effectDelay_;
    if (effectDelay_ > effectMaxDelay_)
    {
        EffectManager::Instance().GetEffect("Fire")->Play(GetTransform()->GetPosition(), 0.5f, 3.0f);

        effectDelay_ = 0;
    }
}

// ----- 描画 -----
void Fireball::Render(ID3D11PixelShader* psShader)
{
    Object::Render(psShader);
}

// ----- ImGui用 -----
void Fireball::DrawDebug()
{
    Object::DrawDebug();
}

// ----- 発射 -----
void Fireball::Launch(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, const float& speed)
{
    GetTransform()->SetPosition(position);
    direction_ = direction;
    
    speed_ = (speed == 0.0f) ? speed_ : speed;
}
