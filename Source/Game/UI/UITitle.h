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

public:
    void SetIsDrawAllUI(const bool& flag);
    void SetIsDrawTitleLogo(const bool& flag) { UI::SetIsDraw(flag); }

private:
    

};

