#include "SystemManager.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Input.h"
#include "Character/Player/PlayerManager.h"
#include "UI/UISystem.h"

// ----- 更新 -----
const bool SystemManager::Update()
{
    // ポーズの判定
    if (UpdatePauseState()) return false;

    // Player役職変更
    UpdatePlayerRole();

    return true;
}

// ----- ImGui用 -----
void SystemManager::DrawDebug()
{
    if (ImGui::BeginMenu("SystemManager"))
    {
        ImGui::DragFloat("AllSlowSpeed", &allSlowSpeed_);
        ImGui::DragFloat("PlayerSlowSpeed", &playerSlowSpeed_);

        ImGui::EndMenu();
    }
}

// ----- ポーズの更新処理 -----
const bool SystemManager::UpdatePauseState()
{
    // 入力チェック
    if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_START)
    {
        // ポーズする
        if (isPaused_ == false)
        {
            UISystem* uiSystem = new UISystem();
            isPaused_ = true;
        }
        // ポーズを解除する
        else
        {
            UIManager::Instance().Remove(UIManager::UIType::UISystem);
            isPaused_ = false;
        }
    }

    // ポーズ中ならtrueを返す
    return isPaused_;
}

// ----- Player役職変更 -----
void SystemManager::UpdatePlayerRole()
{
    // プレイヤーの役職を "剣士" に変更
    if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_UP)
    {
        PlayerManager::Instance().GetPlayer()->ChangePlayerRole(Player::PlayerRole::SwordsMan);
    }
    // プレイヤーの役職を "魔法使い" に変更
    if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_RIGHT)
    {
        PlayerManager::Instance().GetPlayer()->ChangePlayerRole(Player::PlayerRole::Mage);
    }
}
