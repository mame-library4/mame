#include "UINumber.h"
#include "Easing.h"

// ----- �R���X�g���N�^ -----
UINumber::UINumber(const int& num, const DirectX::XMFLOAT3& hitPos) :
    UI(UIManager::UIType::UINumber, nullptr, "UINumber"),
    num_(num), hitPos_(hitPos)
{
    // �����_����addPos�����߂�
    addPos_.x = (rand() % 10 - 5) / 10.0f;
    addPos_.y = (rand() % 10 - 5) / 10.0f;
    addPos_.z = (rand() % 10 - 5) / 10.0f;

    // ���������߂�
    if(num >= 1000)      digit_ = 4;
    else if (num >= 100) digit_ = 3;
    else if (num >= 10)  digit_ = 2;
    else if (num >= 0)   digit_ = 1;

    // �����ݒ�
    for (int spriteIndex = 0; spriteIndex < digit_; ++spriteIndex)
    {
        numSprite_[spriteIndex] = std::make_unique<Sprite>(L"./Resources/Image/UI/Number/numbers.png");
        numSprite_[spriteIndex]->GetTransform()->SetTexSize(256.0f);
        numSprite_[spriteIndex]->GetTransform()->SetSize(numSpriteSize_);
    }

    // �ǂ̐������g�p���邩�ݒ�
    SetNumSproteNumbers();

    // �`��t���O�𗧂Ă�
    SetIsDraw();

    // �ϐ�������    
    lifeTimer_ = 0.5f;
    easingTimer_ = 0.0f;
}

// ----- �X�V -----
void UINumber::Update(const float& elapsedTime)
{
    // �ʒu�ݒ�
    SetNumSpritePosition();

    lifeTimer_ -= elapsedTime;
    if (lifeTimer_ < 0.0f)
    {
        const float totalFrame = 0.5f;
        const float addY = Easing::InQuint(easingTimer_, totalFrame, -40.0f, 0.0f);
        const float alpha = Easing::InSine(easingTimer_, totalFrame, 0.0f, 1.0f);
        easingTimer_ += elapsedTime;
        easingTimer_ = min(easingTimer_, totalFrame);

        // �������Ԃ��O�ɂȂ����玩�g������
        if (easingTimer_ >= totalFrame) UIManager::Instance().Remove(this);

        for (int spriteIndex = 0; spriteIndex < digit_; ++spriteIndex)
        {
            numSprite_[spriteIndex]->GetTransform()->AddPositionY(addY);
            numSprite_[spriteIndex]->GetTransform()->SetColorA(alpha);
        }
    }
}

// ----- �`�� -----
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

// ----- ������ݒ肷�� -----
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

// ----- �ʒu��ݒ肷�� -----
void UINumber::SetNumSpritePosition()
{
    // ------------------------------------------------------------
    //  hitPos��addPos�𑫂����ƂŁA�����Ƃ���ɏo���ɂ��������ɂȂ�B
    //  ����������Y�l���}�C�i�X�̏ꍇ�A0.0f�ɖ߂��B
    // ------------------------------------------------------------
    DirectX::XMFLOAT3 pos = hitPos_ + addPos_;
    pos.y = max(0.0f, pos.y);
    const DirectX::XMFLOAT2 hitPos = Sprite::ConvertToScreenPos(pos);

    // -----------------------------------
    //  �����ɂ���ďo���ʒu��ς���
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