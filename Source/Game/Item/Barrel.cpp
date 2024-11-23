#include "Barrel.h"
#include "MathHelper.h"
#include "Effect/EffectManager.h"

// ----- コンストラクタ -----
Barrel::Barrel(const DirectX::XMFLOAT3& generatePosition)
    : Item("./Resources/Model/Barrel/Barrel.gltf", 1.0f, "Barrel")
{
    GetTransform()->SetPosition(generatePosition);
    GetTransform()->SetScale({ 1.1f, 1.0f, 1.1f });

    offsetPosition_ = DirectX::XMFLOAT3(0.0f, 0.6f, 0.0f);
    damageRadius_ = 0.7f;
}

// ----- 初期化 -----
void Barrel::Initialize()
{
}

// ----- 終了化 -----
void Barrel::Finalize()
{
}

// ----- 更新 -----
void Barrel::Update(const float& elapsedTime)
{
}

// ----- 描画 -----
void Barrel::Render(ID3D11PixelShader* psShader)
{
    if(isDrawModel_) Object::Render(psShader);
}

// ----- ImGui用 -----
void Barrel::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat("Scale", &effectScele_, 0.01f);
        ImGui::DragFloat("Speed", &effectSpeed_, 0.01f);
        if (ImGui::Button("Play"))
        {
            DirectX::XMFLOAT3 position = GetTransform()->GetPosition() + offsetPosition_;
            EffectManager::Instance().GetEffect("Explosion")->Play(position, effectScele_, effectSpeed_);
        }        

        ImGui::DragFloat3("OffsetPosition", &offsetPosition_.x, 0.01f);
        ImGui::DragFloat("DamageRadius", &damageRadius_, 0.01f);


        Item::DrawDebug();

        ImGui::TreePop();
    }
}

// ----- ヒットしたときに呼ばれる -----
void Barrel::OnHit()
{
    DirectX::XMFLOAT3 position = GetTransform()->GetPosition() + offsetPosition_;
    EffectManager::Instance().GetEffect("Explosion")->Play(position, effectScele_, effectSpeed_);
    isDrawModel_ = false;
}
