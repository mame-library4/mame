#include "AquaBullet.h"
#include "ProjectileManager.h"
#include "Graphics.h"
#include "Texture.h"
#include "MathHelper.h"

// ----- コンストラクタ -----
AquaBullet::AquaBullet()
    : Projectile("./Resources/Model/Sphere.gltf", 1.0f, "AquaBullet", static_cast<int>(ProjectileManager::DrawType::Normal))
{
    aquaBulletConstants_ = std::make_unique<ConstantBuffer<AquaBulletConstants>>();

    Graphics::Instance().CreatePsFromCso("./Resources/Shader/AquaBulletPS.cso", aquaBulletPS_.GetAddressOf());
    D3D11_TEXTURE2D_DESC desc = {};
    //Texture::Instance().LoadTexture(L"./Resources/Image/Mask/Mask1.png", shaderResourceView_.GetAddressOf(), &desc);
    Texture::Instance().LoadTexture(L"./Resources/Image/Mask/Noise1.png", shaderResourceView_.GetAddressOf(), &desc);
}

void AquaBullet::Initialize()
{    
    Object::SetScrollDirection({ 1.0f, 1.0f });

    lifeTimer_ = 3.0f;
}

void AquaBullet::Finalize()
{
}

void AquaBullet::Update(const float& elapsedTime)
{
    // 座標更新
    GetTransform()->AddPosition(moveDirection_ * moveSpeed_ * elapsedTime);

    Object::AddScrollTimer(elapsedTime);

    lifeTimer_ -= elapsedTime;
    if (lifeTimer_ <= 0.0f)
    {
        ProjectileManager::Instance().Remove(this);
    }
}

// ----- 描画 -----
void AquaBullet::Render(ID3D11PixelShader* psShader)
{
    aquaBulletConstants_->Activate(9);
    Graphics::Instance().GetDeviceContext()->PSSetShaderResources(9, 1, shaderResourceView_.GetAddressOf());
    Object::Render(aquaBulletPS_.Get());
}

// ----- ImGui用 -----
void AquaBullet::DrawDebug()
{
    if (ImGui::TreeNodeEx(GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        Projectile::DrawDebug();

        ImGui::ColorEdit3("BaseColor", &aquaBulletConstants_->GetData()->baseColor_.x);
        ImGui::ColorEdit3("RimColor", &aquaBulletConstants_->GetData()->rimColor_.x);
        ImGui::DragFloat("RimThreshold", &aquaBulletConstants_->GetData()->rimThreshold_, 0.1f, 0.0f, 1.0f);
        ImGui::DragFloat("RimAmount", &aquaBulletConstants_->GetData()->rimAmount_, 0.001f, 0.0f, 1.0f);

        ImGui::TreePop();
    }
}

void AquaBullet::OnHit()
{
}

// ----- 発射 -----
void AquaBullet::Launch(const DirectX::XMFLOAT3& moveDirection, const float& moveSpeed)
{
    moveDirection_ = moveDirection;
    moveSpeed_ = moveSpeed;
}
