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
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/VignettePS.cso", vignettePS_.GetAddressOf());
    Graphics::Instance().CreatePsFromCso("./Resources/Shader/toneMapPS.cso", toneMapPS_.GetAddressOf());

    
    sceneBuffer_ = std::make_unique<FrameBuffer>(SCREEN_WIDTH, SCREEN_HEIGHT);
    postProcess_ = std::make_unique<FrameBuffer>(SCREEN_WIDTH, SCREEN_HEIGHT);
    radialBlur_  = std::make_unique<FrameBuffer>(SCREEN_WIDTH, SCREEN_HEIGHT);
    vignette_    = std::make_unique<FrameBuffer>(SCREEN_WIDTH, SCREEN_HEIGHT);

    postEffectConstants_ = std::make_unique<ConstantBuffer<PostEffectConstants>>();

    constant_            = std::make_unique<ConstantBuffer<Constants>>();
    radialBlurConstants_ = std::make_unique<ConstantBuffer<RadialBlurConstants>>();
    vignetteConstants_   = std::make_unique<ConstantBuffer<VignetteConstants>>();
}

// ----- デストラクタ -----
PostProcess::~PostProcess()
{
}

// ----- ポストプロセス開始 -----
void PostProcess::Activate()
{
    sceneBuffer_->Clear();
    sceneBuffer_->Activate(Graphics::Instance().GetShader()->GetGBufferDepthStencilView());
}

// ----- ポストプロセス終了 -----
void PostProcess::Deactivate()
{
    sceneBuffer_->Deactivate();
}

// ----- ポストプロセス描画 -----
void PostProcess::Draw()
{
    constant_->Activate(2);
    bloom_.Execute(sceneBuffer_->GetColorMap().Get());
    
    Graphics::Instance().SetDepthStencileState(Shader::DEPTH_STATE::ZT_OFF_ZW_OFF);
    Graphics::Instance().SetRasterizerState(Shader::RASTER_STATE::CULL_NONE);
    Graphics::Instance().SetBlendState(Shader::BLEND_STATE::NONE);

    ID3D11ShaderResourceView* shaderResourceViews[] =
    {
        sceneBuffer_->GetColorMap().Get(),
        Graphics::Instance().GetShader()->GetDepthMap().Get(),
        bloom_.GetColorMap().Get(),
        cascadedShadowMap_.GetDepthMap().Get(),
    };
    postProcess_->Clear();
    postProcess_->Activate();


    if (useVignette_ && useRadialBlur_)
    {
        UpdateVignette([&]() { UpdateRadialBler([&]() { renderer_->Draw(shaderResourceViews, 0, _countof(shaderResourceViews), postProcessPS_.Get()); }); });
    }
    else if (useVignette_)
    {
        UpdateVignette([&]() { renderer_->Draw(shaderResourceViews, 0, _countof(shaderResourceViews), postProcessPS_.Get()); });
    }
    else if (useRadialBlur_)
    {
        UpdateRadialBler([&]() { renderer_->Draw(shaderResourceViews, 0, _countof(shaderResourceViews), postProcessPS_.Get()); });
    }
    else
    {
        renderer_->Draw(shaderResourceViews, 0, _countof(shaderResourceViews), postProcessPS_.Get());
    }    

    postProcess_->Deactivate();

    // ToneMapping
    {
        postEffectConstants_->Activate(8);
        renderer_->Draw(postProcess_->GetColorMap().GetAddressOf(), 0, 1, toneMapPS_.Get());
    }
}

// ----- ImGui用 -----
void PostProcess::DrawDebug()
{
    if (ImGui::BeginMenu("PostProcess"))
    {
        if (ImGui::TreeNode("ToneMap"))
        {
            ImGui::ColorEdit3("Colrize", &postEffectConstants_->GetData()->colorize_.x);
            ImGui::DragFloat("Exposure", &postEffectConstants_->GetData()->exposure_, 0.01f, 0.0f, 10.0f);

            ImGui::DragFloat("Brightness", &postEffectConstants_->GetData()->brightness_, 0.01f, -1.0f, 1.0f);
            ImGui::DragFloat("Contrast", &postEffectConstants_->GetData()->contrast_, 0.01f, -1.0f, 1.0f);
            ImGui::DragFloat("Hue", &postEffectConstants_->GetData()->hue_, 0.01f, -1.0f, 1.0f);
            ImGui::DragFloat("Saturation", &postEffectConstants_->GetData()->saturation_, 0.01f, -1.0f, 1.0f);

            ImGui::TreePop();
        }


        ImGui::DragFloat("CriticalDepthValue", &criticalDepthValue_);

        ImGui::SliderFloat("ShadowColor", &constant_->GetData()->shadowColor_, 0.0f, 1.0f);
        ImGui::DragFloat("ShadowDepthBias", &constant_->GetData()->shadowDepthBias_, 0.00001f, 0.0f, 0.01f, "%.8f");
        ImGui::Checkbox("ColorizeCascadedLayer", &constant_->GetData()->colorizeCascadedLayer_);

        cascadedShadowMap_.DrawDebug();

        if (ImGui::TreeNode("Bloom"))
        {
            ImGui::Image(reinterpret_cast<ImTextureID>(postProcess_->GetColorMap().Get()), ImVec2(256.0, 256.0));

            bloom_.DrawDebug();

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("RadialBlur"))
        {
            ImGui::Checkbox("UseRadialBlur", &useRadialBlur_);
            ImGui::DragFloat2("UVOffset", &radialBlurConstants_->GetData()->uvOffset_.x, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Strength", &radialBlurConstants_->GetData()->strength_, 0.1f, 0.0f, 2.0f);
            ImGui::DragInt("SampleCount", &radialBlurConstants_->GetData()->sampleCount_, 1, 1, 5);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Vignette"))
        {
            ImGui::Checkbox("UseVignette", &useVignette_);
            ImGui::ColorEdit4("Color", &vignetteConstants_->GetData()->vignetteColor_.x);
            ImGui::DragFloat2("Center", &vignetteConstants_->GetData()->vignetteCenter_.x);
            ImGui::DragFloat("Intensity", &vignetteConstants_->GetData()->vignetteIntensity_, 0.1f, 0.0f, 10.0f);
            ImGui::DragFloat("Smoothness", &vignetteConstants_->GetData()->vignetteSmoothness_, 0.1f, 0.0f, 10.0f);
            ImGui::DragFloat("Rounded", &vignetteConstants_->GetData()->vignetteRounded_, 0.1f, 0.0f, 10.0f);
            ImGui::DragFloat("Roundness", &vignetteConstants_->GetData()->vignetteRoundness_, 0.1f, 0.0f, 10.0f);

            ImGui::TreePop();
        }

        ImGui::EndMenu();
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

void PostProcess::UpdateRadialBler(std::function<void()> drawcallback)
{
    radialBlur_->Clear();
    radialBlur_->Activate();

    drawcallback();

    radialBlur_->Deactivate();

    radialBlurConstants_->Activate(0);
    renderer_->Draw(radialBlur_->GetColorMap().GetAddressOf(), 0, 1, radialBlurPS_.Get());
}

void PostProcess::UpdateVignette(std::function<void()> drawcallback)
{
    vignette_->Clear();
    vignette_->Activate();

    drawcallback();

    vignette_->Deactivate();

    vignetteConstants_->Activate(0);
    renderer_->Draw(vignette_->GetColorMap().GetAddressOf(), 0, 1, vignettePS_.Get());
}
