#pragma once
#include "UI.h"

class UISystem : public UI
{
public:
    UISystem();
    ~UISystem() override {}

    void Update(const float& elapsedTime)   override; // XV
    void Render()                           override; // •`‰æ
    void DrawDebug()                        override; // ImGui—p

private:
    void Initialize(); // ‰Šú‰»
};