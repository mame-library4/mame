#include "Fireball.h"
#include "MathHelper.h"
#include "ProjectileManager.h"
#include "Graphics.h"
#include "Texture.h"

// ----- コンストラクタ -----
Fireball::Fireball()
    : Projectile("./Resources/Model/Sphere.gltf", 1.0f, "FireBall",
        static_cast<int>(ProjectileManager::DrawType::Normal), static_cast<int>(ProjectileManager::AttackType::Player))
{
    aquaConstants_ = std::make_unique<ConstantBuffer<AquaConstants>>();

    Graphics::Instance().CreatePsFromCso("./Resources/Shader/AquaBulletPS.cso", aquaPS_.GetAddressOf());
    D3D11_TEXTURE2D_DESC desc = {};
    Texture::Instance().LoadTexture(L"./Resources/Image/Mask/Noise1.png", shaderResourceView_.GetAddressOf(), &desc);
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

    // エフェクト読み込み
    computeParticleEmitter_.SetEmitParameter("FireballTrail");
    hitEffect0Emitter_.SetEmitParameter("FireballHitEffect0");
    hitEffect1Emitter_.SetEmitParameter("FireballHitEffect1");

    Object::SetScrollDirection({ 1.0f, 1.0f });
}

// ----- 終了化 -----
void Fireball::Finalize()
{
}

// ----- 更新 -----
void Fireball::Update(const float& elapsedTime)
{
    Object::AddScrollTimer(elapsedTime);

    GetTransform()->AddPosition(moveDirection_ * moveSpeed_ * elapsedTime);

    // 軌跡エフェクトを生成する
    if (GetIsHit() == false)
    {
        computeParticleEmitter_.SetEmitPosition(GetTransform()->GetPosition());
        computeParticleEmitter_.EmitParticle();
    }

    // ヒットしていたら、ヒットエフェクトを指定回数生成する
    if (GetIsHit())
    {
        // ----- HitEffect0 -----
        if (hitEffect0EmitCounter_ < hitEffect0EmitNum_)
        {
            hitEffect0EmitTimer_ += elapsedTime;
            if (hitEffect0EmitTimer_ >= hitEffect0EmitTime_)
            {
                hitEffect0Emitter_.SetEmitPosition(GetTransform()->GetPosition());
                hitEffect0Emitter_.EmitParticle();

                hitEffect0EmitTimer_ = 0.0f;
                ++hitEffect0EmitCounter_;
            }
        }

        // ----- HitEffect1 -----
        hitEffect1EmitTimer_ += elapsedTime;
        if (hitEffect1EmitTimer_ >= hitEffect1EmitTime_)
        {
            DirectX::XMFLOAT3 emitPosition = GetTransform()->GetPosition();
            emitPosition.y = 0.1f;
            hitEffect1Emitter_.SetEmitPosition(emitPosition);
            hitEffect1Emitter_.EmitParticle();

            hitEffect1EmitTimer_ = 0.0f;
            ++hitEffect1EmitCounter_;

            // 指定回数生成したら自分自身を削除する
            if (hitEffect1EmitCounter_ >= hitEffect1EmitNum_)
            {
                ProjectileManager::Instance().Remove(this);
            }
        }
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
    // 既に当たっているため、描画しない
    if (GetIsHit()) return;

    aquaConstants_->Activate(9);
    Graphics::Instance().GetDeviceContext()->PSSetShaderResources(9, 1, shaderResourceView_.GetAddressOf());
    Object::Render(aquaPS_.Get());
}

// ----- ImGui用 -----
void Fireball::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat("MoveSpeed", &moveSpeed_, 0.01f);

        ImGui::ColorEdit3("BaseColor", &aquaConstants_->GetData()->baseColor_.x);
        ImGui::ColorEdit3("RimColor", &aquaConstants_->GetData()->rimColor_.x);
        ImGui::DragFloat("RimThreshold", &aquaConstants_->GetData()->rimThreshold_, 0.1f, 0.0f, 1.0f);
        ImGui::DragFloat("RimAmount", &aquaConstants_->GetData()->rimAmount_, 0.001f, 0.0f, 1.0f);

        Projectile::DrawDebug();

        ImGui::TreePop();
    }
}

// ----- 当たった時に呼び出される処理 -----
const bool Fireball::OnHit(const DirectX::XMFLOAT3& hitPosition)
{
    // 移動速度をゼロにする
    moveSpeed_ = 0.0f;

    return true;
}

// ----- 発射 -----
void Fireball::Launch(const DirectX::XMFLOAT3& emitPosition, const DirectX::XMFLOAT3& moveDirection, const float& moveSpeed)
{
    // 位置を設定する
    GetTransform()->SetPosition(emitPosition);

    moveDirection_  = moveDirection;
    moveSpeed_      = moveSpeed;
}
