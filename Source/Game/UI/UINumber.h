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
    void SetNumSproteNumbers();     // 数字を設定する
    void SetNumSpritePosition();    // 位置を設定する

private:
    static const int        MaxDigit_ = 4;  // 桁の最大
    int                     digit_    = 0;  // 桁
    const int               num_;           // 数字
    const DirectX::XMFLOAT3 hitPos_;        // 当たったところの位置
    DirectX::XMFLOAT3       addPos_;        // 散らばらせる用
    const float             numSpriteSize_ = 32.0f; // 画像サイズ
    std::unique_ptr<Sprite> numSprite_[MaxDigit_];

    float lifeTimer_    = 0.0f; // 生存時間
    float easingTimer_  = 0.0f; // easing用
};

