#include "Projectile.h"
#include "ProjectileManager.h"

// ----- コンストラクタ -----
Projectile::Projectile(const std::string filename, const float& scaleFactor, const std::string& name, const int drawType)
    : Object(filename, scaleFactor), drawType_(drawType)
{
    // マネージャーに登録
    ProjectileManager::Instance().Register(this);

    // 登録番号設定
    id_ = ProjectileManager::Instance().GetMyID();

    // 名前を設定
    name_ = name + std::to_string(id_);
}

void Projectile::DrawDebug()
{
    Object::DrawDebug();

    ImGui::DragFloat("Radius", &radius_);
    ImGui::DragFloat("CounterRadius", &counterRadius_);
}
