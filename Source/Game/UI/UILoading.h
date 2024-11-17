#pragma once
#include "UI.h"

class UILoading : public UI
{
public:
    UILoading();
    ~UILoading() override {}

    void Update(const float& elapsedTime)   override; // çXêV
    void Render()                           override; // ï`âÊ
    void DrawDebug()                        override; // ImGuióp

};

