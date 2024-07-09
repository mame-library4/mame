#include "UINumber.h"

// ----- コンストラクタ -----
UINumber::UINumber(const int& num, const DirectX::XMFLOAT2& pos) :
    UI(),
    num_(num), hitPos_(pos)
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

    // どの数字を使用するか設定＆位置設定
    SetSpriteNumbers();
}

void UINumber::Update()
{
}

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
void UINumber::SetSpriteNumbers()
{
    const int texSizeX = numSprite_[0]->GetTransform()->GetTexSizeX();

    if (digit_ == 1)
    {
        // 位置設定
        numSprite_[0]->GetTransform()->SetPosition(hitPos_);

        // テクスチャ座標設定
        numSprite_[0]->GetTransform()->SetTexPosX(num_ % 10 * texSizeX);
    }
    else if (digit_ == 2)
    {
        // 位置設定
        numSprite_[0]->GetTransform()->SetPosition(hitPos_.x - numSpriteSize_ * 0.4f, hitPos_.y);
        numSprite_[1]->GetTransform()->SetPosition(hitPos_.x + numSpriteSize_ * 0.4f, hitPos_.y);

        // テクスチャ座標設定
        numSprite_[0]->GetTransform()->SetTexPosX(num_ / 10 % 10 * texSizeX);
        numSprite_[1]->GetTransform()->SetTexPosX(num_ % 10 * texSizeX);
    }
    else if (digit_ == 3)
    {
        // テクスチャ座標設定
        numSprite_[0]->GetTransform()->SetTexPosX(num_ / 100 % 10 * texSizeX);
        numSprite_[1]->GetTransform()->SetTexPosX(num_ / 10 % 10 * texSizeX);
        numSprite_[2]->GetTransform()->SetTexPosX(num_ % 10 * texSizeX);
    }
    else if (digit_ == 4)
    {
        // テクスチャ座標設定
        numSprite_[0]->GetTransform()->SetTexPosX(num_ / 1000 % 10 * texSizeX);
        numSprite_[1]->GetTransform()->SetTexPosX(num_ / 100 % 10 * texSizeX);
        numSprite_[2]->GetTransform()->SetTexPosX(num_ / 10 % 10 * texSizeX);
        numSprite_[3]->GetTransform()->SetTexPosX(num_ % 10 * texSizeX);
    }
}
