#include "UI.h"
#include "UIManager.h"

// ----- �R���X�g���N�^ -----
UI::UI(const UIManager::UIType& type, const wchar_t* filename, const std::string& name)
    : type_(type)
{
    if (filename != nullptr)
    {
        sprite_ = std::make_unique<Sprite>(filename);
    }

    // ���O��ݒ�
    name_ = name + std::to_string(UIManager::Instance().GetImGuiNameNum());

    // �}�l�[�W���[�֓o�^
    UIManager::Instance().Register(this);
}

// ----- �X�V -----
void UI::Update(const float& elapsedTime)
{
    if (sprite_ != nullptr)
    {
    }
}

// ----- �`�� -----
void UI::Render()
{
    if (isDraw_ == false) return;

    if (sprite_ != nullptr) sprite_->Render();
}

// ----- ImGui�p -----
void UI::DrawDebug()
{
    if (sprite_ != nullptr) sprite_->DrawDebug();
}

// ----- ���g������sprite�̖��O�ݒ� -----
void UI::SetSpriteName(const std::string& name)
{
    if (sprite_ != nullptr)
    {
        sprite_->SetName(name.c_str());
    }
}