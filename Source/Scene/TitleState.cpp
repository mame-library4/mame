#include "TitleState.h"
#include "Input.h"
#include "System/SystemManager.h"
#include "Camera.h"
#include "SceneManager.h"
#include "LoadingScene.h"
#include "GameScene.h"
#include "Application.h"
#include "AudioManager.h"

// ----- IdleState -----
namespace TitleState
{
    // ----- 初期化 -----
    void IdleState::Initialize()
    {
        // 全UI非表示(リセット)
        owner_->GetUITitle()->SetIsDrawAllUI(false);

        // タイトルロゴ表示
        owner_->GetUITitle()->SetIsDraw(UITitle::Type::TitleLogo, true);

        owner_->GetUITitle()->SetIsDraw(UITitle::Type::PressAnyButton, true);

        // 変数初期化
        lerpTimer_ = 0.0f;
        startRotationY_ = Camera::Instance().GetTransform()->GetRotationY();

        isAllowedToTransition_ = false;
    } 

    // ----- 更新 -----
    void IdleState::Update(const float& elapsedTime)
    {
        lerpTimer_ += lerpSpeed_ * elapsedTime;
        lerpTimer_ = min(lerpTimer_, 1.0f);
        const float rotationY = XMFloatLerp(startRotationY_, targetRotationY_, lerpTimer_);
        Camera::Instance().GetTransform()->SetRotationY(rotationY);

        if (isAllowedToTransition_ == false && lerpTimer_ == 1.0f) isAllowedToTransition_ = true;

        // 入力をチェックする
        if (CheckInput())
        {
            AudioManager::Instance().PlaySE(SE::Press);

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
        if (ImGui::TreeNodeEx("Idle", ImGuiTreeNodeFlags_Framed))
        {
            ImGui::DragFloat("LerpSpeed", &lerpSpeed_);

            ImGui::TreePop();
        }
    }

    // ----- 入力をチェックする -----
    const bool IdleState::CheckInput()
    {
        // 遷移可能ではないので入力をチェックしない
        if (isAllowedToTransition_ == false) return false;

        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_A ||
            Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_B ||
            Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_Y ||
            Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_X ||
            Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_RIGHT_SHOULDER ||
            Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_RIGHT_TRIGGER ||
            Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_LEFT_SHOULDER ||
            Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_LEFT_TRIGGER ||
            Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_START)
        {
            return true;
        }

        return false;
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

        // タイトルロゴ表示
        owner_->GetUITitle()->SetIsDraw(UITitle::Type::TitleLogo, true);

        owner_->GetUITitle()->SetIsDraw(UITitle::Type::Back, true);
        owner_->GetUITitle()->SetIsDraw(UITitle::Type::GameStart, true);
        owner_->GetUITitle()->SetIsDraw(UITitle::Type::Option, true);
        owner_->GetUITitle()->SetIsDraw(UITitle::Type::Quit, true);

        // 変数初期化
        lerpTimer_ = 0.0f;
        startRotationY_ = Camera::Instance().GetTransform()->GetRotationY();

        isAllowedToTransition_ = false;

        currentState_           = STATE::GameStart;
        breakTimer_             = 0.0f;
    }

    // ----- 更新 -----
    void SelectState::Update(const float& elapsedTime)
    {
        // カメラ更新
        UpdateCamera(elapsedTime);

        // 入力チェック
        CheckInput(elapsedTime);

        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_B)
        {
            if (isAllowedToTransition_)
            {
                owner_->ChangeState(TitleScene::STATE::Idle);
                return;
            }
        }     
    }

    // ----- 終了化 -----
    void SelectState::Finalize()
    {
    }

    // ----- ImGui用 -----
    void SelectState::DrawDebug()
    {
        if (ImGui::TreeNodeEx("Select", ImGuiTreeNodeFlags_Framed))
        {
            if (ImGui::TreeNodeEx("---------- Select ----------", ImGuiTreeNodeFlags_DefaultOpen))
            {
                std::string stateNameList[] = { "GameStart", "Options", "Quit" };
                std::string stateName = "CurrentState : " + stateNameList[static_cast<int>(currentState_)];
                ImGui::Text(stateName.c_str());
                ImGui::DragFloat("BreakTime", &breakTime_, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("BreakTimer", &breakTimer_);
                ImGui::DragFloat("StickThreshold", &stickThreshold_, 0.01f, 0.0f, 1.0f);

                ImGui::TreePop();
            }

            ImGui::DragFloat("LerpSpeed", &lerpSpeed_);

            ImGui::TreePop();
        }
    }

    // ----- カメラ更新 -----
    void SelectState::UpdateCamera(const float& elapsedTime)
    {
        // 更新終了しているので処理しない
        if (lerpTimer_ == 1.0f) return;

        lerpTimer_ += lerpSpeed_ * elapsedTime;
        lerpTimer_ = min(lerpTimer_, 1.0f);
        const float rotationY = XMFloatLerp(startRotationY_, targetRotationY_, lerpTimer_);
        Camera::Instance().GetTransform()->SetRotationY(rotationY);

        // 更新終了したらIdleStateに戻れるようになる
        if (lerpTimer_ == 1.0f) isAllowedToTransition_ = true;
    }

    // ----- 入力チェック -----
    void SelectState::CheckInput(const float& elapsedTime)
    {
        breakTimer_ -= elapsedTime;
        breakTimer_ = max(breakTimer_, 0.0f);

        bool isPressAButton = false;
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_A) isPressAButton = true;

        // タイトルステート遷移入力
        if (isPressAButton)
        {
            switch (currentState_)
            {
            case STATE::GameStart:
                if (isAllowedToTransition_)
                {
                    AudioManager::Instance().PlaySE(SE::Press);
                    owner_->ChangeState(TitleScene::STATE::GameStart);
                }
                break;
            //case STATE::Options:   owner_->ChangeState(TitleScene::STATE::Options);   break;
            case STATE::Quit:
                if (isAllowedToTransition_)
                {
                    owner_->ChangeState(TitleScene::STATE::Quit);
                }
                break;
            }
        }

        // セレクトステート変更入力
        switch (currentState_)
        {
        case STATE::GameStart: ChangeState(STATE::Quit, STATE::Options);      break;
        case STATE::Options:   ChangeState(STATE::GameStart, STATE::Quit);    break;
        case STATE::Quit:      ChangeState(STATE::Options, STATE::GameStart); break;
        }
    }

    // ----- ステート変更 -----
    void SelectState::ChangeState(const STATE& upState, const STATE& downState)
    {
        // 遷移できない
        if (breakTimer_ != 0.0f) return;
        // 入力値が閾値に届いていない
        const float aLY = Input::Instance().GetGamePad().GetAxisLY();
        if (fabsf(aLY) < stickThreshold_) return;

        // 上方向の入力
        if (aLY > 0) currentState_ = upState;
        // 下方向の入力
        else         currentState_ = downState;

        owner_->GetUITitle()->SetBackSpritePosition(static_cast<int>(currentState_));
        breakTimer_ = breakTime_;

        // カーソル移動音再生
        AudioManager::Instance().PlaySE(SE::Select);
    }
}

// ----- GameStartState -----
namespace TitleState
{
    // ----- 初期化 -----
    void GameStartState::Initialize()
    {
        // 全UI非表示(リセット)
        owner_->GetUITitle()->SetIsDrawAllUI(false);

        owner_->GetUITitle()->SetIsDraw(UITitle::Type::TitleLogo, true);
        owner_->GetUITitle()->SetIsDraw(UITitle::Type::Back, true);
        owner_->GetUITitle()->SetIsDraw(UITitle::Type::GameStart, true);
        owner_->GetUITitle()->SetIsDraw(UITitle::Type::Option, true);
        owner_->GetUITitle()->SetIsDraw(UITitle::Type::Quit, true);
        owner_->GetUITitle()->SetIsDraw(UITitle::Type::Black, true);
    }

    // ----- 更新 -----
    void GameStartState::Update(const float& elapsedTime)
    {
        if (owner_->GetUITitle()->FadeOut(elapsedTime))
        {
            SceneManager::Instance().ChangeScene(new LoadingScene(new GameScene));
        }
    }

    // ----- 終了化 -----
    void GameStartState::Finalize()
    {
    }

    // ----- ImGui用 -----
    void GameStartState::DrawDebug()
    {
    }
}

// ----- OptionState -----
namespace TitleState
{
    // ----- 初期化 -----
    void OptionState::Initialize()
    {
        // 全UI非表示(リセット)
        owner_->GetUITitle()->SetIsDrawAllUI(false);


    }

    // ----- 更新 -----
    void OptionState::Update(const float& elapsedTime)
    {
    }

    // ----- 終了化 -----
    void OptionState::Finalize()
    {
    }

    // ----- ImGui用 -----
    void OptionState::DrawDebug()
    {
    }
}

// ----- QuitState -----
namespace TitleState
{
    // ----- 初期化 -----
    void QuitState::Initialize()
    {
        Application::GameEnd();
    }

    // ----- 更新 -----
    void QuitState::Update(const float& elapsedTime)
    {
    }

    // ----- 終了化 -----
    void QuitState::Finalize()
    {
    }

    // ----- ImGui用 -----
    void QuitState::DrawDebug()
    {
    }
}