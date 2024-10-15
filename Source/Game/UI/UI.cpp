#include "UI.h"
#include "UIManager.h"

// ----- コンストラクタ -----
UI::UI(const UIManager::UIType& type, const wchar_t* filename, const std::string& name)
    : type_(type)
{
    if (filename != nullptr)
    {
        sprite_ = std::make_unique<Sprite>(filename);
    }

    // 名前を設定
    name_ = name + std::to_string(UIManager::Instance().GetImGuiNameNum());

    // マネージャーへ登録
    UIManager::Instance().Register(this);
}

// ----- 更新 -----
void UI::Update(const float& elapsedTime)
{
    if (sprite_ != nullptr)
    {
    }
}

// ----- 描画 -----
void UI::Render()
{
    if (isDraw_ == false) return;

    if (sprite_ != nullptr) sprite_->Render();
}

// ----- ImGui用 -----
void UI::DrawDebug()
{
    if (sprite_ != nullptr) sprite_->DrawDebug();
}

// ----- 自身が持つspriteの名前設定 -----
void UI::SetSpriteName(const std::string& name)
{
    if (sprite_ != nullptr)
    {
        sprite_->SetName(name.c_str());
    }
}