#pragma once
#include "UI.h"

class UIHealth : public UI
{
public:
    UIHealth();
    ~UIHealth() override {}

    void Update(const float& elapsedTime)   override; // �X�V
    void Render()                           override; // �`��
    void DrawDebug()                        override; // ImGui�p

private:
    void ApplyDamageEffect();                           // �_���[�W���ʂ�K�p����
    void UpdateVibration(const float& elapsedTime);     // �U���X�V
    void UpdateAutoRecovery(const float& elapsedTime);  // ������
    void UpdateSpriteSize();

    void SetUIPosition(const DirectX::XMFLOAT2& position);

private:
    bool isAllUICreated = false;

    std::unique_ptr<Sprite> healthFrame_;
    std::unique_ptr<Sprite> autoRecoveryBar_;

    float oldHealth_        = 0.0f;
    float autoRecoveryHealth_ = 0.0f;

    // ---------- �U�� ----------
    float             vibrationVolume_ = 0.0f;
    float             vibrationTime_   = 0.0f;
    float             vibrationTimer_  = 0.0f;
    bool              isVibration_     = false;

    const DirectX::XMFLOAT2 healthPosition_ = { 50.0f, 50.0f };

    float maxHealthSizeX_ = 450.0f;

    float healSpeed_ = 0.5f; // �񕜑��x
};

