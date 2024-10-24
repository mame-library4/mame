#pragma once
#include "UI.h"

class UIGuardGauge : public UI
{
public:
    UIGuardGauge();
    ~UIGuardGauge() override {}

    void Update(const float& elapsedTime)   override; // �X�V
    void Render()                           override; // �`��
    void DrawDebug()                        override; // ImGui�p

private:
    std::unique_ptr<Sprite> guardGaugeFrame_;

    bool isAllUICreated = false; // �S�Ẵ��\�[�X���������ꂽ��
};

