#include "UIStamina.h"
#include "Easing.h"
#include "Character/Player/PlayerManager.h"

// ----- �R���X�g���N�^ -----
UIStamina::UIStamina()
    : UI(UIManager::UIType::UIStamina, L"./Resources/Image/White.png", "UIStamina")
{
    // �X�^�~�i�����Ȃ��x��(�ԐF)�̐ݒ�
    staminaWarning_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    staminaWarning_->SetName("StaminaWarning");
    staminaWarning_->GetTransform()->SetPosition(50.0f, 70.0f);
    staminaWarning_->GetTransform()->SetSize(maxStaminaSizeX_, 7.0f);
    staminaWarning_->GetTransform()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);

    // �X�^�~�i�̘g(���F)�̐ݒ�
    staminaFrame_ = std::make_unique<Sprite>(L"./Resources/Image/White.png");
    staminaFrame_->SetName("StaminaFrame");
    staminaFrame_->GetTransform()->SetPosition(50.0f, 70.0f);
    staminaFrame_->GetTransform()->SetSize(maxStaminaSizeX_, 7.0f);
    staminaFrame_->GetTransform()->SetColor(0.0f, 0.0f, 0.0f, 0.4f);    

    SetSpriteName("Stamina");
    GetTransform()->SetPosition(50.0f, 70.0f);
    GetTransform()->SetSize(maxStaminaSizeX_, 7.0f);
    GetTransform()->SetColor(1.0f, 1.0f, 0.0f);

    isAllUICreated = true;
}

// ----- �X�V -----
void UIStamina::Update(const float& elapsedTime)
{
    if (isAllUICreated == false) return;

    const float stamina = PlayerManager::Instance().GetPlayer()->GetStamina();
    const float maxStamina = PlayerManager::Instance().GetPlayer()->GetMaxStamina();

    // ���݂̗̑͂̑S�̂��猩���������o��
    float currentStamina = stamina / maxStamina;

    const float staminaSizeX = max(maxStaminaSizeX_ * currentStamina, 0.0f);

    GetTransform()->SetSizeX(staminaSizeX);

    // ------------------------------------------------------------
    //  �X�^�~�i������̃R�X�g�g�p�R�X�g��菭�Ȃ�������Warning���o��
    // ------------------------------------------------------------
    const float dodgeStaminaCost = PlayerManager::Instance().GetPlayer()->GetDodgeStaminaCost();
    if (stamina < dodgeStaminaCost)
    {
        warningFlashTimer_ += elapsedTime;
        if (warningFlashTimer_ >= warningFlashTime_)
        {
            isFadingIn_ = isFadingIn_ ? false : true;
            warningFlashTimer_ = 0.0f;
        }

        float alpha = 0.0f;
        if (isFadingIn_)
        {
            alpha = Easing::InSine(warningFlashTimer_, warningFlashTime_, 1.0f, 0.0f);
        }
        else
        {
            alpha = Easing::InSine(warningFlashTimer_, warningFlashTime_, 0.0f, 1.0f);
        }
        staminaWarning_->GetTransform()->SetColorA(alpha);
    }
    else
    {
        staminaWarning_->GetTransform()->SetColorA(0.0f);
    }
}

// ----- �`�� -----
void UIStamina::Render()
{
    // �S�Ă�UI����������Ă��Ȃ�
    if (isAllUICreated == false) return;
    // �`�悵�Ȃ�
    if (GetIsDraw() == false) return;

    staminaFrame_->Render();

    staminaWarning_->Render();

    UI::Render();
}

// ----- ImGui�p -----
void UIStamina::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat("warningFlashTime", &warningFlashTime_);

        UI::DrawDebug();

        staminaWarning_->DrawDebug();

        staminaFrame_->DrawDebug();

        ImGui::TreePop();
    }
}
