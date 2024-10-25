#include "UIGuardGauge.h"
#include "Character/Player/PlayerManager.h"
#include "Easing.h"

// ----- コンストラクタ -----
UIGuardGauge::UIGuardGauge()
    : UI(UIManager::UIType::UIGuardGauge, L"./Resources/Image/White.png", "UIGuardGauge")
{
    // ガードゲージ(青色)
    SetSpriteName("GuardGauge");
    GetTransform()->SetPosition(50.0f, 90.0f);
    GetTransform()->SetSize(maxGuardGaugeSizeX, 7.0f);
    GetTransform()->SetColor(0.4f, 0.4f, 1.0f, 1.0f);

    // ガードゲージの枠(黒色)の設定
    guardGaugeFrame_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    guardGaugeFrame_->SetName("GuardGaugeFrame");
    guardGaugeFrame_->GetTransform()->SetPosition(50.0f, 90.0f);
    guardGaugeFrame_->GetTransform()->SetSize(maxGuardGaugeSizeX, 7.0f);
    guardGaugeFrame_->GetTransform()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);

    // ガードゲージが少ない警告(赤色)の設定
    guardGaugeWarning_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    guardGaugeWarning_->SetName("GuardGaugeWarning");
    guardGaugeWarning_->GetTransform()->SetPosition(50.0f, 90.0f);
    guardGaugeWarning_->GetTransform()->SetSize(maxGuardGaugeSizeX, 7.0f);
    guardGaugeWarning_->GetTransform()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);

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

    
    // ------------------------------------------------------------
    //  ガードが壊れた場合、ゲージをクレースケールに。赤点滅もさせる
    // ------------------------------------------------------------
    const bool isGuardGaugeDepleted = PlayerManager::Instance().GetPlayer()->GetIsGuardGaugeDepleted();
    if (isGuardGaugeDepleted)
    {
        warningFlashTimer_ += elapsedTime;
        if (warningFlashTimer_ >= warningFlashTime_)
        {
            isFadingIn_ = isFadingIn_ ? false : true;
            warningFlashTimer_ = 0.0f;
        }

        float alpha = 0.0f;
        if (isFadingIn_)
        {
            alpha = Easing::InSine(warningFlashTimer_, warningFlashTime_, 1.0f, 0.0f);
        }
        else
        {
            alpha = Easing::InSine(warningFlashTimer_, warningFlashTime_, 0.0f, 1.0f);
        }
        guardGaugeWarning_->GetTransform()->SetColorA(alpha);
    }
    else
    {
        guardGaugeWarning_->GetTransform()->SetColorA(0.0f);
    }


    // ------------------------------------------------------------
    //  ガードゲージを使い切った時に全回復するまでグレースケールにする
    // ------------------------------------------------------------
    if (isGuardGaugeDepleted)
    {
        GetTransform()->SetColor(0.43f, 0.43f, 0.6f, 1.0f);
    }
    else
    {
        GetTransform()->SetColor(0.4f, 0.4f, 1.0f, 1.0f);
    }

}

// ----- 描画 -----
void UIGuardGauge::Render()
{
    // 全てのUIが生成されていない
    if (isAllUICreated == false) return;
    // 描画しない
    if (GetIsDraw() == false) return;

    guardGaugeFrame_->Render();

    guardGaugeWarning_->Render();

    UI::Render();
}

// ----- ImGui用 -----
void UIGuardGauge::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        UI::DrawDebug();

        guardGaugeWarning_->DrawDebug();

        guardGaugeFrame_->DrawDebug();

        ImGui::TreePop();
    }
}
