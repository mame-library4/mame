#include "SceneManager.h"

// 初期化
void SceneManager::Initialize()
{
}

// 更新処理
void SceneManager::Update(const float& elapsedTime)
{
    if (nextScene)
    {
        // 古いシーンを終了
        Clear();

        // 新しいシーンを設定
        currentScene = nextScene;
        nextScene = nullptr;

        // シーン初期化処理(マルチスレッド処理をしていない場合に行う)
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

// デバッグ用
void SceneManager::DrawDebug()
{
}

// シーンクリア
void SceneManager::Clear()
{
    if (!currentScene) return;

    currentScene->Finalize();
    delete currentScene;
    currentScene = nullptr;
    //SafeDelete(currentScene);
}

// シーン切り替え
void SceneManager::ChangeScene(BaseScene* scene)
{
    // 新しいシーンを設定
    nextScene = scene;
}