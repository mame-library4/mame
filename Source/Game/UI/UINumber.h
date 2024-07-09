#pragma once
#include "UI.h"

class UINumber : public UI
{
public:
    UINumber(const int& num, const DirectX::XMFLOAT2& pos);
    ~UINumber() override {}

    void Update()       override;
    void Render()       override;
    void DrawDebug()    override;

private:
    void SetSpriteNumbers();

private:
    static const int        MaxDigit_ = 4;  // ���̍ő�
    int                     digit_    = 0;  // ��
    const int               num_;           // ����
    const DirectX::XMFLOAT2 hitPos_;        // ���������Ƃ���̈ʒu
    const float             numSpriteSize_ = 32.0f; // �摜�T�C�Y
    std::unique_ptr<Sprite> numSprite_[MaxDigit_];
};

