#include "UIActionGuide.h"

// ----- �R���X�g���N�^ -----
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

// ----- �X�V -----
void UIActionGuide::Update(const float& elapsedTime)
{
}

// ----- �`�� -----
void UIActionGuide::Render()
{
    UI::Render();
}

// ----- ImGui�p -----
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
