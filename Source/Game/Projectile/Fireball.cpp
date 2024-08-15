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
    GetTransform()->SetScaleFactor(0.5f);

    speed_ = 25.0f;
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

// ----- ���� -----
void Fireball::Launch(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, const float& speed)
{
    GetTransform()->SetPosition(position);
    direction_ = direction;
    
    speed_ = (speed == 0.0f) ? speed_ : speed;
}
