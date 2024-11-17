#include "UILoading.h"

// ----- コンストラクタ -----
UILoading::UILoading()
    : UI(UIManager::UIType::UIFader, L"./Resources/Image/White.png", "UILoading")
{
    GetTransform()->SetColorBlack();

    // 描画フラグを立てる
    SetIsDraw();
}

// ----- 更新 -----
void UILoading::Update(const float& elapsedTime)
{
}

// ----- 描画 -----
void UILoading::Render()
{
    UI::Render();
}

// ----- ImGui用 -----
void UILoading::DrawDebug()
{
}
