#pragma once
#include "BaseScene.h"
#include "Object.h"
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

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> iblTextures_[4];
    std::unique_ptr<Object> stageObject_;
    std::unique_ptr<Object> dragonObject_;
    std::unique_ptr<Object> playerObject_;

    bool isDrawUI_ = false; // UI�`�攻��
};
