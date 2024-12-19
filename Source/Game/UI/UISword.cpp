#include "UISword.h"
#include "Character/Player/PlayerManager.h"

// ----- コンストラクタ -----
UISword::UISword()
    : UI(UIManager::UIType::UISword, L"./Resources/Image/UI/Sword.png", "UISword")
{
    SetSpriteName("Sword");
    GetTransform()->SetPosition(60.0f, 120.0f);

    swordFrame_ = std::make_unique<Sprite>(L"./Resources/Image/UI/SwordFrame.png");
    swordFrame_->SetName("SwordFrame");
    swordFrame_->GetTransform()->SetPosition(60.0f, 120.0f);
    swordFrame_->GetTransform()->SetColor(0.65f, 0.0f, 0.0f, 1.0f);

    swordFrameBlack_ = std::make_unique<Sprite>(L"./Resources/Image/UI/SwordFrame.png");
    swordFrameBlack_->SetName("SwordFrameBlack");
    swordFrameBlack_->GetTransform()->SetPosition(60.0f, 120.0f);    
    swordFrameBlack_->GetTransform()->SetColor(0.2f, 0.0f, 0.0f, 1.0f);

    // 全てのUIが生成された
    isAllUICreated = true;
}

// ----- 更新 -----
void UISword::Update(const float& elapsedTime)
{
    const float swordSpirit = PlayerManager::Instance().GetPlayer()->GetSwordSpirit();
    const float maxSizeX = 160.0f;

    isSwordFrameActive_ = true;

    if (swordSpirit == 0.0f)
    {
        isSwordFrameActive_ = false;
    }
    else
    {
        const float sizeX = maxSizeX * swordSpirit;

        swordFrame_->GetTransform()->SetSizeX(sizeX);
        swordFrame_->GetTransform()->SetTexSizeX(sizeX);
    }

    // フェードアウト
    if (isFadeOut_)
    {
        const float alpha = XMFloatLerp(1.0f, 0.0f, fadeOutTimer_);
        swordFrameBlack_->GetTransform()->SetColorA(alpha);
        swordFrame_->GetTransform()->SetColorA(alpha);
    }

    UI::Update(elapsedTime);
}

// ----- 描画 -----
void UISword::Render()
{
    // 全てのUIが生成されていない
    if (isAllUICreated == false) return;
    // 描画しない
    if (GetIsDraw() == false) return;

    UI::Render();

    if (isSwordFrameActive_)
    {
        swordFrameBlack_->Render();
        swordFrame_->Render();
    }
}

// ----- ImGui用 -----
void UISword::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        UI::DrawDebug();
        swordFrame_->DrawDebug();
        swordFrameBlack_->DrawDebug();

        ImGui::TreePop();
    }
}
