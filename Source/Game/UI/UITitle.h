#pragma once
#include "UI.h"

class UITitle : public UI
{
public:
    UITitle();
    ~UITitle() override {}

    void Update(const float& elapsedTime)   override; // �X�V
    void Render()                           override; // �`��
    void DrawDebug()                        override; // ImGui�p

public:
    void SetIsDrawAllUI(const bool& flag);
    void SetIsDrawTitleLogo(const bool& flag) { UI::SetIsDraw(flag); }

private:
    

};

