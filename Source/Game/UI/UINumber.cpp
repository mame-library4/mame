#include "UINumber.h"
#include "UIManager.h"
#include "Easing.h"

// ----- コンストラクタ -----
UINumber::UINumber(const int& num, const DirectX::XMFLOAT3& hitPos) :
    UI(),
    num_(num), hitPos_(hitPos),
    addPos_(DirectX::XMFLOAT3((rand() % 10 - 5) / 10.0f, (rand() % 10 - 5) / 10.0f, (rand() % 10 - 5) / 10.0f))
{
    // 桁数を求める
    if(num >= 1000)      digit_ = 4;
    else if (num >= 100) digit_ = 3;
    else if (num >= 10)  digit_ = 2;
    else if (num >= 0)   digit_ = 1;

    // 初期設定
    for (int spriteIndex = 0; spriteIndex < digit_; ++spriteIndex)
    {
        numSprite_[spriteIndex] = std::make_unique<Sprite>(L"./Resources/Image/Number/numbers.png");
        numSprite_[spriteIndex]->GetTransform()->SetTexSize(256.0f);
        numSprite_[spriteIndex]->GetTransform()->SetSize(numSpriteSize_);
    }

    // どの数字を使用するか設定
    SetNumSproteNumbers();

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
    const DirectX::XMFLOAT2 hitPos = Sprite::ConvertToScreenPos(hitPos_ + addPos_);

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