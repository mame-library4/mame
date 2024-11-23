#pragma once
#include "UI.h"

class UIRush : public UI
{
public:
    UIRush();
    ~UIRush() override {}

    void Update(const float& elapsedTime)   override; // XV
    void Render()                           override; // •`‰æ
    void DrawDebug()                        override; // ImGui—p

private:
    void Initialize(); // ‰Šú‰»

private:
    std::unique_ptr<Sprite> rushWord_;

    float fadeTimer_ = 0.0f;
    float fadeSpeed_ = 5.0f;
};

