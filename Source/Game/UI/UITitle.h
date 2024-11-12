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
        GameStart,      // |
        Option,         // |
        Quit,           // |
                        // Å´

        Max,
    };

public:
    void SetIsDrawAllUI(const bool& flag);
    void SetIsDraw(const Type& type, const bool& flag);

private:
    std::unique_ptr<Sprite> sprite_[static_cast<int>(Type::Max)];

};

