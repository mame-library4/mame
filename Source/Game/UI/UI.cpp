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
        // �ړ�����
        if(isMovingToTarget_)
        {
            moveTimer_ += moveSpeed_ * elapsedTime;
            moveTimer_ = min(moveTimer_, 1.0f);

            const DirectX::XMFLOAT2 position = XMFloat2Lerp(oldPosition_, moveTargetPosition_, moveTimer_);

            GetTransform()->SetPosition(position);

            if (moveTimer_ == 1.0f) isMovingToTarget_ = false;
        }
    }
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

// ----- ���g������sprite�̖��O�ݒ� -----
void UI::SetSpriteName(const std::string& name)
{
    if (sprite_ != nullptr)
    {
        sprite_->SetName(name.c_str());
    }
}

// ----- �ړ��֘A�̕ϐ��ݒ� -----
void UI::SetMoveTarget(const DirectX::XMFLOAT2& moveValue)
{
    oldPosition_ = GetTransform()->GetPosition();
    moveTargetPosition_ = oldPosition_ + moveValue;
    moveTimer_ = 0.0f;
    isMovingToTarget_ = true;
}

// ----- ���̏����ňړ��������Ƃ��ɓK�����邽�� -----
void UI::AddMoveTargetPosition(const DirectX::XMFLOAT2& position)
{
    oldPosition_ += position;
    moveTargetPosition_ += position;
}
