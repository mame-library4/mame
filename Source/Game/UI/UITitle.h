#pragma once
#include "UI.h"

class UITitle : public UI
{
public:
    UITitle();
    ~UITitle() override {}

    void Update(const float& elapsedTime)   override; // çXêV
    void Render()                           override; // ï`âÊ
    void DrawDebug()                        override; // ImGuióp

private:


};

