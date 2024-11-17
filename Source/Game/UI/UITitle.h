#pragma once
#include "UI.h"

class UITitle : public UI
{
public:
    UITitle();
    ~UITitle() override {}

    void Initialize();
    void Update(const float& elapsedTime)   override; // çXêV
    void Render()                           override; // ï`âÊ
    void DrawDebug()                        override; // ImGuióp

public:
    enum class Type
    {
        // ----- ï`âÊèá -----
        TitleLogo,      // |
        PressAnyButton, // |
        Back,           // |
        GameStart,      // |
        Option,         // |
        Quit,           // |
        Black,          // |
                        // Å´

        Max,
    };

public:
    void SetIsDrawAllUI(const bool& flag);
    void SetIsDraw(const Type& type, const bool& flag);
    void SetBackSpritePosition(const int& type);
    [[nodiscard]] const bool FadeOut(const float& elapsedtime);

private:
    std::unique_ptr<Sprite> sprite_[static_cast<int>(Type::Max)];

    // ----- BackSprite -----
    const DirectX::XMFLOAT3 backSpriteColor_ = DirectX::XMFLOAT3(0.4f, 0.75f, 0.47f);
    float backSpriteTimer_      = 0.0f;
    float backSpriteTimerSpeed_ = 2.0f;
    bool  isTimerDecreasing_    = false;

    // ----- FadeOut -----
    float fadeOutTimer_ = 0.0f;
    float fadeOutSpeed_ = 1.5f;
};

