#include "UIRush.h"

// ----- コンストラクタ -----
UIRush::UIRush()
    : UI(UIManager::UIType::UIRush, L"./Resources/Image/UI/Rush/Rush.png", "UIRush")
{
    rushWord_ = std::make_unique<Sprite>(L"./Resources/Image/UI/Rush/RushWord.png");

    // 初期化
    Initialize();

    // 描画フラグを立てる
    SetIsDraw();
}

// ----- 初期化 -----
void UIRush::Initialize()
{
    SetSpriteName("RushButtton");
    GetTransform()->SetPosition(720.0f, 520.0f);
    GetTransform()->SetSize(142.0f, 48.0f);

    rushWord_->SetName("RushWord");
    rushWord_->GetTransform()->SetPosition(720.0f, 520.0f);
    rushWord_->GetTransform()->SetSize(142.0f, 48.0f);
    rushWord_->GetTransform()->SetColorA(0.0f);
}

// ----- 更新 -----
void UIRush::Update(const float& elapsedTime)
{
    fadeTimer_ += fadeSpeed_ * elapsedTime;
    if (fadeTimer_ <= 0.0f || fadeTimer_ >= 1.0f)
    {
        fadeSpeed_ = fadeSpeed_ * -1;
        fadeTimer_ = std::clamp(fadeTimer_, 0.0f, 1.0f);
    }
    const float alpha = XMFloatLerp(0.0f, 1.0f, fadeTimer_);
    rushWord_->GetTransform()->SetColorA(alpha);
}

// ----- 描画 -----
void UIRush::Render()
{    
    UI::Render();
    rushWord_->Render();
}

// ----- ImGui用 -----
void UIRush::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat("Timer", &fadeTimer_);
        ImGui::DragFloat("Speed", &fadeSpeed_, 0.01f, 0.0f, 10.0f);

        UI::DrawDebug();

        rushWord_->DrawDebug();

        ImGui::TreePop();
    }
}
