#include "UIStamina.h"
#include "Character/Player/PlayerManager.h"

// ----- コンストラクタ -----
UIStamina::UIStamina()
    : UI(UIManager::UIType::UIStamina, L"./Resources/Image/White.png", "UIStamina")
{
    staminaFrame_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    staminaFrame_->SetName("StaminaFrame");
    staminaFrame_->GetTransform()->SetPosition(50.0f, 70.0f);
    staminaFrame_->GetTransform()->SetSize(maxStaminaSizeX_, 7.0f);
    staminaFrame_->GetTransform()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);    

    SetSpriteName("Stamina");
    GetTransform()->SetPosition(50.0f, 70.0f);
    GetTransform()->SetSize(maxStaminaSizeX_, 7.0f);
    GetTransform()->SetColor(1.0f, 1.0f, 0.0f);
}

// ----- 更新 -----
void UIStamina::Update(const float& elapsedTime)
{
    const float stamina = PlayerManager::Instance().GetPlayer()->GetStamina();
    const float maxStamina = PlayerManager::Instance().GetPlayer()->GetMaxStamina();

    // 現在の体力の全体から見た割合を出す
    float currentStamina = stamina / maxStamina;

    const float staminaSizeX = max(maxStaminaSizeX_ * currentStamina, 0.0f);

    GetTransform()->SetSizeX(staminaSizeX);
}

// ----- 描画 -----
void UIStamina::Render()
{
    staminaFrame_->Render();

    UI::Render();
}

// ----- ImGui用 -----
void UIStamina::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        UI::DrawDebug();

        staminaFrame_->DrawDebug();

        ImGui::TreePop();
    }
}
