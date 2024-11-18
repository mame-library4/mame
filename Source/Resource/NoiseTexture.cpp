#include "NoiseTexture.h"
#include "Texture.h"
#include "Graphics.h"

// コンストラクタ
NoiseTexture::NoiseTexture()
{
    D3D11_TEXTURE2D_DESC noiseTexture2dDesc = {};

    Texture::Instance().LoadTexture(L"./Resources/Image/Mask/Noise.png", noiseTexture[0].GetAddressOf(), &noiseTexture2dDesc);
    Texture::Instance().LoadTexture(L"./Resources/Image/Mask/Mask.png",  noiseTexture[1].GetAddressOf(), &noiseTexture2dDesc);
    Texture::Instance().LoadTexture(L"./Resources/Image/Mask/Mask1.png", noiseTexture[2].GetAddressOf(), &noiseTexture2dDesc);
}

void NoiseTexture::PSSetShaderResourceView(const int& slot, const int& num)
{
    Graphics::Instance().GetDeviceContext()->PSSetShaderResources(slot, 1, noiseTexture[num].GetAddressOf());
}
