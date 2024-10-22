#include "TitleState.h"
#include "Input.h"

// ----- IdleState -----
namespace TitleState
{
    // ----- 初期化 -----
    void IdleState::Initialize()
    {
        // 全UI非表示(リセット)
        owner_->GetUITitle()->SetIsDrawAllUI(false);

        // タイトルロゴ表示
        owner_->GetUITitle()->SetIsDrawTitleLogo(true);
    } 

    // ----- 更新 -----
    void IdleState::Update(const float& elapsedTime)
    {
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_A)
        {
            owner_->ChangeState(TitleScene::STATE::Select);
            return;
        }
    }

    // ----- 終了化 -----
    void IdleState::Finalize()
    {

    }

    // ----- ImGui用 -----
    void IdleState::DrawDebug()
    {

    }
}

// ----- SelectState -----
namespace TitleState
{
    // ----- 初期化 -----
    void SelectState::Initialize()
    {
        // 全UI非表示(リセット)
        owner_->GetUITitle()->SetIsDrawAllUI(false);
    }

    // ----- 更新 -----
    void SelectState::Update(const float& elapsedTime)
    {
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_B)
        {
            owner_->ChangeState(TitleScene::STATE::Idle);
            return;
        }
    }

    // ----- 終了化 -----
    void SelectState::Finalize()
    {
    }

    // ----- ImGui用 -----
    void SelectState::DrawDebug()
    {
    }
}