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

    float maxStaminaSizeX_ = 450.0f;

};

