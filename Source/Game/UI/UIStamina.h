#pragma once
#include "UI.h"

class UIStamina : public UI
{
public:
    UIStamina();
    ~UIStamina() override {}

    void Update(const float& elapsedTime)   override; // çXêV
    void Render()                           override; // ï`âÊ
    void DrawDebug()                        override; // ImGuióp

private:
    std::unique_ptr<Sprite> staminaFrame_;
    std::unique_ptr<Sprite> staminaWarning_;

    float maxStaminaSizeX_ = 450.0f;

    float warningFlashTimer_ = 0.0f;
    float warningFlashTime_  = 0.3f;
    bool isFadingIn_ = true;
};

