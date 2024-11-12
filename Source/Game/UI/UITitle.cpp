#include "UITitle.h"

// ----- コンストラクタ -----
UITitle::UITitle()
    : UI(UIManager::UIType::UITitle)
{
    sprite_[static_cast<int>(Type::TitleLogo)] = std::make_unique<Sprite>(L"./Resources/Image/UI/Title/TitleLogo.png");

    // 初期化
    Initialize();
}

// ----- 初期化 -----
void UITitle::Initialize()
{
    sprite_[static_cast<int>(Type::TitleLogo)]->GetTransform()->SetPosition(50.0f, 350.0f);
}

// ----- 更新 -----
void UITitle::Update(const float& elapsedTime)
{
}

// ----- 描画 -----
void UITitle::Render()
{
    for (int i = 0; i < static_cast<int>(Type::Max); ++i)
    {
        if(sprite_[i] != nullptr) sprite_[i]->Render();
    }
}

// ----- ImGui用 -----
void UITitle::DrawDebug()
{
}

// ----- 全てのUIを一括で変更 -----
void UITitle::SetIsDrawAllUI(const bool& flag)
{
    for (int i = 0; i < static_cast<int>(Type::Max); ++i)
    {
        if (sprite_[i] != nullptr) sprite_[i]->SetIsDraw(flag);
    }
}

// ----- 指定のUIの表示を変更 -----
void UITitle::SetIsDraw(const Type& type, const bool& flag)
{
    sprite_[static_cast<int>(type)]->SetIsDraw(flag);
}
