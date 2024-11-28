#include "UILoading.h"

// ----- コンストラクタ -----
UILoading::UILoading()
    : UI(UIManager::UIType::UIFader, L"./Resources/Image/UI/LoadDragonHunter.png", "UILoading")
{
    GetTransform()->SetPosition(770.0f, 500.0f);
    GetTransform()->SetSize(320.0f, 180.0f);

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
    UI::DrawDebug();
}
