#include "PostProcess.h"
#include "Graphics.h"
#include "Application.h"

// ----- コンストラクタ -----
PostProcess::PostProcess()
    : cascadedShadowMap_(1024 * 4, 1024 * 4)
{
    renderer_ = std::make_unique<FullscreenQuad>();

    Graphics::Instance().CreatePsFromCso("./Resources/Shader/PostProcessPS.cso", postProcessPS_.GetAddressOf());
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/RadialBlurPS.cso", radialBlurPS_.GetAddressOf());

    postProcess_ = std::make_unique<FrameBuffer>(SCREEN_WIDTH, SCREEN_HEIGHT);
    radialBlur_  = std::make_unique<FrameBuffer>(SCREEN_WIDTH, SCREEN_HEIGHT);

    constant_            = std::make_unique<ConstantBuffer<Constants>>();
    radialBlurConstants_ = std::make_unique<ConstantBuffer<RadialBlurConstants>>();
}

// ----- デストラクタ -----
PostProcess::~PostProcess()
{
}

// ----- ポストプロセス開始 -----
void PostProcess::Activate()
{
    postProcess_->Clear();
    postProcess_->Activate(Graphics::Instance().GetShader()->GetGBufferDepthStencilView());
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

    // ラジアルブラーを使用する
    if (useRadialBlur_)
    {
        radialBlur_->Clear();
        radialBlur_->Activate();
        renderer_->Draw(shaderResourceViews, 0, _countof(shaderResourceViews), postProcessPS_.Get());
        radialBlur_->Deactivate();

        radialBlurConstants_->Activate(0);
        renderer_->Draw(radialBlur_->shaderResourceViews_->GetAddressOf(), 0, 1, radialBlurPS_.Get());
    }
    // ラジアルブラーを使用しない
    else
    {
        renderer_->Draw(shaderResourceViews, 0, _countof(shaderResourceViews), postProcessPS_.Get());
    }
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
            ImGui::Image(reinterpret_cast<ImTextureID>(postProcess_->shaderResourceViews_[0].Get()), ImVec2(256.0, 256.0));

            bloom_.DrawDebug();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("RadialBlur"))
        {
            ImGui::DragFloat2("UVOffset", &radialBlurConstants_->GetData()->uvOffset_.x, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Strength", &radialBlurConstants_->GetData()->strength_, 0.1f, 0.0f, 2.0f);
            ImGui::DragInt("SampleCount", &radialBlurConstants_->GetData()->sampleCount_, 1, 1, 5);

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
