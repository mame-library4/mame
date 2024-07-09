#include "UIManager.h"
#include "Common.h"

// ----- XV -----
void UIManager::Update(const float& elapsedTime)
{
    // ¶¬
    for (UI* ui : generates_)
    {
        userInterface_.emplace_back(ui);
    }
    generates_.clear();

    for (UI*& ui : userInterface_)
    {
        ui->Update(elapsedTime);
    }

    // jü
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

// ----- `æ -----
void UIManager::Render()
{
    for (UI*& ui : userInterface_)
    {
        ui->Render();
    }
}

// ----- ImGuip -----
void UIManager::DrawDebug()
{
    for (UI*& ui : userInterface_)
    {
        ui->DrawDebug();
    }
}

// ----- o^ -----
void UIManager::Register(UI* ui)
{
    generates_.insert(ui);
}

// ----- [u -----
void UIManager::Remove(UI* ui)
{
    removes_.insert(ui);
}

void UIManager::Clear()
{
    for (UI*& ui : userInterface_)
    {
        SafeDeletePtr(ui);
    }
    userInterface_.clear();
    userInterface_.shrink_to_fit();
}
