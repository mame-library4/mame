#include "Fireball.h"
#include "ProjectileManager.h"
#include "MathHelper.h"

// ----- �R���X�g���N�^ -----
Fireball::Fireball()
    : Projectile("./Resources/Model/Sphere.gltf", 1.0f)
{
    ProjectileManager::Instance().Register(this);
}

// ----- �f�X�g���N�^ -----
Fireball::~Fireball()
{
}

// ----- ������ -----
void Fireball::Initialize()
{
    speed_ = 10.0f;
}

// ----- �I���� -----
void Fireball::Finalize()
{
}

// ----- �X�V -----
void Fireball::Update(const float& elapsedTime)
{
    GetTransform()->AddPosition(direction_ * speed_ * elapsedTime);
}

// ----- �`�� -----
void Fireball::Render(ID3D11PixelShader* psShader)
{
    Object::Render(psShader);
}

// ----- ImGui�p -----
void Fireball::DrawDebug()
{
    Object::DrawDebug();
}
