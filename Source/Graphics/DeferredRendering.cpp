#include "DeferredRendering.h"
#include "Graphics.h"

DeferredRendering::DeferredRendering()
{
    renderer_ = std::make_unique<FullscreenQuad>();

    Graphics::Instance().CreatePsFromCso("./Resources/Shader/DeferredRenderingPS.cso", psShader_.GetAddressOf());
}

void DeferredRendering::Draw()
{
    Graphics::Instance().SetBlendState(Shader::BLEND_STATE::ALPHA);
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_OFF_ZW_OFF);

    Graphics::Instance().GetShader()->SetGBufferShaderResourceView();
    renderer_->Draw(Graphics::Instance().GetShader()->GetGBufferBaseColorShaderResourceView(), 0, 1, psShader_.Get());
}
