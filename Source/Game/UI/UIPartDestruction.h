#pragma once
#include "UI.h"

class UIPartDestruction : public UI
{
public:
    UIPartDestruction();
    ~UIPartDestruction() override {}

    void Update(const float& elapsedTime)   override; // çXêV
    void Render()                           override; // ï`âÊ
    void DrawDebug()                        override; // ImGuióp

private:
};

