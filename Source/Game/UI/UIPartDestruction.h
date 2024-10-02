#pragma once
#include "UI.h"

class UIPartDestruction : public UI
{
public:
    UIPartDestruction();
    ~UIPartDestruction() override {}

    void Update(const float& elapsedTime)   override; // 更新
    void Render()                           override; // 描画
    void DrawDebug()                        override; // ImGui用

private:
    float lifeTimer_    = 10.0f; // 生存時間
    
    float fadeInTimer_ = 0.0f;   // フェイドインタイマー
    float fadeInSpeed_ = 10.0f;  // フェイドイン速度
    bool  isFadingIn_  = true;   // フェイドインする
    
    float fadeOutTimer_ = 0.0f;  // フェイドアウトタイマー
    float fadeOutSpeed_ = 5.0f;  // フェイドアウト速度
    bool  isFadingOut_  = false; // フェイドアウトする

    DirectX::XMFLOAT2 oldPostiion_ = {};
};

