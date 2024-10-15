#pragma once
#include "UI.h"

class UIFlashOut : public UI
{
public:
    UIFlashOut();
    ~UIFlashOut() override {}

    void Update(const float& elapsedTime)   override; // çXêV
    void Render()                           override; // ï`âÊ
    void DrawDebug()                        override; // ImGuióp

private:
    float flashOutFrameCounter_ = 0.0f;
    float flashOutSpeed_ = 5.0f;
};

