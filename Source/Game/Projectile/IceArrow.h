#pragma once
#include "Projectile.h"
#include "ComputeParticle/ComputeParticleEmitter.h"

class IceArrow : public Projectile
{
public:
    IceArrow();
    ~IceArrow() override {}

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override;
    void DrawDebug()                                    override;
    void OnHit()                                        override;

    void SetIsDrawActive(const bool& flag) { isDrawActive_ = flag; }
    [[nodiscard]] const bool GetIsDrawActive() const { return isDrawActive_; }

private:
    bool isDrawActive_ = false;

    Microsoft::WRL::ComPtr<ID3D11PixelShader> iceArrowPS_;

    DirectX::XMFLOAT3 iceArrowLocation_ = { 5000.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 iceArrowRotation_ = { 0.0f, -90.0f, 0.0f };
    DirectX::XMFLOAT3 iceArrowScale_ = { 0.8f, 0.8f, -0.8f };
    DirectX::XMFLOAT4X4 iceArrowWorld_ = {};
};

