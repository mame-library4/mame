#include "TitleScene.h"
#include "../Graphics/Graphics.h"
#include "../Resource/texture.h"

#include "GameScene.h"
#include "LoadingScene.h"
#include "SceneManager.h"

// ----- リソース生成 -----
void TitleScene::CreateResource()
{
}

// ----- 初期化 -----
void TitleScene::Initialize()
{
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
}

// ----- ImGui用 -----
void TitleScene::DrawDebug()
{
}
