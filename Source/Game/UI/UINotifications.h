#pragma once
#include "UI.h"

class UINotifications : public UI
{
public:
    UINotifications(const UIManager::UIType& type, const wchar_t* filename = nullptr, const std::string& name = "UI");
    ~UINotifications() override {}

    void Update(const float& elapsedTime)   override; // �X�V
    void Render()                           override; // �`��
    void DrawDebug()                        override; // ImGui�p

private:
    float lifeTimer_ = 10.0f; // ��������

    float fadeInStartPositionX_ = 1280.0f;
    float fadeInEndPositionX_ = 1030.0f;

    float fadeInTimer_ = 0.0f;   // �t�F�C�h�C���^�C�}�[
    float fadeInSpeed_ = 10.0f;  // �t�F�C�h�C�����x
    bool  isFadingIn_ = true;   // �t�F�C�h�C������

    float fadeOutTimer_ = 0.0f;  // �t�F�C�h�A�E�g�^�C�}�[
    float fadeOutSpeed_ = 5.0f;  // �t�F�C�h�A�E�g���x
    bool  isFadingOut_ = false; // �t�F�C�h�A�E�g����

    DirectX::XMFLOAT2 oldPostiion_ = {};
};

