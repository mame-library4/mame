#pragma once
#include "UI.h"

class UIActionGuide : public UI
{
public:
    UIActionGuide();
    ~UIActionGuide() override {}

    void Update(const float& elapsedTime)   override; // çXêV
    void Render()                           override; // ï`âÊ
    void DrawDebug()                        override; // ImGuióp
};

