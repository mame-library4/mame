#pragma once
#include "BaseScene.h"
#include "UI/UITitle.h"

class TitleScene : public BaseScene
{
public:// ��{�I�Ȋ֐�
    TitleScene() {}
    ~TitleScene() override {}

    void CreateResource()                   override; // ���\�[�X����
    void Initialize()                       override; // ������
    void Finalize()                         override; // �I����
    void Update(const float& elapsedTime)   override; // �X�V����
    
    void ShadowRender()                     override;
    void DeferredRender()                   override;
    void ForwardRender()                    override;
    
    void Render() override {};

    void DrawDebug()                        override; // ImGui�p

private:
    UITitle* uiTitle_;

    bool isDrawUI_ = false; // UI�`�攻��
};
