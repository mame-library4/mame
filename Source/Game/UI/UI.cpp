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
        // 移動処理
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

// ----- 自身が持つspriteの名前設定 -----
void UI::SetSpriteName(const std::string& name)
{
    if (sprite_ != nullptr)
    {
        sprite_->SetName(name.c_str());
    }
}

// ----- 移動関連の変数設定 -----
void UI::SetMoveTarget(const DirectX::XMFLOAT2& moveValue)
{
    oldPosition_ = GetTransform()->GetPosition();
    moveTargetPosition_ = oldPosition_ + moveValue;
    moveTimer_ = 0.0f;
    isMovingToTarget_ = true;
}

// ----- 他の処理で移動させたときに適応するため -----
void UI::AddMoveTargetPosition(const DirectX::XMFLOAT2& position)
{
    oldPosition_ += position;
    moveTargetPosition_ += position;
}
