#include "NoiseTexture.h"
#include "Texture.h"
#include "Graphics.h"

// コンストラクタ
NoiseTexture::NoiseTexture()
{
    D3D11_TEXTURE2D_DESC noiseTexture2dDesc = {};

    Texture::Instance().LoadTexture(L"./Resources/Image/Mask/Fade/mask1.png",
        noiseTexture[0].GetAddressOf(), &noiseTexture2dDesc);
}

// constantBuffer set
void NoiseTexture::SetConstantBuffers(int slot)
{
    Graphics& graphics = Graphics::Instance();

    graphics.GetDeviceContext()->PSSetShaderResources(slot,
        _countof(noiseTexture), noiseTexture->GetAddressOf());
}
