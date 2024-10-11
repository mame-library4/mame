#include "UIActionGuide.h"

// ----- コンストラクタ -----
UIActionGuide::UIActionGuide()
    : UI(UIManager::UIType::UIActionGuide, L"./Resources/Image/UI/ActionGuide.png", "UIActionGuide")
{
#if 1
    GetTransform()->SetPosition(980.0f, 40.0f);
    GetTransform()->SetSize(300.0f, 140.0f);
#else
    GetTransform()->SetPosition(1050.0f, 40.0f);
    GetTransform()->SetSize(225.0f, 105.0f);
#endif

    GetTransform()->SetTexSize(750.0f, 350.0f);
}

// ----- 更新 -----
void UIActionGuide::Update(const float& elapsedTime)
{
}

// ----- 描画 -----
void UIActionGuide::Render()
{
    UI::Render();
}

// ----- ImGui用 -----
void UIActionGuide::DrawDebug()
{
#ifdef _DEBUG
    if (ImGui::TreeNode(GetName().c_str()))
    {
        UI::DrawDebug();

        ImGui::TreePop();
    }
#endif
}
