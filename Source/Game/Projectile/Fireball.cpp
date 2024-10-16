#include "Fireball.h"
#include "ProjectileManager.h"
#include "MathHelper.h"
#include "Effect/EffectManager.h"
#include "Particle/ParticleManager.h"

// ----- �R���X�g���N�^ -----
Fireball::Fireball()
    : Projectile("./Resources/Model/Sphere.gltf", 1.0f)
{
    // Manager�Ɏ������g��o�^
    ProjectileManager::Instance().Register(this);

    fireBallParticle_ = new FireBallParticle();
}

// ----- �f�X�g���N�^ -----
Fireball::~Fireball()
{
}

// ----- ������ -----
void Fireball::Initialize()
{
    // �T�C�Y�ݒ�
    GetTransform()->SetScaleFactor(0.5f);

    // �����蔻��̔��a�ݒ�
    SetRadius(0.5f);

    // �J�E���^�[����p�̔��a�ݒ�
    SetCounterRadius(2.0f);

    // �_���[�W�ݒ�
    SetDamage(40);

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

    // �G�t�F�N�g�X�V
    if (fireBallParticle_->GetIsHit() == false)
    {
        ++effectDelay_;
        if (effectDelay_ > effectMaxDelay_)
        {
            EffectManager::Instance().GetEffect("Fire")->Play(GetTransform()->GetPosition(), 0.5f, 3.0f);

            effectDelay_ = 0;
        }
    }

    lifeTimer_ += elapsedTime;
    if (lifeTimer_ > 10.0f)
    {
        //ProjectileManager::Instance().Remove(this);
    }

    fireBallParticle_->UpdateFireBallParticle(GetTransform()->GetPosition());
}

// ----- �`�� -----
void Fireball::Render(ID3D11PixelShader* psShader)
{
    //Object::Render(psShader);
}

// ----- ImGui�p -----
void Fireball::DrawDebug()
{
    std::string name = "Fireball" + std::to_string(GetId());
    if (ImGui::TreeNode(name.c_str()))
    {
        Projectile::DrawDebug();
        ImGui::TreePop();
    }
}

// ----- �����������ɌĂяo����鏈�� -----
void Fireball::OnHit()
{
    fireBallParticle_->SetToExplode(); // �G�t�F�N�g�̓����𔚔��ɐ؂�ւ���
}

// ----- ���� -----
void Fireball::Launch(const float& elapsedTime, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, const float& speed)
{
    fireBallParticle_->PlayFireBallParticle(elapsedTime, position);

    GetTransform()->SetPosition(position);
    direction_ = direction;
    
    speed_ = (speed == 0.0f) ? speed_ : speed;
}
