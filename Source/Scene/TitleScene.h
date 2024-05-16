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
    
    void ShadowRender()                     override;
    void DeferredRender()                   override;
    void ForwardRender()                    override;
    void UserInterfaceRender()              override;

    void DrawDebug()                        override; // ImGui�p

};
