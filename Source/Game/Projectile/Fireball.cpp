#include "Fireball.h"
#include "ProjectileManager.h"
#include "MathHelper.h"
#include "Effect/EffectManager.h"

// ----- コンストラクタ -----
Fireball::Fireball()
    : Projectile("./Resources/Model/Sphere.gltf", 1.0f)
{
    // Managerに自分自身を登録
    ProjectileManager::Instance().Register(this);
}

// ----- デストラクタ -----
Fireball::~Fireball()
{
}

// ----- 初期化 -----
void Fireball::Initialize()
{
    // サイズ設定
    GetTransform()->SetScaleFactor(0.5f);

    // 当たり判定の半径設定
    SetRadius(0.5f);

    // カウンター判定用の半径設定
    SetCounterRadius(2.0f);

    // ダメージ設定
    SetDamage(40);

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

    lifeTimer_ += elapsedTime;
    if (lifeTimer_ > 10.0f)
    {
        ProjectileManager::Instance().Remove(this);
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
    std::string name = "Fireball" + std::to_string(GetId());
    if (ImGui::TreeNode(name.c_str()))
    {
        Projectile::DrawDebug();
        ImGui::TreePop();
    }
}

// ----- 発射 -----
void Fireball::Launch(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, const float& speed)
{
    GetTransform()->SetPosition(position);
    direction_ = direction;
    
    speed_ = (speed == 0.0f) ? speed_ : speed;
}
