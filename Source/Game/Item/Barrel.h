#pragma once
#include "Item.h"

class Barrel : public Item
{
public:
    Barrel(const DirectX::XMFLOAT3& generatePosition);
    ~Barrel() override {}

    void Initialize()                                   override;
    void Finalize()                                     override;
    void Update(const float& elapsedTime)               override;
    void Render(ID3D11PixelShader* psShader = nullptr)  override;
    void DrawDebug()                                    override;
    void OnHit()                                        override;

};

