#include "UIGuardGauge.h"
#include "Character/Player/PlayerManager.h"

// ----- コンストラクタ -----
UIGuardGauge::UIGuardGauge()
    : UI(UIManager::UIType::UIGuardGauge, L"./Resources/Image/White.png", "UIGuardGauge")
{
    // ガードゲージの枠(黒色)の設定
    guardGaugeFrame_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    guardGaugeFrame_->SetName("GuardGaugeFrame");
    guardGaugeFrame_->GetTransform()->SetPosition(50.0f, 90.0f);
    guardGaugeFrame_->GetTransform()->SetSize(maxGuardGaugeSizeX, 7.0f);
    guardGaugeFrame_->GetTransform()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);

    SetSpriteName("GuardGauge");
    GetTransform()->SetPosition(50.0f, 90.0f);
    GetTransform()->SetSize(maxGuardGaugeSizeX, 7.0f);
    GetTransform()->SetColor(0.4f, 0.4f, 1.0f, 1.0f);

    // 全てのUIが生成された
    isAllUICreated = true;
}

// ----- 更新 -----
void UIGuardGauge::Update(const float& elapsedTime)
{
    // 全てのUIが生成されていない    
    if (isAllUICreated == false) return;

    const float guardGauge = PlayerManager::Instance().GetPlayer()->GetGuardGauge();
    const float maxGuardGauge = PlayerManager::Instance().GetPlayer()->GetMaxGuardGauge();

    // 現在のガードゲージを全体から見た割合を出す 0~1 の間
    float currentGuardGauge = guardGauge / maxGuardGauge;

    const float guardGaugeSizeX = max(maxGuardGaugeSizeX * currentGuardGauge, 0.0f);

    GetTransform()->SetSizeX(guardGaugeSizeX);
}

// ----- 描画 -----
void UIGuardGauge::Render()
{
    // 全てのUIが生成されていない
    if (isAllUICreated == false) return;

    guardGaugeFrame_->Render();

    UI::Render();
}

// ----- ImGui用 -----
void UIGuardGauge::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        UI::DrawDebug();

        guardGaugeFrame_->DrawDebug();

        ImGui::TreePop();
    }
}
