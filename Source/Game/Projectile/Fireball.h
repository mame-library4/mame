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

    void SetDirection(const DirectX::XMFLOAT3& direction) { direction_ = direction; }
    void SetSpeed(const float& speed) { speed_ = speed; }

private:
    DirectX::XMFLOAT3 direction_ = {};
    float speed_ = 0.0f;
};

