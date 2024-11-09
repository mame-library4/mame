#include "UITitle.h"

// ----- コンストラクタ -----
UITitle::UITitle()
    : UI(UIManager::UIType::UITitle, L"./Resources/Image/UI/Title/TitleLogo.png")
{
    UI::GetTransform()->SetPosition(50.0f, 350.0f);
}

// ----- 更新 -----
void UITitle::Update(const float& elapsedTime)
{
}

// ----- 描画 -----
void UITitle::Render()
{
    UI::Render();
}

// ----- ImGui用 -----
void UITitle::DrawDebug()
{
    UI::DrawDebug();
}

// ----- 全てのUIを一括で変更 -----
void UITitle::SetIsDrawAllUI(const bool& flag)
{
    UI::SetIsDraw(flag);
}