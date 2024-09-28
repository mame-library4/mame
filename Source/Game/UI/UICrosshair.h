#pragma once
#include "UI.h"

class UICrosshair : public UI
{
public:
    UICrosshair();
    ~UICrosshair() override {}

    void Update(const float& elapsedTime)   override; // �X�V
    void Render()                           override; // �`��
    void DrawDebug()                        override; // ImGui�p

private:
    void UpdateSpritePosition();
    void UpdateSpriteSizeAndColor(const float& elapsedTime);
    
    [[nodiscard]] const bool JudgementEnemyAlive(); // �G(�^�[�Q�b�g)�������Ă��邩����
    [[nodiscard]] const bool JudgementDraw();       // �`�悷�邩����

private:
    std::unique_ptr<Sprite> centerDot_;

    float outerLineTimer_ = 0.0f;
    float outerLineTotalFrame_ = 0.2f;

    DirectX::XMFLOAT3 targetJointPosition_ = {};

    bool isTargetEnemyAlive_    = false; // �G(�^�[�Q�b�g)�������Ă��邩
    bool isDraw_                = false; // �`�悷�邩
    bool isSpriteSizeUpdated_   = false; // �X�V��������p
};

