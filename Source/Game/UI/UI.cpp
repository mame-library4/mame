#include "UI.h"
#include "UIManager.h"

// ----- コンストラクタ -----
UI::UI(const wchar_t* filename)
{
    if (filename != nullptr)
    {
        sprite_ = std::make_unique<Sprite>(filename);
    }

    // マネージャーへ登録
    UIManager::Instance().Register(this);
}

// ----- 描画 -----
void UI::Render()
{
    if (sprite_ != nullptr) sprite_->Render();
}

// ----- ImGui用 -----
void UI::DrawDebug()
{
    if (sprite_ != nullptr) sprite_->DrawDebug();
}
