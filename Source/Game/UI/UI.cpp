#include "UI.h"
#include "UIManager.h"

// ----- �R���X�g���N�^ -----
UI::UI(const wchar_t* filename)
{
    if (filename != nullptr)
    {
        sprite_ = std::make_unique<Sprite>(filename);
    }

    // �}�l�[�W���[�֓o�^
    UIManager::Instance().Register(this);
}

// ----- �`�� -----
void UI::Render()
{
    if (sprite_ != nullptr) sprite_->Render();
}

// ----- ImGui�p -----
void UI::DrawDebug()
{
    if (sprite_ != nullptr) sprite_->DrawDebug();
}
