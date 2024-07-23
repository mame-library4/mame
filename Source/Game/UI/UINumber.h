#pragma once
#include "UI.h"

class UINumber : public UI
{
public:
    UINumber(const int& num, const DirectX::XMFLOAT3& hitPos);
    ~UINumber() override {}

    void Update(const float& elapsedTime)   override;
    void Render()                           override;

private:
    void SetNumSproteNumbers();     // ������ݒ肷��
    void SetNumSpritePosition();    // �ʒu��ݒ肷��

private:
    static const int        MaxDigit_ = 4;  // ���̍ő�
    int                     digit_    = 0;  // ��
    const int               num_;           // ����
    const DirectX::XMFLOAT3 hitPos_;        // ���������Ƃ���̈ʒu
    DirectX::XMFLOAT3       addPos_;        // �U��΂点��p
    const float             numSpriteSize_ = 32.0f; // �摜�T�C�Y
    std::unique_ptr<Sprite> numSprite_[MaxDigit_];

    float lifeTimer_    = 0.0f; // ��������
    float easingTimer_  = 0.0f; // easing�p
};

