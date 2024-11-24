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
    void Initialize(); // 初期化

private:
    std::unique_ptr<Sprite> guardGaugeRhombus_;
    std::unique_ptr<Sprite> guardGaugeRhombusFrame_;
    std::unique_ptr<Sprite> guardGaugeFrame_;
    std::unique_ptr<Sprite> guardGaugeWarning_;
    std::unique_ptr<Sprite> guardGaugeIcon_;

    bool isAllUICreated = false; // 全てのリソースが生成されたか
    
    const float maxGuardGaugeSizeX = 300.0f;

    float warningFlashTimer_    = 0.0f;
    float warningFlashTime_     = 0.3f;
    bool  isFadingIn_           = true;

    float frameAlpha_ = 0.4f;
};

