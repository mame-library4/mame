#include "UINumber.h"
#include "Easing.h"

// ----- コンストラクタ -----
UINumber::UINumber(const int& num, const DirectX::XMFLOAT3& hitPos) :
    UI(UIManager::UIType::UINumber, nullptr, "UINumber"),
    num_(num), hitPos_(hitPos)
{
    // ランダムでaddPosを決める
    addPos_.x = (rand() % 10 - 5) / 10.0f;
    addPos_.y = (rand() % 10 - 5) / 10.0f;
    addPos_.z = (rand() % 10 - 5) / 10.0f;

    // 桁数を求める
    if(num >= 1000)      digit_ = 4;
    else if (num >= 100) digit_ = 3;
    else if (num >= 10)  digit_ = 2;
    else if (num >= 0)   digit_ = 1;

    // 初期設定
    for (int spriteIndex = 0; spriteIndex < digit_; ++spriteIndex)
    {
        numSprite_[spriteIndex] = std::make_unique<Sprite>(L"./Resources/Image/UI/Number/numbers.png");
        numSprite_[spriteIndex]->GetTransform()->SetTexSize(256.0f);
        numSprite_[spriteIndex]->GetTransform()->SetSize(numSpriteSize_);
    }

    // どの数字を使用するか設定
    SetNumSproteNumbers();

    // 描画フラグを立てる
    SetIsDraw();

    // 変数初期化    
    lifeTimer_ = 0.5f;
    easingTimer_ = 0.0f;
}

// ----- 更新 -----
void UINumber::Update(const float& elapsedTime)
{
    // 位置設定
    SetNumSpritePosition();

    lifeTimer_ -= elapsedTime;
    if (lifeTimer_ < 0.0f)
    {
        const float totalFrame = 0.5f;
        const float addY = Easing::InQuint(easingTimer_, totalFrame, -40.0f, 0.0f);
        const float alpha = Easing::InSine(easingTimer_, totalFrame, 0.0f, 1.0f);
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

        // 生存時間が０になったら自身を消す
        if (easingTimer_ >= totalFrame) UIManager::Instance().Remove(this);

        for (int spriteIndex = 0; spriteIndex < digit_; ++spriteIndex)
        {
            numSprite_[spriteIndex]->GetTransform()->AddPositionY(addY);
            numSprite_[spriteIndex]->GetTransform()->SetColorA(alpha);
        }
    }
}

// ----- 描画 -----
void UINumber::Render()
{
    for (int spriteIndex = 0; spriteIndex < digit_; ++spriteIndex)
    {
        numSprite_[spriteIndex]->Render();
    }
}

void UINumber::DrawDebug()
{
#ifdef _DEBUG
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat("LifeTimer", &lifeTimer_);

        ImGui::TreePop();
    }
#endif
}

// ----- 数字を設定する -----
void UINumber::SetNumSproteNumbers()
{
    const int texSizeX = numSprite_[0]->GetTransform()->GetTexSizeX();
    if (digit_ == 1)
    {
        numSprite_[0]->GetTransform()->SetTexPosX(num_ % 10 * texSizeX);
    }
    else if (digit_ == 2)
    {
        numSprite_[0]->GetTransform()->SetTexPosX(num_ / 10 % 10 * texSizeX);
        numSprite_[1]->GetTransform()->SetTexPosX(num_ % 10 * texSizeX);
    }
    else if (digit_ == 3)
    {
        numSprite_[0]->GetTransform()->SetTexPosX(num_ / 100 % 10 * texSizeX);
        numSprite_[1]->GetTransform()->SetTexPosX(num_ / 10 % 10 * texSizeX);
        numSprite_[2]->GetTransform()->SetTexPosX(num_ % 10 * texSizeX);
    }
    else if (digit_ == 4)
    {
        numSprite_[0]->GetTransform()->SetTexPosX(num_ / 1000 % 10 * texSizeX);
        numSprite_[1]->GetTransform()->SetTexPosX(num_ / 100 % 10 * texSizeX);
        numSprite_[2]->GetTransform()->SetTexPosX(num_ / 10 % 10 * texSizeX);
        numSprite_[3]->GetTransform()->SetTexPosX(num_ % 10 * texSizeX);
    }
}

// ----- 位置を設定する -----
void UINumber::SetNumSpritePosition()
{
    // ------------------------------------------------------------
    //  hitPosにaddPosを足すことで、同じところに出ずにいい感じになる。
    //  足した結果Y値がマイナスの場合、0.0fに戻す。
    // ------------------------------------------------------------
    DirectX::XMFLOAT3 pos = hitPos_ + addPos_;
    pos.y = max(0.0f, pos.y);
    const DirectX::XMFLOAT2 hitPos = Sprite::ConvertToScreenPos(pos);

    // -----------------------------------
    //  桁数によって出す位置を変える
    // -----------------------------------
    if (digit_ == 1)
    {
        numSprite_[0]->GetTransform()->SetPosition(hitPos);
    }
    else if (digit_ == 2)
    {
        numSprite_[0]->GetTransform()->SetPosition(hitPos.x - numSpriteSize_ * 0.4f, hitPos.y);
        numSprite_[1]->GetTransform()->SetPosition(hitPos.x + numSpriteSize_ * 0.4f, hitPos.y);
    }
    else if (digit_ == 3)
    {
        numSprite_[0]->GetTransform()->SetPosition(hitPos.x - numSpriteSize_ * 0.8f, hitPos.y);
        numSprite_[1]->GetTransform()->SetPosition(hitPos);
        numSprite_[2]->GetTransform()->SetPosition(hitPos.x + numSpriteSize_ * 0.8f, hitPos.y);
    }
    else if (digit_ == 4)
    {
        const float left = hitPos.x - numSpriteSize_ * 0.4f;
        numSprite_[0]->GetTransform()->SetPosition(left - numSpriteSize_ * 0.8f, hitPos_.y);
        numSprite_[1]->GetTransform()->SetPosition(left, hitPos_.y);

        const float right = hitPos.x + numSpriteSize_ * 0.4f;
        numSprite_[2]->GetTransform()->SetPosition(right, hitPos_.y);
        numSprite_[3]->GetTransform()->SetPosition(right + numSpriteSize_ * 0.8f, hitPos_.y);
    }
}