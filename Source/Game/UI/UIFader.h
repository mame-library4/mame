#pragma once
#include "UI.h"

class UIFader : public UI
{
public:
    UIFader();
    ~UIFader() override {}

    void Update(const float& elapsedTime)   override; // XV
    void Render()                           override; // •`‰æ
    void DrawDebug()                        override; // ImGui—p

private:
    float fadeTimer_ = 0.0f;

};

