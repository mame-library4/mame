#include "UITitle.h"

// ----- �R���X�g���N�^ -----
UITitle::UITitle()
    : UI(UIManager::UIType::UITitle, L"./Resources/Image/Emma.png")
{
}

// ----- �X�V -----
void UITitle::Update(const float& elapsedTime)
{
}

// ----- �`�� -----
void UITitle::Render()
{
    UI::Render();
}

// ----- ImGui�p -----
void UITitle::DrawDebug()
{
}
