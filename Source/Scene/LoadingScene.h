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

    void ShadowRender()                     override;
    void DeferredRender()                   override;
    void ForwardRender()                    override;
    void UserInterfaceRender()              override;

    void Render() override {};

    void DrawDebug()    override;
    
private:// �X���b�h�֌W
    static void LoadingThread(LoadingScene* scene);
    BaseScene*      nextScene_   = nullptr;
    std::thread*    thread_      = nullptr;
};

