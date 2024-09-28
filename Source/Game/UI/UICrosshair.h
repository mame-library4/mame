#pragma once
#include "UI.h"

class UICrosshair : public UI
{
public:
    UICrosshair();
    ~UICrosshair() override {}

    void Update(const float& elapsedTime)   override; // 更新
    void Render()                           override; // 描画
    void DrawDebug()                        override; // ImGui用

private:
    void UpdateSpritePosition();
    void UpdateSpriteSizeAndColor(const float& elapsedTime);
    
    [[nodiscard]] const bool JudgementEnemyAlive(); // 敵(ターゲット)が生きているか判定
    [[nodiscard]] const bool JudgementDraw();       // 描画するか判定

private:
    std::unique_ptr<Sprite> centerDot_;

    float outerLineTimer_ = 0.0f;
    float outerLineTotalFrame_ = 0.2f;

    DirectX::XMFLOAT3 targetJointPosition_ = {};

    bool isTargetEnemyAlive_    = false; // 敵(ターゲット)が生きているか
    bool isDraw_                = false; // 描画するか
    bool isSpriteSizeUpdated_   = false; // 更新処理制御用
};

