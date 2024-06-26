#include "SkyMap.h"
#include "Graphics.h"
#include "Texture.h"

// ----- コンストラクタ -----
SkyMap::SkyMap()
{
    D3D11_TEXTURE2D_DESC desc = {};
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/SkyMapPS.cso", psShader_.GetAddressOf());
    //Texture::Instance().LoadTexture(L"./Resources/SkyBox/RainSky/skybox.dds", shaderResourceView_.GetAddressOf(), &desc);
    Texture::Instance().LoadTexture(L"./Resources/Image/skyMap.dds", shaderResourceView_.GetAddressOf(), &desc);
    renderer_ = std::make_unique<FullscreenQuad>();
}

void SkyMap::Render()
{
    Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_OFF_ZW_OFF);
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    renderer_->Draw(shaderResourceView_.GetAddressOf(), 0, 1, psShader_.Get());
}
