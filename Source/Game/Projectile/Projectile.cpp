#include "Projectile.h"

int Projectile::idCounter_ = 0;

// ----- コンストラクタ -----
Projectile::Projectile(const std::string filename, const float& scaleFactor)
    : Object(filename, scaleFactor)
{
    // 登録番号設定
    id_ = idCounter_++;
}

Projectile::~Projectile()
{
}

void Projectile::DrawDebug()
{
    Object::DrawDebug();

    ImGui::DragFloat("Radius", &radius_);
    ImGui::DragFloat("CounterRadius", &counterRadius_);
}
