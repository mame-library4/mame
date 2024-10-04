#include "UIPartDestruction.h"

// ----- �R���X�g���N�^ -----
UIPartDestruction::UIPartDestruction()
    : UI(UIManager::UIType::UIPartDestruction, L"./Resources/Image/PartDestruction/PartDestruction2.png", "UIPartDestruction")
    //: UI(UIManager::UIType::UIPartDestruction, L"./Resources/Image/PartDestruction/PartDestruction.png", "UIPartDestruction")
{
    GetTransform()->SetPosition(970.0f, 420.0f);
    
    GetTransform()->SetSize(230.0f, 40.0f);
}

// ----- �X�V -----
void UIPartDestruction::Update(const float& elapsedTime)
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
        fadeOutTimer_ += fadeOutSpeed_* elapsedTime;
        fadeOutTimer_ = min(fadeOutTimer_, 1.0f);

        const float currentPosY = GetTransform()->GetPositionY();

        const float posY = XMFloatLerp(currentPosY, currentPosY - 10.0f, fadeOutTimer_);
        const float alpha = XMFloatLerp(1.0f, 0.0f, fadeOutTimer_);

        GetTransform()->SetPositionY(posY);
        GetTransform()->SetColorA(alpha);

        // UI�N���X�ł̈ړ���������肭�@�\�����邽�߂ɂ����ōX�V����
        const float addPosY = oldPostiion_.y - posY;
        oldPostiion_.y = posY;
        AddMoveTargetPosition({ 0.0f, addPosY });

        // �t�F�C�h�A�E�g�����������玩�g���폜����
        if (fadeOutTimer_ == 1.0f) UIManager::Instance().Remove(this);
    }
}

// ----- �`�� -----
void UIPartDestruction::Render()
{
    UI::Render();
}

// ----- ImGui -----
void UIPartDestruction::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        ImGui::DragFloat("LifeTimer", &lifeTimer_);
        ImGui::DragFloat("FadeOutTimer", &fadeOutTimer_);
        ImGui::DragFloat("FadeOutSpeed", &fadeOutSpeed_);

        UI::DrawDebug();

        ImGui::TreePop();
    }
}
