#include "UINumber.h"

// ----- �R���X�g���N�^ -----
UINumber::UINumber(const int& num, const DirectX::XMFLOAT2& pos) :
    UI(),
    num_(num), hitPos_(pos)
{
    // ���������߂�
    if(num >= 1000)      digit_ = 4;
    else if (num >= 100) digit_ = 3;
    else if (num >= 10)  digit_ = 2;
    else if (num >= 0)   digit_ = 1;

    // �����ݒ�
    for (int spriteIndex = 0; spriteIndex < digit_; ++spriteIndex)
    {
        numSprite_[spriteIndex] = std::make_unique<Sprite>(L"./Resources/Image/Number/numbers.png");
        numSprite_[spriteIndex]->GetTransform()->SetTexSize(256.0f);
        numSprite_[spriteIndex]->GetTransform()->SetSize(numSpriteSize_);
    }

    // �ǂ̐������g�p���邩�ݒ聕�ʒu�ݒ�
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

// ----- ������ݒ肷�� -----
void UINumber::SetSpriteNumbers()
{
    const int texSizeX = numSprite_[0]->GetTransform()->GetTexSizeX();

    if (digit_ == 1)
    {
        // �ʒu�ݒ�
        numSprite_[0]->GetTransform()->SetPosition(hitPos_);

        // �e�N�X�`�����W�ݒ�
        numSprite_[0]->GetTransform()->SetTexPosX(num_ % 10 * texSizeX);
    }
    else if (digit_ == 2)
    {
        // �ʒu�ݒ�
        numSprite_[0]->GetTransform()->SetPosition(hitPos_.x - numSpriteSize_ * 0.4f, hitPos_.y);
        numSprite_[1]->GetTransform()->SetPosition(hitPos_.x + numSpriteSize_ * 0.4f, hitPos_.y);

        // �e�N�X�`�����W�ݒ�
        numSprite_[0]->GetTransform()->SetTexPosX(num_ / 10 % 10 * texSizeX);
        numSprite_[1]->GetTransform()->SetTexPosX(num_ % 10 * texSizeX);
    }
    else if (digit_ == 3)
    {
        // �e�N�X�`�����W�ݒ�
        numSprite_[0]->GetTransform()->SetTexPosX(num_ / 100 % 10 * texSizeX);
        numSprite_[1]->GetTransform()->SetTexPosX(num_ / 10 % 10 * texSizeX);
        numSprite_[2]->GetTransform()->SetTexPosX(num_ % 10 * texSizeX);
    }
    else if (digit_ == 4)
    {
        // �e�N�X�`�����W�ݒ�
        numSprite_[0]->GetTransform()->SetTexPosX(num_ / 1000 % 10 * texSizeX);
        numSprite_[1]->GetTransform()->SetTexPosX(num_ / 100 % 10 * texSizeX);
        numSprite_[2]->GetTransform()->SetTexPosX(num_ / 10 % 10 * texSizeX);
        numSprite_[3]->GetTransform()->SetTexPosX(num_ % 10 * texSizeX);
    }
}
