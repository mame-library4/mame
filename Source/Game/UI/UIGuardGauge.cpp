#include "UIGuardGauge.h"
#include "Character/Player/PlayerManager.h"
#include "Easing.h"

// ----- コンストラクタ -----
UIGuardGauge::UIGuardGauge()
    : UI(UIManager::UIType::UIGuardGauge, L"./Resources/Image/White.png", "UIGuardGauge")
{
    // ガードゲージアイコン
    guardGaugeIcon_ = std::make_unique<Sprite>(L"./Resources/Image/UI/Guard.png");
    guardGaugeIcon_->SetName("GuardGaugeIcon");
    guardGaugeIcon_->GetTransform()->SetPosition(20.0f, 84.0f);
    guardGaugeIcon_->GetTransform()->SetSize(32.0f);
    guardGaugeIcon_->GetTransform()->SetColor(0.86f, 0.86f, 0.86f);

    const DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(0.27f, 0.27f, 0.59f);

    guardGaugeRhombus_ = std::make_unique<Sprite>(L"./Resources/Image/white.png");
    guardGaugeRhombus_->SetName("GuardGaugeRhombus");
    guardGaugeRhombus_->GetTransform()->SetPosition(60.0f, 95.0f);
    guardGaugeRhombus_->GetTransform()->SetSize(9.0f);
    guardGaugeRhombus_->GetTransform()->SetAngle(45.0f);
    guardGaugeRhombus_->GetTransform()->SetColor(color);

    const DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(68.0f, 98.0f);
    const float sizeY = 5.0f;

    // ガードゲージ(青色)
    SetSpriteName("GuardGauge");
    GetTransform()->SetPosition(position);
    GetTransform()->SetSize(maxGuardGaugeSizeX, sizeY);
    GetTransform()->SetColor(color);

    // ガードゲージの枠(黒色)の設定
    guardGaugeFrame_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    guardGaugeFrame_->SetName("GuardGaugeFrame");
    guardGaugeFrame_->GetTransform()->SetPosition(68.0f, 96.0f);
    guardGaugeFrame_->GetTransform()->SetSize(302.0f, 9.0f);
    guardGaugeFrame_->GetTransform()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);

    guardGaugeRhombusFrame_ = std::make_unique<Sprite>(L"./Resources/Image/white.png");
    guardGaugeRhombusFrame_->SetName("GuardGaugeRhombusFrame");
    guardGaugeRhombusFrame_->GetTransform()->SetPosition(58.0f, 93.0f);
    guardGaugeRhombusFrame_->GetTransform()->SetSize(13.0f);
    guardGaugeRhombusFrame_->GetTransform()->SetAngle(45.0f);
    guardGaugeRhombusFrame_->GetTransform()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);

    // ガードゲージが少ない警告(赤色)の設定
    guardGaugeWarning_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    guardGaugeWarning_->SetName("GuardGaugeWarning");
    guardGaugeWarning_->GetTransform()->SetPosition(position);
    guardGaugeWarning_->GetTransform()->SetSize(maxGuardGaugeSizeX, sizeY);
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
#if 0
    if (isGuardGaugeDepleted)
    {
        GetTransform()->SetColor(0.1f, 0.1f, 0.23f, 1.0f);
    }
    else
    {
        GetTransform()->SetColor(0.2f, 0.2f, 0.47f, 1.0f);
    }
#endif
}

// ----- 描画 -----
void UIGuardGauge::Render()
{
    // 全てのUIが生成されていない
    if (isAllUICreated == false) return;
    // 描画しない
    if (GetIsDraw() == false) return;

    guardGaugeFrame_->Render();
    guardGaugeRhombusFrame_->Render();

    guardGaugeWarning_->Render();

    UI::Render();
    guardGaugeRhombus_->Render();
    
    guardGaugeIcon_->Render();
}

// ----- ImGui用 -----
void UIGuardGauge::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        UI::DrawDebug();

        guardGaugeWarning_->DrawDebug();

        guardGaugeFrame_->DrawDebug();
        guardGaugeRhombusFrame_->DrawDebug();

        guardGaugeIcon_->DrawDebug();
        guardGaugeRhombus_->DrawDebug();

        ImGui::TreePop();
    }
}
