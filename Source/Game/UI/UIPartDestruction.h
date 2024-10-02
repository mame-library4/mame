#pragma once
#include "UI.h"

class UIPartDestruction : public UI
{
public:
    UIPartDestruction();
    ~UIPartDestruction() override {}

    void Update(const float& elapsedTime)   override; // �X�V
    void Render()                           override; // �`��
    void DrawDebug()                        override; // ImGui�p

private:
    float lifeTimer_    = 10.0f; // ��������
    
    float fadeInTimer_ = 0.0f;   // �t�F�C�h�C���^�C�}�[
    float fadeInSpeed_ = 10.0f;  // �t�F�C�h�C�����x
    bool  isFadingIn_  = true;   // �t�F�C�h�C������
    
    float fadeOutTimer_ = 0.0f;  // �t�F�C�h�A�E�g�^�C�}�[
    float fadeOutSpeed_ = 5.0f;  // �t�F�C�h�A�E�g���x
    bool  isFadingOut_  = false; // �t�F�C�h�A�E�g����

    DirectX::XMFLOAT2 oldPostiion_ = {};
};

