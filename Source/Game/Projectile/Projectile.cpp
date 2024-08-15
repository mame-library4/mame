#include "Projectile.h"

int Projectile::idCounter_ = 0;

// ----- �R���X�g���N�^ -----
Projectile::Projectile(const std::string filename, const float& scaleFactor)
    : Object(filename, scaleFactor)
{
    id_ = idCounter_++;
}

Projectile::~Projectile()
{
    --idCounter_;
}
