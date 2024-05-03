#pragma once
#include "BaseScene.h"
#include <memory>
#include "../Game/Character/Player/Player.h"
#include "../Game/Stage/StageNormal.h"
#include "../Game/Character/Enemy/EnemyTamamo.h"

class TitleScene : public BaseScene
{
public:// ��{�I�Ȋ֐�
    TitleScene() {}
    ~TitleScene() override {}

    void CreateResource()                   override; // ���\�[�X����
    void Initialize()                       override; // ������
    void Finalize()                         override; // �I����
    void Update(const float& elapsedTime)   override; // �X�V����
    
    void ShadowRender(ID3D11DeviceContext* deviceContext)           override;
    void DeferredRender(ID3D11DeviceContext* deviceContext)         override;
    void ForwardRender(ID3D11DeviceContext* deviceContext)          override;
    void UserInterfaceRender(ID3D11DeviceContext* deviceContext)    override;

    void DrawDebug()                        override; // ImGui�p

};
