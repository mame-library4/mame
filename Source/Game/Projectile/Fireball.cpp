#include "Fireball.h"
#include "ProjectileManager.h"
#include "MathHelper.h"

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
    speed_ = 10.0f;
}

// ----- 終了化 -----
void Fireball::Finalize()
{
}

// ----- 更新 -----
void Fireball::Update(const float& elapsedTime)
{
    GetTransform()->AddPosition(direction_ * speed_ * elapsedTime);
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
