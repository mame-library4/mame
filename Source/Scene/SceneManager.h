#pragma once
#include "BaseScene.h"
#include <d3d11.h>

class SceneManager
{
private:
    SceneManager() {}
    ~SceneManager() {}

public:
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    enum class SceneName
    {
        Title,
        Game,
    };

    void Initialize();
    void Update(const float& elapesdTime);  // 更新処理
        
    void ShadowRender();
    void DeferredRender();
    void ForwardRender();

    void Render();

    void DrawDebug();

    void Clear();   // シーンクリア

    // シーン切り替え
    void ChangeScene(BaseScene* scene);

    BaseScene* GetCurrentScene() const { return currentScene; }

    SceneName GetCurrentSceneName() { return currneSceneName_; }
    void SetCurrentSceneName(const SceneName& sceneName) { currneSceneName_ = sceneName; }

private:
    BaseScene* currentScene = nullptr;
    BaseScene* nextScene = nullptr;

    SceneName currneSceneName_ = SceneName::Title;
};
