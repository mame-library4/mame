#include "TitleScene.h"
#include "SceneManager.h"
#include "LoadingScene.h"
#include "GameScene.h"
#include "Input.h"
#include "UI/UITitle.h"

// ----- リソース生成 -----
void TitleScene::CreateResource()
{
    UITitle* uiTitle = new UITitle();
}

// ----- 初期化 -----
void TitleScene::Initialize()
{
    // 現在のSceneを設定
    SceneManager::Instance().SetCurrentSceneName(SceneManager::SceneName::Title);

    // 変数初期化
    isDrawUI_ = false;
}

// ----- 終了化 -----
void TitleScene::Finalize()
{
    UIManager::Instance().Remove(UIManager::UIType::UITitle);
}

// ----- 更新 -----
void TitleScene::Update(const float& elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_A)
    {
        SceneManager::Instance().ChangeScene(new LoadingScene(new GameScene));
        return;
    }

    // UI描画判定更新
    if (isDrawUI_ == false)
    {
        UI* ui = UIManager::Instance().GetUI(UIManager::UIType::UITitle);
        if (ui != nullptr)
        {
            ui->SetIsDraw();

            isDrawUI_ = true;
        }
    }
}

void TitleScene::ShadowRender()
{
}

void TitleScene::DeferredRender()
{
}

void TitleScene::ForwardRender()
{
}

// ----- ImGui用 -----
void TitleScene::DrawDebug()
{
}
