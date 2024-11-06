#include "Rock.h"
#include "MathHelper.h"

// ----- コンストラクタ -----
Rock::Rock(const DirectX::XMFLOAT3& createPosition, const DirectX::XMFLOAT3& targetPosition)
    : Projectile("./Resources/Model/Rock/Rock.gltf", 1.0f, "Rock"),
    createPosition_(createPosition), targetPosition_(targetPosition)
{
    GetTransform()->SetPosition(createPosition);

    GetTransform()->SetRotationX(DirectX::XMConvertToRadians(240.0f));

    rockParticle_ = new RockParticle();
}

// ----- 初期化 -----
void Rock::Initialize()
{
    // サイズ設定
    GetTransform()->SetScaleFactor(15.0f);

    rockParticle_->PlaySmokeParticle();
}

// ----- 終了化 -----
void Rock::Finalize()
{
}

// ----- 更新 -----
void Rock::Update(const float& elapsedTime)
{
    lerpTimer_ += lerpSpeed_ * elapsedTime;
    lerpTimer_ = std::min(lerpTimer_, 1.0f);
    DirectX::XMFLOAT3 position = XMFloat3Lerp(createPosition_, targetPosition_, lerpTimer_);

    GetTransform()->SetPosition(position);
}

// ----- 描画 -----
void Rock::Render(ID3D11PixelShader* psShader)
{
    Object::Render(psShader);
}

// ----- ImGui用 -----
void Rock::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        if (ImGui::TreeNodeEx("---------- Movement ----------", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("LerpTimer", &lerpTimer_, 0.1f, 0.0f, 1.0f);
            ImGui::DragFloat("Speed", &lerpSpeed_, 0.1f, 0.0f, 100.0f);

            ImGui::TreePop();
        }

        Projectile::DrawDebug();

        ImGui::TreePop();
    }
}

// ----- 当たった時に呼び出される処理 -----
void Rock::OnHit()
{
}
