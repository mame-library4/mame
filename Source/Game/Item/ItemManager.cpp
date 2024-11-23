#include "ItemManager.h"
#include "Common.h"
#include "MathHelper.h"

// ----- ‰Šú‰» -----
void ItemManager::Initialize()
{
    for (Item*& item : items_)
    {
        item->Initialize();
    }
}

// ----- I—¹‰» -----
void ItemManager::Finalize()
{
    for (Item*& item : items_)
    {
        item->Finalize();
    }
    Clear();
}

// ----- XV -----
void ItemManager::Update(const float& elapsedTime)
{
    // -------------------------
    //          ¶¬
    // -------------------------
    for (Item* item : generates_)
    {
        items_.emplace_back(item);
        item->Initialize();
    }
    generates_.clear();


    // -------------------------
    //          XV
    // -------------------------
    for (Item*& item : items_)
    {
        item->Update(elapsedTime);
    }

    // -------------------------
    //          ”jŠü
    // -------------------------
    for (Item* item : removes_)
    {
        auto it = std::find(items_.begin(), items_.end(), item);

        if (it != items_.end())
        {
            items_.erase(it);
        }

        SafeDeletePtr(item);
    }
    removes_.clear();
}

// ----- •`‰æ -----
void ItemManager::Render(ID3D11PixelShader* psShader)
{
    for (Item*& item : items_)
    {
        item->Render(psShader);
    }
}

// ----- ImGui—p -----
void ItemManager::DrawDebug()
{
    if (ImGui::BeginMenu("ItemManager"))
    {
        for (Item*& item : items_)
        {
            item->DrawDebug();
        }

        ImGui::EndMenu();
    }
}

// ----- “o˜^ -----
void ItemManager::DebugRender(DebugRenderer* debugRenderer)
{
    for (Item*& item : items_)
    {
        DirectX::XMFLOAT3 position = item->GetTransform()->GetPosition() + item->GetOffsetPosition();
        debugRenderer->DrawSphere(position, item->GetDamageRadius(), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
    }
}

void ItemManager::Register(Item* item)
{
    generates_.insert(item);
}

// ----- íœ -----
void ItemManager::Remove(Item* item)
{
    removes_.insert(item);
}

// ----- ‘Síœ -----
void ItemManager::Clear()
{
    for (Item*& item : items_)
    {
        SafeDeletePtr(item);
    }
    items_.clear();
    items_.shrink_to_fit();
}

Item* ItemManager::GetItem(const int& id)
{
    for (Item*& item : items_)
    {
        if (item->GetId() == id) return item;
    }

    return nullptr;
}
