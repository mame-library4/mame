#pragma once
#include "UI.h"

class UIFlashOut : public UI
{
public:
    UIFlashOut();
    ~UIFlashOut() override {}

    void Update(const float& elapsedTime)   override; // �X�V
    void Render()                           override; // �`��
    void DrawDebug()                        override; // ImGui�p

private:
    float flashOutFrameCounter_ = 0.0f;
    float flashOutSpeed_ = 5.0f;
};

