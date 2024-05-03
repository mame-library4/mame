#include "Bloom.h"
#include "Graphics.h"

#if 0
D3D11_RASTERIZER_DESC rasterizerDesc{};
rasterizerDesc.FillMode = D3D11_FILL_SOLID;
rasterizerDesc.CullMode = D3D11_CULL_BACK;
rasterizerDesc.FrontCounterClockwise = FALSE;
rasterizerDesc.DepthBias = 0;
rasterizerDesc.DepthBiasClamp = 0;
rasterizerDesc.SlopeScaledDepthBias = 0;
rasterizerDesc.DepthClipEnable = TRUE;
rasterizerDesc.ScissorEnable = FALSE;
rasterizerDesc.MultisampleEnable = FALSE;
rasterizerDesc.AntialiasedLineEnable = FALSE;
hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
depthStencilDesc.DepthEnable = FALSE;
depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

D3D11_BLEND_DESC blendDesc{};
blendDesc.AlphaToCoverageEnable = FALSE;
blendDesc.IndependentBlendEnable = FALSE;
blendDesc.RenderTarget[0].BlendEnable = FALSE;
blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
hr = device->CreateBlendState(&blendDesc, blendState.GetAddressOf());
_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#endif
#if 0 
void Bloom::Make(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* colorMap)
{
    // 現在の状態を保存する
    ID3D11ShaderResourceView* nullShaderResourceView{};
    ID3D11ShaderResourceView* cachedShaderResourceViews[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
    deviceContext->PSGetShaderResources(0, downsampledCount, cachedShaderResourceViews);

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> cachedDepthStencilState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> cahedRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11BlendState> cachedBlendState;
    FLOAT blendFactor[4];
    UINT sampleMask;
    deviceContext->OMGetDepthStencilState(cachedDepthStencilState.GetAddressOf(), 0);
    deviceContext->RSGetState(cahedRasterizerState.GetAddressOf());
    deviceContext->OMGetBlendState(cachedBlendState.GetAddressOf(), blendFactor, &sampleMask);

    Microsoft::WRL::ComPtr<ID3D11Buffer> cachedConstantBuffer;
    deviceContext->PSGetConstantBuffers(8, 1, cachedConstantBuffer.GetAddressOf());

    // Bind states
    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    deviceContext->RSSetState(rasterizerState.Get());
    deviceContext->OMSetBlendState(blendState.Get(), nullptr, 0xFFFFFFFF);

    BloomConstants data{};
    data.bloomExtractionThreshold = bloomExtractionThreshold;
    data.bloomIntensity = bloomIntensity;
    deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
    deviceContext->PSSetConstantBuffers(8, 1, constantBuffer.GetAddressOf());

    // 明るい色を抽出する
    glowExtraction->Clear(deviceContext, 0, 0, 0, 1);
    glowExtraction->Activate(deviceContext);
    bitBlockTransfer->Blit(deviceContext, &colorMap, 0, 1, glowExtractionPs.Get());
    glowExtraction->Deactivate(deviceContext);
    deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

    // Gaussian blur
    gaussianBlur[0][0]->Clear(deviceContext, 0, 0, 0, 1);
    gaussianBlur[0][0]->Activate(deviceContext);
    bitBlockTransfer->Blit(deviceContext, glowExtraction->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurDownsamplingPs.Get());
    gaussianBlur[0][0]->Deactivate(deviceContext);
    deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

    // Ping-pong gaussian blur
    gaussianBlur[0][1]->Clear(deviceContext, 0, 0, 0, 1);
    gaussianBlur[0][1]->Activate(deviceContext);
    bitBlockTransfer->Blit(deviceContext, gaussianBlur[0][0]->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurHorizontalPs.Get());
    gaussianBlur[0][1]->Deactivate(deviceContext);
    deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

    gaussianBlur[0][0]->Clear(deviceContext, 0, 0, 0, 1);
    gaussianBlur[0][0]->Activate(deviceContext);
    bitBlockTransfer->Blit(deviceContext, gaussianBlur[0][1]->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurVerticalPs.Get());
    gaussianBlur[0][0]->Deactivate(deviceContext);
    deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

    for (size_t downsampledIndex = 1; downsampledIndex < downsampledCount; ++downsampledIndex)
    {
        // Downsampling
        gaussianBlur[downsampledIndex][0]->Clear(deviceContext, 0, 0, 0, 1);
        gaussianBlur[downsampledIndex][0]->Activate(deviceContext);
        bitBlockTransfer->Blit(deviceContext, gaussianBlur[downsampledIndex - 1][0]->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurDownsamplingPs.Get());
        gaussianBlur[downsampledIndex][0]->Deactivate(deviceContext);
        deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

        // Ping-pong gaussian blur
        gaussianBlur[downsampledIndex][1]->Clear(deviceContext, 0, 0, 0, 1);
        gaussianBlur[downsampledIndex][1]->Activate(deviceContext);
        bitBlockTransfer->Blit(deviceContext, gaussianBlur[downsampledIndex][0]->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurHorizontalPs.Get());
        gaussianBlur[downsampledIndex][1]->Deactivate(deviceContext);
        deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

        gaussianBlur[downsampledIndex][0]->Clear(deviceContext, 0, 0, 0, 1);
        gaussianBlur[downsampledIndex][0]->Activate(deviceContext);
        bitBlockTransfer->Blit(deviceContext, gaussianBlur[downsampledIndex][1]->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurVerticalPs.Get());
        gaussianBlur[downsampledIndex][0]->Deactivate(deviceContext);
        deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);
    }

    // Downsampling
    glowExtraction->Clear(deviceContext, 0, 0, 0, 1);
    glowExtraction->Activate(deviceContext);
    std::vector<ID3D11ShaderResourceView*> shaderResourceViews;
    for (size_t downsampledIndex = 0; downsampledIndex < downsampledCount; ++downsampledIndex)
    {
        shaderResourceViews.push_back(gaussianBlur[downsampledIndex][0]->shaderResourceViews[0].Get());
    }
    bitBlockTransfer->Blit(deviceContext, shaderResourceViews.data(), 0, downsampledCount, gaussianBlurUpsamplingPs.Get());
    glowExtraction->Deactivate(deviceContext);
    deviceContext->PSSetShaderResources(0, 1, &nullShaderResourceView);

    // 状態を復元する
    deviceContext->PSSetConstantBuffers(8, 1, cachedConstantBuffer.GetAddressOf());

    deviceContext->OMSetDepthStencilState(cachedDepthStencilState.Get(), 0);
    deviceContext->RSSetState(cahedRasterizerState.Get());
    deviceContext->OMSetBlendState(cachedBlendState.Get(), blendFactor, sampleMask);

    deviceContext->PSSetShaderResources(0, downsampledCount, cachedShaderResourceViews);
    for (ID3D11ShaderResourceView* cachedShaderResourceView : cachedShaderResourceViews)
    {
        if (cachedShaderResourceView) cachedShaderResourceView->Release();
    }
}
#endif


// ----- コンストラクタ -----
Bloom::Bloom()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    // ピクセルシェーダーオブジェクト生成
    CreatePsFromCso(device, "./Resources/Shader/LuminanceExtractionPS.cso", luminancePS_.GetAddressOf());
    CreatePsFromCso(device, "./Resources/Shader/GaussianBlurHorizontalPS.cso", gaussianBlurHorizontalPS_.GetAddressOf());
    CreatePsFromCso(device, "./Resources/Shader/GaussianBlurVerticalPS.cso", gaussianBlurVerticalPS_.GetAddressOf());
    CreatePsFromCso(device, "./Resources/Shader/BloomPS.cso", bloomPS_.GetAddressOf());

    // FrameBufferなど生成
    renderer_ = std::make_unique<FullscreenQuad>(device);
    const uint32_t width = 1280;
    const uint32_t height = 720;
    luminanceExtraction_ = std::make_unique<FrameBuffer>(device, width, height);
    int shiftNum = 1;
    for (int downSamplingIndex = 0; downSamplingIndex < maxBlurBufferCount_; downSamplingIndex += 2)
    {
        // 横ダウンサンプリング
        gaussianBlur_[downSamplingIndex] = std::make_unique<FrameBuffer>(
            device, width >> shiftNum, height >> max(0, shiftNum - 1));

        // 縦ダウンサンプリング
        gaussianBlur_[downSamplingIndex + 1] = std::make_unique<FrameBuffer>(
            device, width >> shiftNum, height >> shiftNum);

        ++shiftNum;
    }
    bloom_ = std::make_unique<FrameBuffer>(device, width, height);

    // 定数バッファー作成
    constantBuffer_ = std::make_unique<ConstantBuffer<BloomConstants>>(device);
}

// ----- bloom実行 -----
void Bloom::Execute(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* colorMap)
{
    // 定数バッファー更新
    constantBuffer_->data.bloomExtractionThreshold_ = bloomConstants_.bloomExtractionThreshold_;
    constantBuffer_->data.bloomIntensity_ = bloomConstants_.bloomIntensity_;
    constantBuffer_->Activate(deviceContext, 0);

    // 輝度を抽出
    luminanceExtraction_->Clear(deviceContext);
    luminanceExtraction_->Activate(deviceContext);
    renderer_->Draw(deviceContext, &colorMap, 0, 1, luminancePS_.Get());
    luminanceExtraction_->Deactivate(deviceContext);

    // ブラー ( ダウンサンプリング )
    for (int downSamplingIndex = 0; downSamplingIndex < maxBlurBufferCount_; downSamplingIndex += 2)
    {
        // 横方向にブラー ( 横方向 ダウンサンプリング )
        gaussianBlur_[downSamplingIndex]->Clear(deviceContext);
        gaussianBlur_[downSamplingIndex]->Activate(deviceContext);
        renderer_->Draw(deviceContext, luminanceExtraction_->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurHorizontalPS_.Get());
        gaussianBlur_[downSamplingIndex]->Deactivate(deviceContext);

        // 縦方向にブラー ( 縦方向 ダウンサンプリング )
        gaussianBlur_[downSamplingIndex + 1]->Clear(deviceContext);
        gaussianBlur_[downSamplingIndex + 1]->Activate(deviceContext);
        renderer_->Draw(deviceContext, gaussianBlur_[downSamplingIndex]->shaderResourceViews[0].GetAddressOf(), 0, 1, gaussianBlurVerticalPS_.Get());
        gaussianBlur_[downSamplingIndex + 1]->Deactivate(deviceContext);
    }

    // ブルーム実行
    ID3D11ShaderResourceView* shaderResourceViews[] =
    {
        colorMap,
        gaussianBlur_[1]->shaderResourceViews[0].Get(),
        gaussianBlur_[3]->shaderResourceViews[0].Get(),
        gaussianBlur_[5]->shaderResourceViews[0].Get(),
        gaussianBlur_[7]->shaderResourceViews[0].Get(),
    };
    bloom_->Clear(deviceContext);
    bloom_->Activate(deviceContext);
    renderer_->Draw(deviceContext, shaderResourceViews, 0, 5, bloomPS_.Get());
    bloom_->Deactivate(deviceContext);
}

// ----- ImGui用 -----
void Bloom::DrawDebug()
{
    if (ImGui::TreeNode("Bloom"))
    {
        ImGui::DragFloat("ExtractionThreshold", &bloomConstants_.bloomExtractionThreshold_, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Intencity", &bloomConstants_.bloomIntensity_, 0.01f, 0.0f, 1.0f);

        if (ImGui::TreeNode("Textures"))
        {
            ImGui::Image(reinterpret_cast<ImTextureID>(luminanceExtraction_->shaderResourceViews[0].Get()), ImVec2(256.0, 256.0));
            ImGui::Image(reinterpret_cast<ImTextureID>(gaussianBlur_[1]->shaderResourceViews[0].Get()), ImVec2(256.0, 256.0));
            ImGui::Image(reinterpret_cast<ImTextureID>(gaussianBlur_[3]->shaderResourceViews[0].Get()), ImVec2(256.0, 256.0));
            ImGui::Image(reinterpret_cast<ImTextureID>(gaussianBlur_[5]->shaderResourceViews[0].Get()), ImVec2(256.0, 256.0));
            ImGui::Image(reinterpret_cast<ImTextureID>(gaussianBlur_[7]->shaderResourceViews[0].Get()), ImVec2(256.0, 256.0));
            ImGui::Image(reinterpret_cast<ImTextureID>(bloom_->shaderResourceViews[0].Get()), ImVec2(256.0, 256.0));

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}
