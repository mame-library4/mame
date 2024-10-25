#pragma once
#include "UI.h"

class UIGuardGauge : public UI
{
public:
    UIGuardGauge();
    ~UIGuardGauge() override {}

    void Update(const float& elapsedTime)   override; // 更新
    void Render()                           override; // 描画
    void DrawDebug()                        override; // ImGui用

private:
    std::unique_ptr<Sprite> guardGaugeFrame_;
    std::unique_ptr<Sprite> guardGaugeWarning_;

    bool isAllUICreated = false; // 全てのリソースが生成されたか
    
    const float maxGuardGaugeSizeX = 450.0f;

    float warningFlashTimer_    = 0.0f;
    float warningFlashTime_     = 0.3f;
    bool  isFadingIn_           = true;
};

