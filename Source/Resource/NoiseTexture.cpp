#include "NoiseTexture.h"

#include "texture.h"

#include "../Graphics/Graphics.h"

// コンストラクタ
NoiseTexture::NoiseTexture()
{
    Graphics& graphics = Graphics::Instance();

    // noiseTexture Load
    D3D11_TEXTURE2D_DESC noiseTexture2dDesc{};

    LoadTextureFromFile(graphics.GetDevice(),
        L"./Resources/Image/Mask/Fade/mask1.png",
        noiseTexture[0].GetAddressOf(), &noiseTexture2dDesc);
}

// constantBuffer set
void NoiseTexture::SetConstantBuffers(int slot)
{
    Graphics& graphics = Graphics::Instance();

    graphics.GetDeviceContext()->PSSetShaderResources(slot,
        _countof(noiseTexture), noiseTexture->GetAddressOf());
}
