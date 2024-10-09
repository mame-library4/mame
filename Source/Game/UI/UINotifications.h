#pragma once
#include "UI.h"

class UINotifications : public UI
{
public:
    UINotifications(const UIManager::UIType& type, const wchar_t* filename = nullptr, const std::string& name = "UI");
    ~UINotifications() override {}

    void Update(const float& elapsedTime)   override; // 更新
    void Render()                           override; // 描画
    void DrawDebug()                        override; // ImGui用

private:
    float lifeTimer_ = 10.0f; // 生存時間

    float fadeInStartPositionX_ = 1280.0f;
    float fadeInEndPositionX_ = 1030.0f;

    float fadeInTimer_ = 0.0f;   // フェイドインタイマー
    float fadeInSpeed_ = 10.0f;  // フェイドイン速度
    bool  isFadingIn_ = true;   // フェイドインする

    float fadeOutTimer_ = 0.0f;  // フェイドアウトタイマー
    float fadeOutSpeed_ = 5.0f;  // フェイドアウト速度
    bool  isFadingOut_ = false; // フェイドアウトする

    DirectX::XMFLOAT2 oldPostiion_ = {};
};

