#include "UIActionGuide.h"

// ----- �R���X�g���N�^ -----
UIActionGuide::UIActionGuide()
    : UI(UIManager::UIType::UIActionGuide, L"./Resources/Image/UI/ActionGuide.png", "UIActionGuide")
{
    GetTransform()->SetPosition(1000.0f, 40.0f);
    GetTransform()->SetSize(252.0f, 198.0f);
    GetTransform()->SetTexSize(700.0f, 550.0f);
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
