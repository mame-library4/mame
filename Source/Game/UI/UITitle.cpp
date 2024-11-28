#include "UITitle.h"

// ----- コンストラクタ -----
UITitle::UITitle()
    : UI(UIManager::UIType::UITitle, L"./Resources/Image/White.png", "UITitle")
{
    GetTransform()->SetSize(0.0f, 0.0f);

    sprite_[static_cast<int>(Type::TitleLogo)] = std::make_unique<Sprite>(L"./Resources/Image/UI/Title/DragonHunter.png");

    sprite_[static_cast<int>(Type::PressAnyButton)] = std::make_unique<Sprite>(L"./Resources/Image/UI/Title/TitleWord.png");
    sprite_[static_cast<int>(Type::Back)]           = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    sprite_[static_cast<int>(Type::GameStart)]      = std::make_unique<Sprite>(L"./Resources/Image/UI/Title/TitleWord.png");
    sprite_[static_cast<int>(Type::Option)]         = std::make_unique<Sprite>(L"./Resources/Image/UI/Title/TitleWord.png");
    sprite_[static_cast<int>(Type::Quit)]           = std::make_unique<Sprite>(L"./Resources/Image/UI/Title/TitleWord.png");
    sprite_[static_cast<int>(Type::Black)]          = std::make_unique<Sprite>(L"./Resources/Image/White.png");

    // 初期化
    Initialize();

    isAllUICreated = true;
}

// ----- 初期化 -----
void UITitle::Initialize()
{
    // ----- TitleLogo -----
    sprite_[static_cast<int>(Type::TitleLogo)]->SetName("TitleLogo");
    sprite_[static_cast<int>(Type::TitleLogo)]->GetTransform()->SetPosition(-40.0f, 190.0f);
    sprite_[static_cast<int>(Type::TitleLogo)]->GetTransform()->SetSize(640.0f, 360.0f);
    
    // ----- PressAnyButton -----
    sprite_[static_cast<int>(Type::PressAnyButton)]->SetName("PressAnyButton");
    sprite_[static_cast<int>(Type::PressAnyButton)]->GetTransform()->SetPosition(500.0f, 590.0f);
    sprite_[static_cast<int>(Type::PressAnyButton)]->GetTransform()->SetSize(300.0f, 40.0f);
    sprite_[static_cast<int>(Type::PressAnyButton)]->GetTransform()->SetTexSize(700.0f, 85.0f);

    // ----- Back -----
    sprite_[static_cast<int>(Type::Back)]->SetName("Back");
    sprite_[static_cast<int>(Type::Back)]->GetTransform()->SetPosition(885.0f, 415.0f);
    sprite_[static_cast<int>(Type::Back)]->GetTransform()->SetSize(240.0f, 50.0f);
    sprite_[static_cast<int>(Type::Back)]->GetTransform()->SetColor(backSpriteColor_);
    sprite_[static_cast<int>(Type::Back)]->GetTransform()->SetColorA(0.4f);

    // ----- GameStart -----
    sprite_[static_cast<int>(Type::GameStart)]->SetName("GameStart");
    sprite_[static_cast<int>(Type::GameStart)]->GetTransform()->SetPosition(900.0f, 420.0f);
    sprite_[static_cast<int>(Type::GameStart)]->GetTransform()->SetSize(200.0f, 40.0f);
    sprite_[static_cast<int>(Type::GameStart)]->GetTransform()->SetTexPos(710.0f, 0.0f);
    sprite_[static_cast<int>(Type::GameStart)]->GetTransform()->SetTexSize(400.0f, 85.0f);

    // ----- Option -----
    sprite_[static_cast<int>(Type::Option)]->SetName("Option");
    sprite_[static_cast<int>(Type::Option)]->GetTransform()->SetPosition(895.0f, 470.0f);
    sprite_[static_cast<int>(Type::Option)]->GetTransform()->SetSize(150.0f, 40.0f);
    sprite_[static_cast<int>(Type::Option)]->GetTransform()->SetTexPos(0.0f, 85.0f);
    sprite_[static_cast<int>(Type::Option)]->GetTransform()->SetTexSize(290.0f, 85.0f);

    // ----- Quit -----
    sprite_[static_cast<int>(Type::Quit)]->SetName("Quit");
    sprite_[static_cast<int>(Type::Quit)]->GetTransform()->SetPosition(900.0f, 520.0f);
    sprite_[static_cast<int>(Type::Quit)]->GetTransform()->SetSize(80.0f, 40.0f);
    sprite_[static_cast<int>(Type::Quit)]->GetTransform()->SetTexPos(360.0f, 85.0f);
    sprite_[static_cast<int>(Type::Quit)]->GetTransform()->SetTexSize(155.0f, 85.0f);

    // ----- Black -----
    sprite_[static_cast<int>(Type::Black)]->SetName("Black");
    sprite_[static_cast<int>(Type::Black)]->GetTransform()->SetColorBlack();
    sprite_[static_cast<int>(Type::Black)]->GetTransform()->SetColorA(0.0f);
}

// ----- 更新 -----
void UITitle::Update(const float& elapsedTime)
{
    if (isAllUICreated == false) return;

    if (sprite_[static_cast<int>(Type::Black)] != nullptr)
    {
        const float speed = isTimerDecreasing_ ? -elapsedTime * backSpriteTimerSpeed_ : elapsedTime * backSpriteTimerSpeed_;
        backSpriteTimer_ += speed;
        backSpriteTimer_ = std::clamp(backSpriteTimer_, 0.0f, 1.0f);
        if (backSpriteTimer_ == 1.0f && isTimerDecreasing_ == false) isTimerDecreasing_ = true;
        else if (backSpriteTimer_ == 0.0f && isTimerDecreasing_) isTimerDecreasing_ = false;
        DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        color = XMFloat3Lerp(backSpriteColor_, color, backSpriteTimer_);
        sprite_[static_cast<int>(Type::Back)]->GetTransform()->SetColor(color);
    }
}

// ----- 描画 -----
void UITitle::Render()
{
    if (isAllUICreated == false) return;

    if (UI::GetIsDraw() == false) return;

    for (int i = 0; i < static_cast<int>(Type::Max); ++i)
    {
        //if (i == static_cast<int>(Type::PressAnyButton)) continue;
        if(sprite_[i] != nullptr) sprite_[i]->Render();
    }
}

// ----- ImGui用 -----
void UITitle::DrawDebug()
{
#ifdef _DEBUG
    if(ImGui::TreeNodeEx("---------- BackSprite ----------", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("IsTimerDecreasing", &isTimerDecreasing_);
        ImGui::DragFloat("Timer", &backSpriteTimer_);
        ImGui::DragFloat("Speed", &backSpriteTimerSpeed_, 0.01f, 0.0f, 3.0f);

        ImGui::TreePop();
    }
    if (ImGui::TreeNodeEx("---------- FadeOut ----------", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::DragFloat("Timer", &fadeOutTimer_);
        ImGui::DragFloat("Speed", &fadeOutSpeed_, 0.01f, 0.0f, 3.0f);
        
        ImGui::TreePop();
    }

    for (int i = 0; i < static_cast<int>(Type::Max); ++i)
    {
        if (sprite_[i] != nullptr) sprite_[i]->DrawDebug();
    }

#endif
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

// ----- "Back" 位置設定 -----
void UITitle::SetBackSpritePosition(const int& type)
{
    const float positionY[] = { 415.0f, 465.0f, 515.0f };

    sprite_[static_cast<int>(Type::Back)]->GetTransform()->SetPositionY(positionY[type]);
}

// ----- フェードアウトする -----
const bool UITitle::FadeOut(const float& elapsedtime)
{
    fadeOutTimer_ += fadeOutSpeed_ * elapsedtime;
    fadeOutTimer_ = min(fadeOutTimer_, 1.0f);

    const float alpha = XMFloatLerp(0.0f, 1.0f, fadeOutTimer_);
    sprite_[static_cast<int>(Type::Black)]->GetTransform()->SetColorA(alpha);

    // フェードアウトが終わったらtrueが帰る
    return fadeOutTimer_ == 1.0f;
}
