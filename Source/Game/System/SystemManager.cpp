#include "SystemManager.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Input.h"
#include "Character/Player/PlayerManager.h"

void SystemManager::Update()
{
    // ゲームシーンの場合
    if (SceneManager::Instance().GetCurrentSceneName() == SceneManager::SceneName::Game)
    {
        if (Input::Instance().GetGamePad().GetButtonDown() & GamePad::BTN_START)
        {
            PlayerManager::Instance().GetPlayer()->ChangePlayerRole();
        }
    }
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
