#pragma once
#include "BaseScene.h"
#include <thread>
#include <memory>

class LoadingScene : public BaseScene
{
public:// ��{�I�Ȋ֐�
    LoadingScene(BaseScene* nextScene);
    ~LoadingScene() override {}

    void CreateResource()                   override; // ���\�[�X����
    void Initialize()                       override; // ������
    void Finalize()                         override; // �I����
    void Update(const float& elapsedTime)   override; // �X�V����

    void ShadowRender(ID3D11DeviceContext* deviceContext)           override;
    void DeferredRender(ID3D11DeviceContext* deviceContext)         override;
    void ForwardRender(ID3D11DeviceContext* deviceContext)          override;
    void UserInterfaceRender(ID3D11DeviceContext* deviceContext)    override;

    void DrawDebug()    override;
    
private:// �X���b�h�֌W
    static void LoadingThread(LoadingScene* scene);
    BaseScene*      nextScene_   = nullptr;
    std::thread*    thread_      = nullptr;
};

