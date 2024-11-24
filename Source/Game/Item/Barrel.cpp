#include "Barrel.h"
#include "ItemManager.h"
#include "MathHelper.h"
#include "Effect/EffectManager.h"
#include "AudioManager.h"

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
    if (isAttackActive_)
    {
        attackTimer_ += elapsedTime;

        if (attackTimer_ >= attackTime_)
        {
            isAttackActive_ = false;
            ItemManager::Instance().Remove(this);
        }
    }
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
        ImGui::DragFloat("AttackTime", &attackTime_, 0.01f);
        ImGui::DragFloat("AttackTimer", &attackTimer_, 0.01f);

        ImGui::DragFloat("Scale", &effectScele_, 0.01f);
        ImGui::DragFloat("Speed", &effectSpeed_, 0.01f);
        if (ImGui::Button("Play"))
        {
            DirectX::XMFLOAT3 position = GetTransform()->GetPosition() + offsetPosition_;
            EffectManager::Instance().GetEffect("Explosion")->Play(position, effectScele_, effectSpeed_);
        }        

        ImGui::DragFloat3("OffsetPosition", &offsetPosition_.x, 0.01f);
        ImGui::DragFloat("DamageRadius", &damageRadius_, 0.01f);
        ImGui::DragFloat("AttackRadius", &attackRadius_, 0.01f);


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

    isAttackActive_ = true;

    // 爆発効果音を再生
    AudioManager::Instance().PlaySE(SE::Bomb);
}
