#include "UIFader.h"
#include "Easing.h"


// ----- �R���X�g���N�^ -----
UIFader::UIFader()
    : UI(UIManager::UIType::UIFader, L"./Resources/Image/White.png", "UIFader")
{
    GetTransform()->SetColorBlack();
}

// ----- �X�V -----
void UIFader::Update(const float& elapsedTime)
{
    const float totalFrame = 1.0f;

    fadeTimer_ += elapsedTime;    
    fadeTimer_ = min(fadeTimer_, totalFrame);

    const float alpha = Easing::InSine(fadeTimer_, totalFrame, 1.0f, 0.0f);

    GetTransform()->SetColorA(alpha);

    if (fadeTimer_ == totalFrame)
    {
        UIManager::Instance().Remove(this);
    }
}

// ----- �`�� -----
void UIFader::Render()
{
    UI::Render();
}

// ----- ImGui�p -----
void UIFader::DrawDebug()
{
#ifdef _DEBUG
    if (ImGui::TreeNode(GetName().c_str()))
    {
        UI::DrawDebug();

        ImGui::TreePop();
    }
#endif
}
