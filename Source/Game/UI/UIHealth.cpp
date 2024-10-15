#include "UIHealth.h"
#include "Character/Player/PlayerManager.h"
#include "Easing.h"

// ----- �R���X�g���N�^ -----
UIHealth::UIHealth()
    : UI(UIManager::UIType::UIHealth, L"./Resources/Image/white.png", "UIHealth")
{
    healthFrame_ = std::make_unique<Sprite>(L"./Resources/Image/white.png");
    healthFrame_->SetName("HealthFrame");
    healthFrame_->GetTransform()->SetPosition(50.0f, 50.0f);
    healthFrame_->GetTransform()->SetSize(maxHealthSizeX_, 10.0f);
    healthFrame_->GetTransform()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);

    autoRecoveryBar_ = std::make_unique<Sprite>(L"./Resources/Image/white.png");
    autoRecoveryBar_->SetName("AutoRepairBar");
    autoRecoveryBar_->GetTransform()->SetPosition(50.0f, 50.0f);
    autoRecoveryBar_->GetTransform()->SetSize(maxHealthSizeX_, 10.0f);
    autoRecoveryBar_->GetTransform()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    SetSpriteName("Health");
    GetTransform()->SetPosition(50.0f, 50.0f);
    GetTransform()->SetSize(maxHealthSizeX_, 10.0f);
    GetTransform()->SetColor(0.0f, 1.0f, 0.0f);

    oldHealth_ = 300.0f;
    autoRecoveryHealth_ = 300.0f;

    isAllUICreated = true;
}

// ----- �X�V -----
void UIHealth::Update(const float& elapsedTime)
{
    if (isAllUICreated == false) return;

    // �\�����Ă��Ȃ��Ƃ��͍X�V���Ȃ�
    if (GetIsDraw() == false)
    {
        oldHealth_ = PlayerManager::Instance().GetPlayer()->GetHealth();

        return;
    }

    // �_���[�W���ʂ�UI�ɔ��f������
    ApplyDamageEffect();

    // �U���X�V
    UpdateVibration(elapsedTime);

    // ������
    UpdateAutoRecovery(elapsedTime);

    // �摜�T�C�Y�X�V
    UpdateSpriteSize();

    // ���݂̗̑͂�ۑ�����
    oldHealth_ = PlayerManager::Instance().GetPlayer()->GetHealth();
}

// ----- �`�� -----
void UIHealth::Render()
{
    // �S�Ă�UI����������Ă��Ȃ�
    if (isAllUICreated == false) return;
    // �`�悵�Ȃ�
    if (GetIsDraw() == false) return;

    healthFrame_->Render();

    autoRecoveryBar_->Render();

    UI::Render();
}

// ----- ImGui�p -----
void UIHealth::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str())) 
    {
        ImGui::DragFloat("HealSpeed", &healSpeed_);

        UI::DrawDebug();

        autoRecoveryBar_->DrawDebug();

        healthFrame_->DrawDebug();

        ImGui::TreePop();
    }
}

// ----- �_���[�W���ʂ�K�p -----
void UIHealth::ApplyDamageEffect()
{
    const float currentHealth = PlayerManager::Instance().GetPlayer()->GetHealth();

    // �_���[�W���󂯂Ă��Ȃ��̂ł����ŏI��
    if (currentHealth >= oldHealth_) return;

    isVibration_     = true; // �U���X�V�����K�p
    vibrationVolume_ = 5.0f; // �U���̋���
    vibrationTimer_  = 0.2f; // �U�����Ԍv���p
    vibrationTime_   = 0.2f; // �U������
    
    // �����񕜂���Q�[�W�̕������߂�
    autoRecoveryHealth_ = currentHealth + 30.0f;
}

// ----- �U���X�V -----
void UIHealth::UpdateVibration(const float& elapsedTime)
{
    // �U�����������Ȃ��̂ł����ŏI��
    if (isVibration_ == false) return;

    // �^�C�}�[�X�V
    vibrationTimer_ -= elapsedTime;
    vibrationTimer_ = max(vibrationTimer_, 0.0f);

    // �U���̍X�V���Ԃ��I�����Ă�����UI�����̈ʒu�ɖ߂��ďI������
    if (vibrationTimer_ <= 0.0f)
    {
        SetUIPosition(healthPosition_);

        isVibration_ = false;

        return;
    }

    DirectX::XMFLOAT2 vibrationVec = { rand() % 100 - 50.0f, rand() % 100 - 50.0f };
    vibrationVec = XMFloat2Normalize(vibrationVec);

    const float vibrationVolume = Easing::InSine(vibrationTimer_, vibrationTimer_, vibrationVolume_, 0.0f);
    const DirectX::XMFLOAT2 uiPosition = vibrationVec * vibrationVolume + healthPosition_;
    
    SetUIPosition(uiPosition);
}

// ----- ������ -----
void UIHealth::UpdateAutoRecovery(const float& elapsedTime)
{
    float health = PlayerManager::Instance().GetPlayer()->GetHealth();

    // �񕜗ʂ��Ȃ��Ƃ��͂����ŏI��
    if (health >= autoRecoveryHealth_) return;

    health += healSpeed_ * elapsedTime;
    health = min(health, autoRecoveryHealth_);

    PlayerManager::Instance().GetPlayer()->SetHealth(health);
}

// ----- �摜�T�C�Y���X�V���� -----
void UIHealth::UpdateSpriteSize()
{
    const float health = PlayerManager::Instance().GetPlayer()->GetHealth();
    const float maxHealth = PlayerManager::Instance().GetPlayer()->GetMaxHealth();

    // ���݂̗̑͂̑S�̂��猩���������o��
    float currentHealth = health / maxHealth;

    const float healthSizeX = max(maxHealthSizeX_ * currentHealth, 0.0f);

    GetTransform()->SetSizeX(healthSizeX);

    float recoveryHealth = autoRecoveryHealth_ / maxHealth;
    recoveryHealth = max(maxHealthSizeX_ * recoveryHealth, 0.0f);
    autoRecoveryBar_->GetTransform()->SetSizeX(recoveryHealth);
}

// ----- �ꊇ��UI�̈ʒu��ݒ肷�� -----
void UIHealth::SetUIPosition(const DirectX::XMFLOAT2& position)
{
    GetTransform()->SetPosition(position);
    healthFrame_->GetTransform()->SetPosition(position);
    autoRecoveryBar_->GetTransform()->SetPosition(position);
}
