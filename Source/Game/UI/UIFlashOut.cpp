#include "UIFlashOut.h"
#include "MathHelper.h"

// ----- �R���X�g���N�^ -----
UIFlashOut::UIFlashOut()
    : UI(UIManager::UIType::UIFlashOut, L"./Resources/Image/White.png", "UIFlashOut")
{
}

// ----- �X�V -----
void UIFlashOut::Update(const float& elapsedTime)
{
    flashOutFrameCounter_ += flashOutSpeed_ * elapsedTime;
    
    const float alpha = XMFloatLerp(1.0f, 0.0f, flashOutFrameCounter_);

    GetTransform()->SetColorA(alpha);

    // ����т��I������玩�g���폜����
    if (flashOutFrameCounter_ > 1.0f)
    {
        UIManager::Instance().Remove(this);
    }
}

// ----- �`�� -----
void UIFlashOut::Render()
{
    UI::Render();
}

void UIFlashOut::DrawDebug()
{
#ifdef _DEBUG
    if (ImGui::TreeNode(GetName().c_str()))
    {
        UI::DrawDebug();

        ImGui::TreePop();
    }
#endif
}
