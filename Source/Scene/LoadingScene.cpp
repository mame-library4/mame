#include "LoadingScene.h"
#include "SceneManager.h"

#include "../Graphics/Graphics.h"

#include "../Other/misc.h"

// ----- コンストラクタ -----
LoadingScene::LoadingScene(BaseScene* nextScene) 
    : nextScene_(nextScene)
{
}

// ----- リソース生成 -----
void LoadingScene::CreateResource()
{
}

// ----- 初期化 -----
void LoadingScene::Initialize()
{
    // スレッド開始
    // std::thread(LoadingThread, this);
    // 二個目の引数はLoadingThreadの引数になる
    thread_ = new std::thread(LoadingThread, this);
}

// ----- 終了化 -----
void LoadingScene::Finalize()
{
    // スレッド終了化
    thread_->join();
    if (thread_ != nullptr)
    {
        delete thread_;
        thread_ = nullptr;
    }
}

// 更新処理
void LoadingScene::Update(const float& elapsedTime)
{
    // 次のシーンが準備できたら
    if (nextScene_->IsReady())
    {
        // シーン切り替え
        SceneManager::Instance().ChangeScene(nextScene_);
        return;
    }
}

void LoadingScene::ShadowRender()
{
}

void LoadingScene::DeferredRender()
{
}

void LoadingScene::ForwardRender()
{
}

void LoadingScene::UserInterfaceRender()
{
}


// ----- ImGui用 -----
void LoadingScene::DrawDebug()
{
}

// ----- ローディングスレッド -----
void LoadingScene::LoadingThread(LoadingScene* scene)
{
    // COM関連の初期化でスレッド毎に呼ぶ必要がある
    std::ignore = CoInitialize(nullptr); // std::ignoreで返り値警告解消

    // 次のシーンの初期化を行う
    scene->nextScene_->CreateResource();
    scene->nextScene_->Initialize();
    

    // スレッドが終わる前にCOM関連の終了化
    CoUninitialize();

    // 次のシーンの準備完了設定
    scene->nextScene_->SetReady();
}