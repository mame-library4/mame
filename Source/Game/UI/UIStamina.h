#pragma once
#include "UI.h"

class UIStamina : public UI
{
public:
    UIStamina();
    ~UIStamina() override {}

    void Update(const float& elapsedTime)   override; // 更新
    void Render()                           override; // 描画
    void DrawDebug()                        override; // ImGui用

private:
    std::unique_ptr<Sprite> staminaFrame_;
    std::unique_ptr<Sprite> staminaWarning_;

    bool isAllUICreated = false; // 全てのリソースが生成されたか

    const float maxStaminaSizeX_ = 450.0f;

    float warningFlashTimer_ = 0.0f;
    float warningFlashTime_  = 0.3f;
    bool isFadingIn_ = true;
};

