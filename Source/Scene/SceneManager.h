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

    void Initialize();
    void Update(const float& elapesdTime);  // �X�V����
        
    void ShadowRender();
    void DeferredRender();
    void ForwardRender();
    void UserInterfaceRender();

    void DrawDebug();

    void Clear();   // �V�[���N���A

    // �V�[���؂�ւ�
    void ChangeScene(BaseScene* scene);

    BaseScene* GetCurrentScene()const { return currentScene; }

private:
    BaseScene* currentScene = nullptr;
    BaseScene* nextScene = nullptr;
};
