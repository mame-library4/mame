#include "UISystem.h"

// ----- コンストラクタ -----
UISystem::UISystem()
    : UI(UIManager::UIType::UISystem, L"./Resources/Image/White.png", "UISystem")
{
    
    // 初期化
    Initialize();

    // 描画フラグを立てる
    SetIsDraw();
}

// ----- 初期化 -----
void UISystem::Initialize()
{
    GetTransform()->SetColor(0.0f, 0.0f, 0.0f, 0.5f);
}

// ----- 更新 -----
void UISystem::Update(const float& elapsedTime)
{
}

// ----- 描画 -----
void UISystem::Render()
{
    UI::Render();
}

// ----- ImGui用 -----
void UISystem::DrawDebug()
{
    if (ImGui::TreeNode(GetName().c_str()))
    { 
        UI::DrawDebug();

        ImGui::TreePop();
    }
}
