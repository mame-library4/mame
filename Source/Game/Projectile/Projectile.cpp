#include "Projectile.h"

int Projectile::idCounter_ = 0;

// ----- �R���X�g���N�^ -----
Projectile::Projectile(const std::string filename, const float& scaleFactor)
    : Object(filename, scaleFactor)
{
    // �o�^�ԍ��ݒ�
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
