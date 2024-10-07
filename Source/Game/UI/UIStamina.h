#pragma once
#include "UI.h"

class UIStamina : public UI
{
public:
    UIStamina();
    ~UIStamina() override {}

    void Update(const float& elapsedTime)   override; // �X�V
    void Render()                           override; // �`��
    void DrawDebug()                        override; // ImGui�p

private:
    std::unique_ptr<Sprite> staminaFrame_;

    float maxStaminaSizeX_ = 450.0f;

};

