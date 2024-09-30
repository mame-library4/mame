#include "UICrosshair.h"
#include "Character/Enemy/EnemyManager.h"
#include "Camera.h"
#include "Easing.h"
#include "AudioManager.h"

// ----- �R���X�g���N�^ -----
UICrosshair::UICrosshair()
    : UI(UIManager::UIType::UICrosshair, L"./Resources/Image/Crosshair/OuterLine.png", "UICrosshair")
{
    // �A�E�g���C������
    SetSpriteName("OuterLine");
    
    // �Z���^�[�h�b�gSprite���� & ����
    centerDot_ = std::make_unique<Sprite>(L"./Resources/Image/Crosshair/CenterDot.png");
    centerDot_->SetName("CenterDot");
    centerDot_->GetTransform()->SetColorA(0.0f);
}

// ----- �X�V -----
void UICrosshair::Update(const float& elapsedTime)
{
    // �G(�^�[�Q�b�g)�������Ă��邩����
    if (JudgementEnemyAlive() == false) return;

    // ���݃��b�N�I�����Ă���W���C���g�̈ʒu���擾����
    const std::string jointName = Camera::Instance().GetCurrentTargetJointName();
    targetJointPosition_ = EnemyManager::Instance().GetEnemy(0)->GetJointPosition(jointName);

    // UI�`��ʒu�����E�ɓ����Ă邩�𔻒肷��
    if (JudgementDraw() == false) return;

    // Sprite�̑傫���ƐF���X�V����
    UpdateSpriteSizeAndColor(elapsedTime);

    // Sprite�̈ʒu���X�V����
    UpdateSpritePosition();
}

// ----- �`�� -----
void UICrosshair::Render()
{
    // �G(�^�[�Q�b�g)�������Ă��Ȃ�
    if (isTargetEnemyAlive_ == false) return;

    // ���E�ɓ����ĂȂ��̂ŕ`�悵�Ȃ�
    if (isDraw_ == false) return;

    // �A�E�g���C���`�� ( �O�g )
    UI::Render();

    // �Z���^�[�h�b�g�`��
    centerDot_->Render();
}

// ----- ImGui�p -----
void UICrosshair::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    {
        if (ImGui::Button("ResetTimer"))
        {
            outerLineTimer_ = 0.0f;
        }
        ImGui::DragFloat("OuterLineTotalFrame_", &outerLineTotalFrame_);

        UI::DrawDebug();

        centerDot_->DrawDebug();

        ImGui::TreePop();
    }
}

// ----- Sprite�̈ʒu�X�V -----
void UICrosshair::UpdateSpritePosition()
{   
    // World��Ԃ���Screen��Ԃ֕ϊ�
    DirectX::XMFLOAT2 targetPosition = Sprite::ConvertToScreenPos(targetJointPosition_);

    // ---------- �ʒu���X�V���� ----------
    DirectX::XMFLOAT2 outerLinePosition = { targetPosition.x - GetTransform()->GetSizeX() * 0.5f, targetPosition.y - GetTransform()->GetSizeY() * 0.5f };
    GetTransform()->SetPosition(outerLinePosition, GetTransform()->GetSize());

    DirectX::XMFLOAT2 centerDotPosition = { targetPosition.x - centerDot_->GetTransform()->GetSizeX() * 0.5f, targetPosition.y - centerDot_->GetTransform()->GetSizeY() * 0.5f };
    centerDot_->GetTransform()->SetPosition(centerDotPosition, centerDot_->GetTransform()->GetSize());
}

// ----- Sprite�̑傫���X�V -----
void UICrosshair::UpdateSpriteSizeAndColor(const float& elapsedTime)
{
    // �傫���̍X�V�͏I�������̂ł����ŏI��
    if (isSpriteSizeUpdated_) return;

    outerLineTimer_ += elapsedTime;
    outerLineTimer_ = min(outerLineTimer_, outerLineTotalFrame_);

    const float size = Easing::InSine(outerLineTimer_, outerLineTotalFrame_, 64.0f, 256.0f);
    const float alpha = Easing::InSine(outerLineTimer_, outerLineTotalFrame_, 1.0f, 0.0f);

    GetTransform()->SetSize(size);
    GetTransform()->SetColorA(alpha);

    // �A�E�g���C�� ( �O�g ) �̍X�V���I�������Z���^�[�h�b�g��\������
    if (outerLineTimer_ == outerLineTotalFrame_)
    {
        centerDot_->GetTransform()->SetSize(128.0f);
        centerDot_->GetTransform()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);

        AudioManager::Instance().PlaySE(SE::Lockon);

        isSpriteSizeUpdated_ = true;
    }
}

// ----- �G(�^�[�Q�b�g)�������Ă��邩���� -----
const bool UICrosshair::JudgementEnemyAlive()
{
    isTargetEnemyAlive_ = false;

    // �����������݂��Ă��Ȃ�
    if (EnemyManager::Instance().GetEnemyCount() == 0) return false;

    // ���S���Ă���
    if (EnemyManager::Instance().GetEnemy(0)->GetHealth() <= 0) return false;

    // �����Ă���
    isTargetEnemyAlive_ = true;

    return true;
}

// ----- UI�`��ʒu�����E�ɓ����Ă邩�𔻒蔻�� -----
const bool UICrosshair::JudgementDraw()
{
    isDraw_ = false;

    // TODO:Y�����l�����Ă��Ȃ��̂ł�����������o�O�邩��
    DirectX::XMFLOAT3 cameraPosition = Camera::Instance().GetTransform()->GetPosition();
    DirectX::XMFLOAT3 cameraForward = Camera::Instance().CalcForward();
    DirectX::XMFLOAT2 targetVec = XMFloat2Normalize({cameraPosition.x - targetJointPosition_.x, cameraPosition.z - targetJointPosition_.z});
    const float dot = XMFloat2Dot(targetVec, XMFloat2Normalize({ cameraForward.x, cameraForward.z }));

    if (dot > 0) return false;
    //if (dot > DirectX::XM_PIDIV2) return false;

    // �`�悷��
    isDraw_ = true;

    return true;
}
