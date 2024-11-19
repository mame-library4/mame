#pragma once
#include "UI.h"

class UIFader : public UI
{
public:
    UIFader(const bool& autoDelete = true, const float& fadeSpeed = 1.0f);
    ~UIFader() override {}

    void Update(const float& elapsedTime)   override; // 更新
    void Render()                           override; // 描画
    void DrawDebug()                        override; // ImGui用

public:
    [[nodiscard]] const bool GetIsFadeComplete() const { return isFadeComplete_; }
    void SetFadeSpeed(const float& speed) { fadeSpeed_ = speed; }

private:
    bool    isAutoDelete_   = false; // マネージャーからの削除をクラス内でやるか
    bool    isFadeComplete_ = false;
    float   fadeTimer_      = 0.0f;
    float   fadeSpeed_      = 1.0f;
};

