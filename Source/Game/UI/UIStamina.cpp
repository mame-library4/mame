#include "UIStamina.h"
#include "Easing.h"
#include "Character/Player/PlayerManager.h"

// ----- コンストラクタ -----
UIStamina::UIStamina()
    : UI(UIManager::UIType::UIStamina, L"./Resources/Image/White.png", "UIStamina")
{
    // スタミナアイコン
    staminaIcon_ = std::make_unique<Sprite>(L"./Resources/Image/UI/Stamina.png");
    staminaIcon_->SetName("StaminaIcon");
    staminaIcon_->GetTransform()->SetPosition(20.0f, 52.0f);
    staminaIcon_->GetTransform()->SetSize(32.0f);
    staminaIcon_->GetTransform()->SetColor(0.59f, 0.59f, 0.59f);

    const DirectX::XMFLOAT3 colorYellow = DirectX::XMFLOAT3(0.47f, 0.47f, 0.12f);
    const DirectX::XMFLOAT4 colorBlack = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.4f);
    
    // ひし形
    staminaRhombus_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    staminaRhombus_->SetName("StaminaRhombus");
    staminaRhombus_->GetTransform()->SetPosition(60.0f, 64.0f);
    staminaRhombus_->GetTransform()->SetSize(9.0f);
    staminaRhombus_->GetTransform()->SetAngle(45.0f);
    staminaRhombus_->GetTransform()->SetColor(colorYellow);

    // ひし形の枠
    staminaRhombusFrame_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    staminaRhombusFrame_->SetName("StaminaRhombusFrame");
    staminaRhombusFrame_->GetTransform()->SetPosition(58.0f, 62.0f);
    staminaRhombusFrame_->GetTransform()->SetSize(13.0f);
    staminaRhombusFrame_->GetTransform()->SetAngle(45.0f);
    staminaRhombusFrame_->GetTransform()->SetColor(colorBlack);

    const DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(68.0f, 66.0f);
    const float sizeY = 5.0f;

    // スタミナが少ない警告(赤色)の設定
    staminaWarning_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    staminaWarning_->SetName("StaminaWarning");
    staminaWarning_->GetTransform()->SetPosition(position);
    staminaWarning_->GetTransform()->SetSize(maxStaminaSizeX_, sizeY);
    staminaWarning_->GetTransform()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    // スタミナの枠(黒色)の設定
    staminaFrame_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    staminaFrame_->SetName("StaminaFrame");
    staminaFrame_->GetTransform()->SetPosition(68.0f, 64.0f);
    staminaFrame_->GetTransform()->SetSize(352.0f, 9.0f);
    staminaFrame_->GetTransform()->SetColor(colorBlack);


    SetSpriteName("Stamina");
    GetTransform()->SetPosition(position);
    GetTransform()->SetSize(maxStaminaSizeX_, sizeY);
    GetTransform()->SetColor(colorYellow);

    // 全てのUIが生成された
    isAllUICreated = true;
}

// ----- 更新 -----
void UIStamina::Update(const float& elapsedTime)
{
    if (isAllUICreated == false) return;

    const float stamina = PlayerManager::Instance().GetPlayer()->GetStamina();
    const float maxStamina = PlayerManager::Instance().GetPlayer()->GetMaxStamina();

    // 現在の体力の全体から見た割合を出す
    float currentStamina = stamina / maxStamina;

    const float staminaSizeX = max(maxStaminaSizeX_ * currentStamina, 0.0f);

    GetTransform()->SetSizeX(staminaSizeX);

    // ------------------------------------------------------------
    //  スタミナが回避のコスト使用コストより少なかったらWarningを出す
    // ------------------------------------------------------------
    const float dodgeStaminaCost = PlayerManager::Instance().GetPlayer()->GetDodgeStaminaCost();
    if (stamina < dodgeStaminaCost)
    {
        warningFlashTimer_ += elapsedTime;
        if (warningFlashTimer_ >= warningFlashTime_)
        {
            isFadingIn_ = isFadingIn_ ? false : true;
            warningFlashTimer_ = 0.0f;
        }

        float alpha = 0.0f;
        if (isFadingIn_)
        {
            alpha = Easing::InSine(warningFlashTimer_, warningFlashTime_, 1.0f, 0.0f);
        }
        else
        {
            alpha = Easing::InSine(warningFlashTimer_, warningFlashTime_, 0.0f, 1.0f);
        }
        staminaWarning_->GetTransform()->SetColorA(alpha);
    }
    else
    {
        staminaWarning_->GetTransform()->SetColorA(0.0f);
    }
}

// ----- 描画 -----
void UIStamina::Render()
{
    // 全てのUIが生成されていない
    if (isAllUICreated == false) return;
    // 描画しない
    if (GetIsDraw() == false) return;

    staminaFrame_->Render();
    staminaRhombusFrame_->Render();

    staminaWarning_->Render();

    staminaIcon_->Render();

    UI::Render();
    staminaRhombus_->Render();
}

// ----- ImGui用 -----
void UIStamina::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat("warningFlashTime", &warningFlashTime_);

        staminaIcon_->DrawDebug();

        UI::DrawDebug();
        staminaRhombus_->DrawDebug();

        staminaWarning_->DrawDebug();

        staminaFrame_->DrawDebug();
        staminaRhombusFrame_->DrawDebug();

        ImGui::TreePop();
    }
}
