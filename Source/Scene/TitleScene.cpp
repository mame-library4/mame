#include "TitleScene.h"
#include "../Graphics/Graphics.h"
#include "../Resource/texture.h"

#include "GameScene.h"
#include "LoadingScene.h"
#include "SceneManager.h"

// ----- ���\�[�X���� -----
void TitleScene::CreateResource()
{
}

// ----- ������ -----
void TitleScene::Initialize()
{
}

// ----- �I���� -----
void TitleScene::Finalize()
{
}

// ----- �X�V -----
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

// ----- ImGui�p -----
void TitleScene::DrawDebug()
{
}
