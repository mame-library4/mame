#include "SystemManager.h"
#include "Camera.h"

// ----- ImGui—p -----
void SystemManager::DrawDebug()
{
    if (ImGui::BeginMenu("SystemManager"))
    {
        ImGui::DragFloat("AllSlowSpeed", &allSlowSpeed_);
        ImGui::DragFloat("PlayerSlowSpeed", &playerSlowSpeed_);

        ImGui::EndMenu();
    }
}
