#include "UIManager.h"
#include "UI.h"
#include "Common.h"

// ----- 更新 -----
void UIManager::Update(const float& elapsedTime)
{
    // 生成
    for (UI* ui : generates_)
    {
        userInterface_.emplace_back(ui);
    }
    generates_.clear();

    for (UI*& ui : userInterface_)
    {
        ui->Update(elapsedTime);
    }

    // 破棄
    for (UI* ui : removes_)
    {
        auto it = std::find(userInterface_.begin(), userInterface_.end(), ui);

        if (it != userInterface_.end())
        {
            userInterface_.erase(it);
        }

        SafeDeletePtr(ui);
    }
    removes_.clear();
}

// ----- 描画 -----
void UIManager::Render()
{
    for (int i = 0; i < static_cast<int>(UIType::Max); ++i)
    {
        for (UI*& ui : userInterface_)
        {
            if (ui->GetType() != static_cast<UIType>(i)) continue;

            ui->Render();
        }
    }
}

// ----- ImGui用 -----
void UIManager::DrawDebug()
{
#ifdef _DEBUG
    if (ImGui::BeginMenu("UIManager"))
    {
        for (int i = 0; i < userInterface_.size(); ++i)
        {
            userInterface_.at(i)->DrawDebug();
        }

        //for (UI*& ui : userInterface_)
        //{
        //    ui->DrawDebug();
        //}
        ImGui::EndMenu();
    }
#endif
}

// ----- 登録 -----
void UIManager::Register(UI* ui)
{
    // 部位破壊UIの場合UI同士が被らないように
    // 先に生成されている部位破壊UIの位置をずらす
    if (ui->GetType() == UIType::UINotifications)
    {
        for (UI*& userInterface : userInterface_)
        {
            if (userInterface->GetType() != UIType::UINotifications) continue;

            // 移動量を設定する
            const float moveValue = -60.0f;
            userInterface->GetTransform()->AddPositionY(moveValue);
        }
    }

    generates_.insert(ui);
}

// ----- 削除 指定のUI -----
void UIManager::Remove(UI* ui)
{
    removes_.insert(ui);
}

// ----- 削除 指定の種類のUI全て -----
void UIManager::Remove(const UIType& type)
{
    for (UI*& ui : userInterface_)
    {
        if (ui->GetType() != type) continue;

        removes_.insert(ui);
    }
}

// ----- 全削除 -----
void UIManager::Clear()
{
    for (UI*& ui : userInterface_)
    {
        SafeDeletePtr(ui);
    }
    userInterface_.clear();
    userInterface_.shrink_to_fit();
}

// ----- 全UIフェードアウトする -----
void UIManager::PlayAllUIFadeOut()
{
    for (UI*& ui : userInterface_)
    {
        ui->SetFadeOut();
    }
}

UI* UIManager::GetUI(const UIType& type)
{
    for (UI*& ui : userInterface_)
    {
        if (ui->GetType() != type) continue;

        return ui;
    }

    return nullptr;
}
