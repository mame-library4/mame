#include "SceneManager.h"

// ������
void SceneManager::Initialize()
{
}

// �X�V����
void SceneManager::Update(const float& elapsedTime)
{
    if (nextScene)
    {
        // �Â��V�[�����I��
        Clear();

        // �V�����V�[����ݒ�
        currentScene = nextScene;
        nextScene = nullptr;

        // �V�[������������(�}���`�X���b�h���������Ă��Ȃ��ꍇ�ɍs��)
        if (!currentScene->IsReady())
        {
            currentScene->CreateResource();
            currentScene->Initialize();
        }
    }

    if (currentScene)
    {
        currentScene->Update(elapsedTime);
#ifdef USE_IMGUI
        currentScene->DrawDebug();
        DrawDebug();
#endif
    }
}

void SceneManager::ShadowRender()
{
    if (!currentScene) return;

    currentScene->ShadowRender();
}

void SceneManager::DeferredRender()
{
    if (!currentScene) return;

    currentScene->DeferredRender();
}

void SceneManager::ForwardRender()
{
    if (!currentScene) return;

    currentScene->ForwardRender();
}

void SceneManager::UserInterfaceRender()
{
    if (!currentScene) return;

    currentScene->UserInterfaceRender();
}

// �f�o�b�O�p
void SceneManager::DrawDebug()
{
}

// �V�[���N���A
void SceneManager::Clear()
{
    if (!currentScene) return;

    currentScene->Finalize();
    delete currentScene;
    currentScene = nullptr;
    //SafeDelete(currentScene);
}

// �V�[���؂�ւ�
void SceneManager::ChangeScene(BaseScene* scene)
{
    // �V�����V�[����ݒ�
    nextScene = scene;
}