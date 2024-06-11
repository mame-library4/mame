#pragma once
#include "BaseScene.h"
#include <thread>
#include <memory>

class LoadingScene : public BaseScene
{
public:// 基本的な関数
    LoadingScene(BaseScene* nextScene);
    ~LoadingScene() override {}

    void CreateResource()                   override; // リソース生成
    void Initialize()                       override; // 初期化
    void Finalize()                         override; // 終了化
    void Update(const float& elapsedTime)   override; // 更新処理

    void ShadowRender()                     override;
    void DeferredRender()                   override;
    void ForwardRender()                    override;
    void UserInterfaceRender()              override;

    void Render() override {};

    void DrawDebug()    override;
    
private:// スレッド関係
    static void LoadingThread(LoadingScene* scene);
    BaseScene*      nextScene_   = nullptr;
    std::thread*    thread_      = nullptr;
};

