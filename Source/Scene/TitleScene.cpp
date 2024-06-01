#include "TitleScene.h"
#include "../Graphics/Graphics.h"
#include "../Resource/texture.h"
#include "Input.h"

#include "GameScene.h"
#include "LoadingScene.h"
#include "SceneManager.h"

// ----- リソース生成 -----
void TitleScene::CreateResource()
{
    //sprite_ = std::make_unique<Sprite>(L"./Resources/Image/white.png");
    sprite_ = std::make_unique<Sprite>(L"./Resources/Image/Emma.png");
}

// ----- 初期化 -----
void TitleScene::Initialize()
{
    SceneManager::Instance().SetCurrentSceneName(SceneManager::SceneName::Title);
}

// ----- 終了化 -----
void TitleScene::Finalize()
{
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

void TitleScene::UserInterfaceRender()
{
    sprite_->Render();
}

// ----- ImGui用 -----
void TitleScene::DrawDebug()
{
    sprite_->DrawDebug();
}
