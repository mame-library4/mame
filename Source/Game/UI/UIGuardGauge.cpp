#include "UIGuardGauge.h"
#include "Character/Player/PlayerManager.h"
#include "Easing.h"

// ----- コンストラクタ -----
UIGuardGauge::UIGuardGauge()
    : UI(UIManager::UIType::UIGuardGauge, L"./Resources/Image/White.png", "UIGuardGauge")
{
    guardGaugeRhombus_ = std::make_unique<Sprite>(L"./Resources/Image/white.png");
    guardGaugeWarning_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    guardGaugeFrame_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    guardGaugeRhombusFrame_ = std::make_unique<Sprite>(L"./Resources/Image/white.png");
    guardGaugeIcon_ = std::make_unique<Sprite>(L"./Resources/Image/UI/Guard.png");

    // 初期化
    Initialize();

    // 全てのUIが生成された
    isAllUICreated = true;
}

// ----- 初期化 -----
void UIGuardGauge::Initialize()
{
    const DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(68.0f, 98.0f);
    const float sizeY = 5.0f;

    // ガードゲージ(青色)
    SetSpriteName("GuardGauge");
    SetIsActiveUVScroll();
    SetNoiseTextureNum(0);
    SetScrollDirection({ 0.25f, 0.0f });
    GetTransform()->SetPosition(position);
    GetTransform()->SetSize(maxGuardGaugeSizeX, sizeY);
    GetTransform()->SetColor(0.42f, 0.42f, 1.0f);

    // ----- ◇ひし形 -----
    guardGaugeRhombus_->SetName("GuardGaugeRhombus");
    guardGaugeRhombus_->SetIsActiveUVScroll();
    guardGaugeRhombus_->SetNoiseTextureNum(1);
    guardGaugeRhombus_->SetScrollDirection({ 0.5f, 0.5f });
    guardGaugeRhombus_->GetTransform()->SetPosition(60.0f, 95.0f);
    guardGaugeRhombus_->GetTransform()->SetSize(9.0f);
    guardGaugeRhombus_->GetTransform()->SetAngle(45.0f);
    guardGaugeRhombus_->GetTransform()->SetColor(0.42f, 0.42f, 1.0f);

    // ----- 赤ゲージ -----
    guardGaugeWarning_->SetName("GuardGaugeWarning");
    guardGaugeWarning_->GetTransform()->SetPosition(position);
    guardGaugeWarning_->GetTransform()->SetSize(maxGuardGaugeSizeX, sizeY);
    guardGaugeWarning_->GetTransform()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    // ----- ガードゲージの枠(黒色) -----
    guardGaugeFrame_->SetName("GuardGaugeFrame");
    guardGaugeFrame_->GetTransform()->SetPosition(68.0f, 96.0f);
    guardGaugeFrame_->GetTransform()->SetSize(302.0f, 9.0f);
    guardGaugeFrame_->GetTransform()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);

    // ----- ◇ひし形の枠(黒色) -----
    guardGaugeRhombusFrame_->SetName("GuardGaugeRhombusFrame");
    guardGaugeRhombusFrame_->GetTransform()->SetPosition(58.0f, 93.0f);
    guardGaugeRhombusFrame_->GetTransform()->SetSize(13.0f);
    guardGaugeRhombusFrame_->GetTransform()->SetAngle(45.0f);
    guardGaugeRhombusFrame_->GetTransform()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);

    // ----- ガードアイコン -----
    guardGaugeIcon_->SetName("GuardGaugeIcon");
    guardGaugeIcon_->SetIsActiveUVScroll();
    guardGaugeIcon_->SetNoiseTextureNum(1);
    guardGaugeIcon_->SetScrollDirection({ 0.0f, 0.5f });
    guardGaugeIcon_->GetTransform()->SetPosition(20.0f, 84.0f);
    guardGaugeIcon_->GetTransform()->SetSize(32.0f);
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
    GetTransform()->SetTexSizeX(guardGaugeSizeX);

    
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

    // 更新
    UI::Update(elapsedTime);
    guardGaugeRhombus_->Update(elapsedTime);
    guardGaugeIcon_->Update(elapsedTime);
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