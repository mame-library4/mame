#include "UIFlashOut.h"
#include "MathHelper.h"

// ----- コンストラクタ -----
UIFlashOut::UIFlashOut()
    : UI(UIManager::UIType::UIFlashOut, L"./Resources/Image/White.png", "UIFlashOut")
{
}

// ----- 更新 -----
void UIFlashOut::Update(const float& elapsedTime)
{
    flashOutFrameCounter_ += flashOutSpeed_ * elapsedTime;
    
    const float alpha = XMFloatLerp(1.0f, 0.0f, flashOutFrameCounter_);

    GetTransform()->SetColorA(alpha);

    // 白飛びが終わったら自身を削除する
    if (flashOutFrameCounter_ > 1.0f)
    {
        UIManager::Instance().Remove(this);
    }
}

// ----- 描画 -----
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
