#include "FireBallParticle.h"
#include "Graphics.h"
#include "Texture.h"
#include "Misc.h"
#include "MathHelper.h"
#include "GameScene.h"

// ----- コンストラクタ -----
FireBallParticle::FireBallParticle()
	: ParticleSystem(3000)
{
	GetParticleData()->CreateParticleData(sizeof(Particle), sizeof(Constants), "./Resources/Shader/ParticleVS.cso", "./Resources/Shader/FireBallPS.cso",
		"./Resources/Shader/FireBallGS.cso", "./Resources/Shader/FireBallInitializeCS.cso", "./Resources/Shader/FireBallUpdateCS.cso");

	constants_.speed_ = 20.0f;
	constants_.particleSize_ = 0.05f;
}

void FireBallParticle::Update(const float& elapsedTime)
{
	constants_.time_ += elapsedTime;
	constants_.deltaTime_ = elapsedTime;

	GetParticleData()->Update(csUAVSlot_, cbSlot_, &constants_);
}

void FireBallParticle::Render()
{
	GetParticleData()->Render(gsSRVSlot_, cbSlot_, &constants_);
}

void FireBallParticle::DrawDebug()
{
}

void FireBallParticle::PlayFireBallParticle(const float& elapsedTime, const DirectX::XMFLOAT3& emitterPosition)
{
	constants_.emitterPosition_ = emitterPosition;
	constants_.currentPosition_ = emitterPosition;
	constants_.oldPosition_		= emitterPosition;
	constants_.time_ += elapsedTime;
	constants_.deltaTime_ = elapsedTime;

	GetParticleData()->PlayParticle(csUAVSlot_, cbSlot_, &constants_);
}

void FireBallParticle::UpdateFireBallParticle(const DirectX::XMFLOAT3& position)
{
	if (constants_.isMoveStraight == 0) return;

	constants_.oldPosition_ = constants_.currentPosition_;
	constants_.currentPosition_ = position;

	// 地面に当たった
	if (position.y < 0.0f) SetToExplode();

	// 壁に当たった
	DirectX::XMFLOAT2 pos = { position.x, position.z };
	const float length = XMFloat2Length(pos);
	if (GameScene::stageRadius_ < length) SetToExplode();
}

void FireBallParticle::SetToExplode()
{
	constants_.isMoveStraight = 0;
}
