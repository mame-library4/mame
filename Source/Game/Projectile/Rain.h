#pragma once
#include "Projectile.h"

class Rain : public Projectile
{
public:
    Rain();
    ~Rain() override {}

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override {}
    void DrawDebug()                                    override;
    [[nodiscard]] const bool OnHit(const DirectX::XMFLOAT3& hitPosition)    override;

    // ----- ”­ŽË -----
    void Launch(const DirectX::XMFLOAT3& moveDirection, const float& moveSpeed);

private:
    DirectX::XMFLOAT3   moveDirection_  = {};
    float               moveSpeed_      = 0.0f;
};

