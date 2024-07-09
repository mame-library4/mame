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
    static const int        MaxDigit_ = 4;  // 桁の最大
    int                     digit_    = 0;  // 桁
    const int               num_;           // 数字
    const DirectX::XMFLOAT2 hitPos_;        // 当たったところの位置
    const float             numSpriteSize_ = 32.0f; // 画像サイズ
    std::unique_ptr<Sprite> numSprite_[MaxDigit_];
};

