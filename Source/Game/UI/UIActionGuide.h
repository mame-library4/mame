#pragma once
#include "UI.h"

class UIActionGuide : public UI
{
public:
    UIActionGuide();
    ~UIActionGuide() override {}

    void Update(const float& elapsedTime)   override; // �X�V
    void Render()                           override; // �`��
    void DrawDebug()                        override; // ImGui�p
};

