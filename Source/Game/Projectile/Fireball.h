#pragma once
#include "Projectile.h"

class Fireball : public Projectile
{
public:
    Fireball();
    ~Fireball();

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override;
    void DrawDebug()                                    override;

    void Launch(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction, const float& speed = 0.0f);

private:
    DirectX::XMFLOAT3 direction_ = {};
    float speed_ = 0.0f;

    int effectDelay_    = 0;
    int effectMaxDelay_ = 3;

    float lifeTimer_ = 0.0f;
};

