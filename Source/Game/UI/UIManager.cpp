#include "UIManager.h"
#include "UI.h"
#include "Common.h"

// ----- �X�V -----
void UIManager::Update(const float& elapsedTime)
{
    // ����
    for (UI* ui : generates_)
    {
        userInterface_.emplace_back(ui);
    }
    generates_.clear();

    for (UI*& ui : userInterface_)
    {
        ui->Update(elapsedTime);
    }

    // �j��
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

// ----- �`�� -----
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

// ----- ImGui�p -----
void UIManager::DrawDebug()
{
#ifdef _DEBUG
    if (ImGui::BeginMainMenuBar())
    {

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

        ImGui::EndMainMenuBar();
    }
#endif
}

// ----- �o�^ -----
void UIManager::Register(UI* ui)
{
    // ���ʔj��UI�̏ꍇUI���m�����Ȃ��悤��
    // ��ɐ�������Ă��镔�ʔj��UI�̈ʒu�����炷
    if (ui->GetType() == UIType::UINotifications)
    {
        for (UI*& userInterface : userInterface_)
        {
            if (userInterface->GetType() != UIType::UINotifications) continue;

            // �ړ��ʂ�ݒ肷��
            const float moveValue = -60.0f;
            userInterface->GetTransform()->AddPositionY(moveValue);
        }
    }

    generates_.insert(ui);
}

// ----- �폜 �w���UI -----
void UIManager::Remove(UI* ui)
{
    removes_.insert(ui);
}

// ----- �폜 �w��̎�ނ�UI�S�� -----
void UIManager::Remove(const UIType& type)
{
    for (UI*& ui : userInterface_)
    {
        if (ui->GetType() != type) continue;

        removes_.insert(ui);
    }
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
