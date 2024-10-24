#include "UIGuardGauge.h"
#include "Character/Player/PlayerManager.h"

// ----- �R���X�g���N�^ -----
UIGuardGauge::UIGuardGauge()
    : UI(UIManager::UIType::UIGuardGauge, L"./Resources/Image/White.png", "UIGuardGauge")
{
    // �K�[�h�Q�[�W�̘g(���F)�̐ݒ�
    guardGaugeFrame_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    guardGaugeFrame_->SetName("GuardGaugeFrame");
    guardGaugeFrame_->GetTransform()->SetPosition(50.0f, 90.0f);
    guardGaugeFrame_->GetTransform()->SetSize(maxGuardGaugeSizeX, 7.0f);
    guardGaugeFrame_->GetTransform()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);

    SetSpriteName("GuardGauge");
    GetTransform()->SetPosition(50.0f, 90.0f);
    GetTransform()->SetSize(maxGuardGaugeSizeX, 7.0f);
    GetTransform()->SetColor(0.4f, 0.4f, 1.0f, 1.0f);

    // �S�Ă�UI���������ꂽ
    isAllUICreated = true;
}

// ----- �X�V -----
void UIGuardGauge::Update(const float& elapsedTime)
{
    // �S�Ă�UI����������Ă��Ȃ�    
    if (isAllUICreated == false) return;

    const float guardGauge = PlayerManager::Instance().GetPlayer()->GetGuardGauge();
    const float maxGuardGauge = PlayerManager::Instance().GetPlayer()->GetMaxGuardGauge();

    // ���݂̃K�[�h�Q�[�W��S�̂��猩���������o�� 0~1 �̊�
    float currentGuardGauge = guardGauge / maxGuardGauge;

    const float guardGaugeSizeX = max(maxGuardGaugeSizeX * currentGuardGauge, 0.0f);

    GetTransform()->SetSizeX(guardGaugeSizeX);
}

// ----- �`�� -----
void UIGuardGauge::Render()
{
    // �S�Ă�UI����������Ă��Ȃ�
    if (isAllUICreated == false) return;

    guardGaugeFrame_->Render();

    UI::Render();
}

// ----- ImGui�p -----
void UIGuardGauge::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        UI::DrawDebug();

        guardGaugeFrame_->DrawDebug();

        ImGui::TreePop();
    }
}
