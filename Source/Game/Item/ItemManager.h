#pragma once
#include "Item.h"
#include <vector>
#include <set>

class ItemManager
{
private:
    ItemManager() {} 
    ~ItemManager() {}

public:
    static ItemManager& Instance()
    {
        static ItemManager instance;
        return instance;
    }

    void Initialize();                                  // ‰Šú‰»
    void Finalize();                                    // I—¹‰»
    void Update(const float& elapsedTime);              // XV
    void Render(ID3D11PixelShader* psShader = nullptr); // •`‰æ
    void DrawDebug();                                   // ImGui—p
    void DebugRender(DebugRenderer* debugRenderer);

    void Register(Item* item);
    void Remove(Item* item);
    void Clear();

    std::vector<Item*> GetItems() { return items_; }
    Item* GetItem(const int& id);
    [[nodiscard]] const int GetItemCount() const { return items_.size(); }

    // ----- ID“o˜^—p -----
    [[nodiscard]] const int GetMyID() { return idCounter_++; }

private:
    std::vector<Item*>    items_;
    std::set<Item*>       generates_;
    std::set<Item*>       removes_;

    int idCounter_ = 0;
};