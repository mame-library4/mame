#include "PostProcess.h"
#include "Graphics.h"
#include "Application.h"

// ----- コンストラクタ -----
PostProcess::PostProcess()
    : cascadedShadowMap_(1024 * 4, 1024 * 4)
{
    renderer_ = std::make_unique<FullscreenQuad>();

    Graphics::Instance().CreatePsFromCso("./Resources/Shader/PostProcessPS.cso", postProcessPS_.GetAddressOf());
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/PostProcessRoarPS.cso", roarPS_.GetAddressOf());

    postProcess_ = std::make_unique<FrameBuffer>(SCREEN_WIDTH, SCREEN_HEIGHT);

    constant_ = std::make_unique<ConstantBuffer<Constants>>();
}

// ----- デストラクタ -----
PostProcess::~PostProcess()
{
}

// ----- ポストプロセス開始 -----
void PostProcess::Activate()
{
    postProcess_->Clear();
    postProcess_->Activate();
}

// ----- ポストプロセス終了 -----
void PostProcess::Deactivate()
{
    postProcess_->Deactivate();
    bloom_.Execute(postProcess_->shaderResourceViews_[0].Get());
}

// ----- ポストプロセス描画 -----
void PostProcess::Draw()
{
    constant_->Activate(2);

    Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_OFF_ZW_OFF);
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    Graphics::Instance().SetBlendState(Shader::BLEND_STATE::NONE);

    ID3D11ShaderResourceView* shaderResourceViews[] =
    {
        postProcess_->shaderResourceViews_[0].Get(),
        Graphics::Instance().GetShader()->GetDepthMap().Get(),
        bloom_.GetColorMap().Get(),
        cascadedShadowMap_.GetDepthMap().Get()
    };

    renderer_->Draw(shaderResourceViews, 0, _countof(shaderResourceViews), postProcessPS_.Get());

#if 0

    if (useRadialBlur_)
    {
        renderer_->Draw(bloom_.GetShaderResourceView(), 0, 1, roarPS_.Get());
    }
    else
    {
        renderer_->Draw(bloom_.GetShaderResourceView(), 0, 1, postProcessPS_.Get());
    }
#endif
}

// ----- ImGui用 -----
void PostProcess::DrawDebug()
{
    if (ImGui::TreeNode("PostProcess"))
    {
        ImGui::DragFloat("CriticalDepthValue", &criticalDepthValue_);

        ImGui::SliderFloat("ShadowColor", &constant_->GetData()->shadowColor_, 0.0f, 1.0f);
        ImGui::DragFloat("ShadowDepthBias", &constant_->GetData()->shadowDepthBias_, 0.00001f, 0.0f, 0.01f, "%.8f");
        ImGui::Checkbox("ColorizeCascadedLayer", &constant_->GetData()->colorizeCascadedLayer_);

        cascadedShadowMap_.DrawDebug();

        if (ImGui::TreeNode("Bloom_"))
        {
            ImGui::DragFloat("blurPower", &constant_->GetData()->blurPower_, 0.01f);

            ImGui::Image(reinterpret_cast<ImTextureID>(postProcess_->shaderResourceViews_[0].Get()), ImVec2(256.0, 256.0));

            bloom_.DrawDebug();

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}

// ----- Make CascadedShadowMap -----
void PostProcess::MakeCascadedShadowMap(const DirectX::XMFLOAT4& lightDirection, UINT cbSlot, std::function<void()> drawcallback)
{
    cascadedShadowMap_.Clear();
    cascadedShadowMap_.Activate(lightDirection, criticalDepthValue_, cbSlot);

    Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_ON_ZW_ON);
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    Graphics::Instance().SetBlendState(Shader::BLEND_STATE::NONE);

    drawcallback();

    cascadedShadowMap_.Deactivate();
}
