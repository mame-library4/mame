#include "UITitle.h"

// ----- コンストラクタ -----
UITitle::UITitle()
    : UI(UIManager::UIType::UITitle, L"./Resources/Image/Emma.png")
{
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
}
