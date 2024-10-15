#pragma once
#include "BaseScene.h"

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
    bool isDrawUI_ = false; // UI�`�攻��
};
