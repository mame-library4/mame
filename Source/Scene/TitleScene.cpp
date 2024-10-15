#include "TitleScene.h"
#include "SceneManager.h"
#include "LoadingScene.h"
#include "GameScene.h"
#include "Input.h"
#include "UI/UITitle.h"

// ----- ���\�[�X���� -----
void TitleScene::CreateResource()
{
    UITitle* uiTitle = new UITitle();
}

// ----- ������ -----
void TitleScene::Initialize()
{
    // ���݂�Scene��ݒ�
    SceneManager::Instance().SetCurrentSceneName(SceneManager::SceneName::Title);

    // �ϐ�������
    isDrawUI_ = false;
}

// ----- �I���� -----
void TitleScene::Finalize()
{
    UIManager::Instance().Remove(UIManager::UIType::UITitle);
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

    // UI�`�攻��X�V
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

// ----- ImGui�p -----
void TitleScene::DrawDebug()
{
}
