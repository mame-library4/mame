#pragma once
#include "UI.h"

class UIPartDestruction : public UI
{
public:
    UIPartDestruction();
    ~UIPartDestruction() override {}

    void Update(const float& elapsedTime)   override; // �X�V
    void Render()                           override; // �`��
    void DrawDebug()                        override; // ImGui�p

private:
};

