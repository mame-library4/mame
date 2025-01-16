#include "Item.h"
#include "ItemManager.h"

// ----- コンストラクタ -----
Item::Item(const std::string filename, const float& scaleFactor, const std::string& name, const int& itemType)
    : Object(filename, scaleFactor), itemType_(itemType)
{
    // マネージャーに登録
    ItemManager::Instance().Register(this);

    // 登録番号設定
    id_ = ItemManager::Instance().GetMyID();

    // 名前を設定
    name_ = name + std::to_string(id_);
}

// ----- ImGui用 -----
void Item::DrawDebug()
{
    Object::DrawDebug();
}
