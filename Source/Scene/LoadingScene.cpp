#include "LoadingScene.h"
#include "SceneManager.h"
#include "Graphics.h"
#include "Misc.h"
#include "UI/UIManager.h"
#include "Camera.h"

// ----- コンストラクタ -----
LoadingScene::LoadingScene(BaseScene* nextScene) 
    : nextScene_(nextScene)
{
}

// ----- リソース生成 -----
void LoadingScene::CreateResource()
{
    UIManager::Instance().Remove(UIManager::UIType::UIFader);
    uiLoading_ = new UILoading();

    Graphics::Instance().CreatePsFromCso("./Resources/Shader/GltfModelLoadingPS.cso", loadingPlayerPS_.GetAddressOf());
    loadingPlayer_ = std::make_unique<Object>("./Resources/Model/LoadingObject/LoadingPlayer.gltf", 0.01f);
}

// ----- 初期化 -----
void LoadingScene::Initialize()
{
    // スレッド開始
    // std::thread(LoadingThread, this);
    // 二個目の引数はLoadingThreadの引数になる
    thread_ = new std::thread(LoadingThread, this);

    loadingPlayer_->PlayAnimation(0, true, 1.1f);
    loadingPlayer_->GetTransform()->SetPosition(-2.5f, -0.1f, 0.0f);
    loadingPlayer_->GetTransform()->SetRotationY(DirectX::XMConvertToRadians(110.0f));
    loadingPlayer_->GetTransform()->SetScaleFactor(0.7f);

    isCreateUIFader_ = false;

    Camera::Instance().SetLoadingCamera();
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

    if (uiLoading_ != nullptr)
    {
        UIManager::Instance().Remove(uiLoading_);
        uiLoading_ = nullptr;
    }
}

// 更新処理
void LoadingScene::Update(const float& elapsedTime)
{
    // 現在のSceneを設定
    if (SceneManager::Instance().GetCurrentSceneName() != SceneManager::SceneName::Loading)
    {
        SceneManager::Instance().SetCurrentSceneName(SceneManager::SceneName::Loading);
    }

    loadingPlayer_->Update(elapsedTime);

    // 次のシーンが準備できたら
    if (nextScene_->IsReady() && isCreateUIFader_ == false)
    {
        uiFader_ = new UIFader(false);
        isCreateUIFader_ = true;
    }

    if (isCreateUIFader_)
    {
        if (uiFader_->GetIsFadeComplete() == false) return;

        if (uiFader_ != nullptr)
        {
            uiFader_ = nullptr;
        }

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

void LoadingScene::Render()
{
    loadingPlayer_->Render(loadingPlayerPS_.Get());
}

// ----- ImGui用 -----
void LoadingScene::DrawDebug()
{
    UIManager::Instance().DrawDebug();

    loadingPlayer_->DrawDebug();
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