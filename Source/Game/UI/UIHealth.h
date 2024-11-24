#pragma once
#include "UI.h"

class UIHealth : public UI
{
public:
    UIHealth();
    ~UIHealth() override {}

    void Update(const float& elapsedTime)   override; // 更新
    void Render()                           override; // 描画
    void DrawDebug()                        override; // ImGui用

private:
    void Initialize(); // 初期化

    void ApplyDamageEffect();                           // ダメージ効果を適用する
    void UpdateVibration(const float& elapsedTime);     // 振動更新
    void UpdateAutoRecovery(const float& elapsedTime);  // 自動回復
    void UpdateSpriteSize();

    void SetUIPosition(const DirectX::XMFLOAT2& position);

private:
    bool isAllUICreated = false;

    std::unique_ptr<Sprite> healthRhombus_;
    std::unique_ptr<Sprite> healthRhombusFrame_;
    std::unique_ptr<Sprite> healthFrame_;
    std::unique_ptr<Sprite> autoRecoveryBar_;
    std::unique_ptr<Sprite> healthIcon_;

    float oldHealth_        = 0.0f;
    float autoRecoveryHealth_ = 0.0f;

    // ---------- 振動 ----------
    float             vibrationVolume_ = 0.0f;
    float             vibrationTime_   = 0.0f;
    float             vibrationTimer_  = 0.0f;
    bool              isVibration_     = false;

    // ----- 初期位置保存用 -----
    DirectX::XMFLOAT2 healthPosition_ = {};
    DirectX::XMFLOAT2 healthFramePosition_ = {};
    DirectX::XMFLOAT2 healthRhombusPosition_ = {};
    DirectX::XMFLOAT2 healthRhombusFramePosition_ = {};    

    float frameAlpha_ = 0.4f;

    float maxHealthSizeX_ = 450.0f;

    float healSpeed_ = 1.0f; // 回復速度
};

