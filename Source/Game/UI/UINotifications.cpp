#include "UINotifications.h"

// ----- �R���X�g���N�^ -----
UINotifications::UINotifications(const UIManager::UIType& type, const wchar_t* filename, const std::string& name)
    : UI(type, filename, name)
{
}

// ----- �X�V -----
void UINotifications::Update(const float& elapsedTime)
{
    UI::Update(elapsedTime);

    // �t�F�C�h�C������
    if (isFadingIn_)
    {
        fadeInTimer_ += fadeInSpeed_ * elapsedTime;
        fadeInTimer_ = min(fadeInTimer_, 1.0f);

        const float posX = XMFloatLerp(fadeInStartPositionX_, fadeInEndPositionX_, fadeInTimer_);

        GetTransform()->SetPositionX(posX);

        // �t�F�C�h�C��������������I������
        if (fadeInTimer_ == 1.0f)
        {
            oldPostiion_ = GetTransform()->GetPosition();

            isFadingIn_ = false;
        }
        // �t�F�C�h�C�����͂����ŏI��
        else
        {
            return;
        }
    }

    // �������Ԃ��X�V����
    lifeTimer_ -= elapsedTime;

    // �������Ԃ��Ȃ��Ȃ�����t�F�C�h�A�E�g�̃t���O�𗧂Ă�
    if (lifeTimer_ <= 0.0f && isFadingOut_ == false) isFadingOut_ = true;

    // �t�F�C�h�A�E�g����UI����������
    if (isFadingOut_)
    {
        fadeOutTimer_ += fadeOutSpeed_ * elapsedTime;
        fadeOutTimer_ = min(fadeOutTimer_, 1.0f);

        const float currentPosY = GetTransform()->GetPositionY();

        const float posY = XMFloatLerp(currentPosY, currentPosY - 5.0f, fadeOutTimer_);
        const float alpha = XMFloatLerp(1.0f, 0.0f, fadeOutTimer_);

        GetTransform()->SetPositionY(posY);
        GetTransform()->SetColorA(alpha);

        // �t�F�C�h�A�E�g�����������玩�g���폜����
        if (fadeOutTimer_ == 1.0f) UIManager::Instance().Remove(this);
    }
}

// ----- �`�� -----
void UINotifications::Render()
{
    UI::Render();
}

// ----- ImGui�p -----
void UINotifications::DrawDebug()
{
#ifdef _DEBUG
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat("LifeTimer", &lifeTimer_);
        ImGui::DragFloat("FadeOutTimer", &fadeOutTimer_);
        ImGui::DragFloat("FadeOutSpeed", &fadeOutSpeed_);

        UI::DrawDebug();

        ImGui::TreePop();
    }
#endif
}
