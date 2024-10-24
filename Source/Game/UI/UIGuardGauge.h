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

    bool isAllUICreated = false; // 全てのリソースが生成されたか
};

