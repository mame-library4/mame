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

private:


};

