#include "shader.h"
#include <fstream>
#include "Graphics.h"
#include "Misc.h"
#include "Application.h"

// ----- コンストラクタ -----
Shader::Shader()
{
    // ブレンドステート作成
    CreateBlendStates();

    // ラスタライザーステート作成
    CreateRasterizerStates();

    // 深度ステンシルステート作成
    CreateDepthStencilStates();

    // サンプラステート作成
    CreateSamplerStates();

    // G-Buffer作成
    CreateGBuffer();

    HRESULT result = S_OK;
    D3D11_TEXTURE2D_DESC texture2dDesc = {};
    texture2dDesc.Width = SCREEN_WIDTH;
    texture2dDesc.Height = SCREEN_HEIGHT;
    texture2dDesc.MipLevels = 1;
    texture2dDesc.ArraySize = 1;
    texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texture2dDesc.SampleDesc.Count = 1;
    texture2dDesc.SampleDesc.Quality = 0;
    texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
    texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    texture2dDesc.CPUAccessFlags = 0;
    texture2dDesc.MiscFlags = 0;
    result = Graphics::Instance().GetDevice()->CreateTexture2D(&texture2dDesc, NULL, gBufferDepthStencilBuffer_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Flags = 0;
    result = Graphics::Instance().GetDevice()->CreateDepthStencilView(gBufferDepthStencilBuffer_.Get(), &depthStencilViewDesc, gBufferDepthStencilView_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    D3D11_SHADER_RESOURCE_VIEW_DESC depthShaderResourceViewDesc = {};
    depthShaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    depthShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    depthShaderResourceViewDesc.Texture2D.MipLevels = 1;
    result = Graphics::Instance().GetDevice()->CreateShaderResourceView(gBufferDepthStencilBuffer_.Get(), &depthShaderResourceViewDesc, gBufferDepthShaderResourceView_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
}

// ----- ImGui用 -----
void Shader::DrawDebug()
{
    if (ImGui::TreeNode("G-Buffer"))
    {
        for (int i = 0; i < static_cast<int>(GBufferId::Max); ++i)
        {
            ImGui::Image(reinterpret_cast<ImTextureID>(gBufferShaderResourceView_[i].Get()), ImVec2(256.0, 256.0));
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("camera"))
    {
        ImGui::SliderFloat("wight", &view.viewWitdh,1.0f,10000.0f);
        ImGui::SliderFloat("height", &view.viewHeight, 1.0f, 10000.0f);
        ImGui::SliderFloat("nearZ", &view.nearZ, 1.0f, 10000.0f);
        ImGui::SliderFloat("farZ", &view.farZ, 1.0f, 10000.0f);

        ImGui::DragFloat4("positon", &view.position.x);
        ImGui::DragFloat4("camera", &view.camera.x);

        ImGui::TreePop();
    }
}

// ----- 頂点シェーダー作成 -----
HRESULT Shader::CreateVsFromCso(const char* csoName, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements)
{
    HRESULT         result = S_OK;
    ID3D11Device*   device = Graphics::Instance().GetDevice();

    FILE* fp = nullptr;
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long cso_sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]>cso_data = std::make_unique<unsigned char[]>(cso_sz);
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);
        
    result = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertexShader);
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    if (inputLayout)
    {
        result = device->CreateInputLayout(inputElementDesc, numElements, cso_data.get(), cso_sz, inputLayout);
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
    }

    return result;
}

// ----- ピクセルシェーダー作成 -----
HRESULT Shader::CreatePsFromCso(const char* csoName, ID3D11PixelShader** pixelShader)
{
    HRESULT         result = S_OK;
    ID3D11Device*   device = Graphics::Instance().GetDevice();

    FILE* fp = nullptr;
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long cso_sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]>cso_data = std::make_unique<unsigned char[]>(cso_sz);
    fread(cso_data.get(), cso_sz, 1, fp);
    fclose(fp);

    result = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixelShader);
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    return result;
}

// ----- ジオメトリシェーダー作成 -----
HRESULT Shader::CreateGsFromCso(const char* csoName, ID3D11GeometryShader** geometryShader)
{
    HRESULT         result = S_OK;
    ID3D11Device*   device = Graphics::Instance().GetDevice();

    FILE* fp = nullptr;
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long csoSz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> csoData = std::make_unique<unsigned char[]>(csoSz);
    fread(csoData.get(), csoSz, 1, fp);
    fclose(fp);

    result = device->CreateGeometryShader(csoData.get(), csoSz, nullptr, geometryShader);
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    return result;
}

// ----- コンピュートシェーダー作成 -----
HRESULT Shader::CreateCsFromCso(const char* csoName, ID3D11ComputeShader** computeShader)
{
    HRESULT         result = S_OK;
    ID3D11Device*   device = Graphics::Instance().GetDevice();

    FILE* fp = nullptr;
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long csoSz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> csoData = std::make_unique<unsigned char[]>(csoSz);
    fread(csoData.get(), csoSz, 1, fp);
    fclose(fp);

    result = device->CreateComputeShader(csoData.get(), csoSz, nullptr, computeShader);
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    return result;
}

// ----- ドメインシェーダー作成 -----
HRESULT Shader::CreateDsFromCso(const char* csoName, ID3D11DomainShader** domainShader)
{
    HRESULT         result = S_OK;
    ID3D11Device*   device = Graphics::Instance().GetDevice();

    FILE* fp = nullptr;
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long csoSz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> csoData = std::make_unique<unsigned char[]>(csoSz);
    fread(csoData.get(), csoSz, 1, fp);
    fclose(fp);

    result = device->CreateDomainShader(csoData.get(), csoSz, nullptr, domainShader);
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    return result;
}

// ----- ハルシェーダー作成 -----
HRESULT Shader::CreateHsFromCso(const char* csoName, ID3D11HullShader** hullShader)
{
    HRESULT         result = S_OK;
    ID3D11Device*   device = Graphics::Instance().GetDevice();

    FILE* fp = nullptr;
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO File not found");

    fseek(fp, 0, SEEK_END);
    long csoSz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]> csoData = std::make_unique<unsigned char[]>(csoSz);
    fread(csoData.get(), csoSz, 1, fp);
    fclose(fp);

    result = device->CreateHullShader(csoData.get(), csoSz, nullptr, hullShader);
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    return result;
}

// ----- ブレンドステート設定 -----
void Shader::SetBlendState(const BLEND_STATE& blendState)
{
    Graphics::Instance().GetDeviceContext()->OMSetBlendState(blendStates_[static_cast<UINT>(blendState)].Get(), nullptr, 0xFFFFFFFF);
}

// ----- ラスタライザステート設定 -----
void Shader::SetRasterizerState(const RASTER_STATE& rasterizerState)
{
    Graphics::Instance().GetDeviceContext()->RSSetState(rasterizerStates_[static_cast<UINT>(rasterizerState)].Get());
}

// ----- デプスステンシルステート設定 -----
void Shader::SetDepthStencileState(const DEPTH_STATE& depthStencileState)
{
    Graphics::Instance().GetDeviceContext()->OMSetDepthStencilState(depthStencilStates_[static_cast<int>(depthStencileState)].Get(), 1);
}

// ----- サンプラーステート設定 -----
void Shader::SetSamplerState()
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    deviceContext->PSSetSamplers(0, 1, samplerState[static_cast<size_t>(SAMPLER_STATE::POINT)].GetAddressOf());
    deviceContext->PSSetSamplers(1, 1, samplerState[static_cast<size_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
    deviceContext->PSSetSamplers(2, 1, samplerState[static_cast<size_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
    deviceContext->PSSetSamplers(3, 1, samplerState[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_BLACK)].GetAddressOf());
    deviceContext->PSSetSamplers(4, 1, samplerState[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_WHITE)].GetAddressOf());
    deviceContext->PSSetSamplers(5, 1, samplerState[static_cast<size_t>(SAMPLER_STATE::COMPARISON)].GetAddressOf());
}

// ----- G-Bufferを有効化する -----
void Shader::SetGBuffer()
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    ID3D11RenderTargetView* renderTargets[static_cast<int>(GBufferId::Max)] =
    {
        gBufferRenderTargetView_[static_cast<int>(GBufferId::BaseColor)].Get(),
        gBufferRenderTargetView_[static_cast<int>(GBufferId::Emissive)].Get(),
        gBufferRenderTargetView_[static_cast<int>(GBufferId::Normal)].Get(),
        gBufferRenderTargetView_[static_cast<int>(GBufferId::Parameters)].Get(),
        gBufferRenderTargetView_[static_cast<int>(GBufferId::Depth)].Get(),
    };
    FLOAT clearColor[] = { 0, 0, 0, 0 };
    for (int i = 0; i < static_cast<int>(GBufferId::Max); ++i)
    {
        deviceContext->ClearRenderTargetView(renderTargets[i], clearColor);
    }
    deviceContext->ClearDepthStencilView(gBufferDepthStencilView_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    deviceContext->OMSetRenderTargets(static_cast<int>(GBufferId::Max), renderTargets, gBufferDepthStencilView_.Get());
}

void Shader::SetGBufferShaderResourceView()
{
    ID3D11DeviceContext* deviceContext = Graphics::Instance().GetDeviceContext();

    deviceContext->PSSetShaderResources(1, 1, gBufferShaderResourceView_[1].GetAddressOf());
    deviceContext->PSSetShaderResources(2, 1, gBufferShaderResourceView_[2].GetAddressOf());
    deviceContext->PSSetShaderResources(3, 1, gBufferShaderResourceView_[3].GetAddressOf());
    deviceContext->PSSetShaderResources(4, 1, gBufferShaderResourceView_[4].GetAddressOf());
}

void Shader::SetDepthBuffer()
{
    //Graphics::Instance().GetDeviceContext()->OMSetDepthStencilState()
    //Graphics::Instance().GetDeviceContext()->ClearDepthStencilView(gBufferShaderResourceView_[4].GetAddressOf())
}

// ----- ブレンドステート作成 -----
void Shader::CreateBlendStates()
{
    HRESULT             result      = S_OK;
    ID3D11Device*       device      = Graphics::Instance().GetDevice();
    D3D11_BLEND_DESC    blendDesc   = {};

    blendDesc.AlphaToCoverageEnable                 = FALSE;
    blendDesc.IndependentBlendEnable                = FALSE;
    blendDesc.RenderTarget[0].BlendEnable           = FALSE;
    blendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    result = device->CreateBlendState(&blendDesc, blendStates_[static_cast<int>(BLEND_STATE::NONE)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    blendDesc.AlphaToCoverageEnable                 = FALSE;
    blendDesc.IndependentBlendEnable                = FALSE;
    blendDesc.RenderTarget[0].BlendEnable           = TRUE;
    blendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    result = device->CreateBlendState(&blendDesc, blendStates_[static_cast<int>(BLEND_STATE::ALPHA)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    blendDesc.AlphaToCoverageEnable                 = FALSE;
    blendDesc.IndependentBlendEnable                = FALSE;
    blendDesc.RenderTarget[0].BlendEnable           = TRUE;
    blendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    result = device->CreateBlendState(&blendDesc, blendStates_[static_cast<int>(BLEND_STATE::ADD)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    blendDesc.AlphaToCoverageEnable                 = FALSE;
    blendDesc.IndependentBlendEnable                = FALSE;
    blendDesc.RenderTarget[0].BlendEnable           = TRUE;
    blendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ZERO; 
    blendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_SRC_COLOR;
    blendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_DEST_ALPHA;
    blendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    result = device->CreateBlendState(&blendDesc, blendStates_[static_cast<int>(BLEND_STATE::MULTIPLY)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;
    for (int i = 0; i < static_cast<int>(GBufferId::Max); ++i)
    {
        blendDesc.RenderTarget[i].BlendEnable           = FALSE;
        blendDesc.RenderTarget[i].SrcBlend              = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].DestBlend             = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[i].BlendOp               = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].SrcBlendAlpha         = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[i].DestBlendAlpha        = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[i].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }
    result = device->CreateBlendState(&blendDesc, blendStates_[static_cast<int>(BLEND_STATE::MRT)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
}

// ----- ラスタライザステート作成 -----
void Shader::CreateRasterizerStates()
{
    HRESULT                 result          = S_OK;
    ID3D11Device*           device          = Graphics::Instance().GetDevice();
    D3D11_RASTERIZER_DESC   rasterizerDesc  = {};

    rasterizerDesc.FillMode                 = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode                 = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise    = TRUE;
    rasterizerDesc.DepthBias                = 0;
    rasterizerDesc.DepthBiasClamp           = 0;
    rasterizerDesc.SlopeScaledDepthBias     = 0;
    rasterizerDesc.DepthClipEnable          = TRUE;
    rasterizerDesc.ScissorEnable            = FALSE;
    rasterizerDesc.MultisampleEnable        = FALSE;
    rasterizerDesc.AntialiasedLineEnable    = FALSE;
    result = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates_[static_cast<size_t>(RASTER_STATE::SOLID)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    rasterizerDesc.FillMode                 = D3D11_FILL_WIREFRAME;
    rasterizerDesc.CullMode                 = D3D11_CULL_BACK;
    rasterizerDesc.AntialiasedLineEnable    = TRUE;
    result = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates_[static_cast<size_t>(RASTER_STATE::WIREFRAME)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    rasterizerDesc.FillMode                 = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode                 = D3D11_CULL_NONE;
    rasterizerDesc.AntialiasedLineEnable    = TRUE;
    result = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates_[static_cast<size_t>(RASTER_STATE::CULL_NONE)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    rasterizerDesc.FillMode                 = D3D11_FILL_WIREFRAME;
    rasterizerDesc.CullMode                 = D3D11_CULL_NONE;
    rasterizerDesc.AntialiasedLineEnable    = TRUE;
    result = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates_[static_cast<size_t>(RASTER_STATE::WIREFRAME_CULL_NONE)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
}

// ----- デプスステンシルステート作成 -----
void Shader::CreateDepthStencilStates()
{
    HRESULT                     result              = S_OK;
    ID3D11Device*               device              = Graphics::Instance().GetDevice();
    D3D11_DEPTH_STENCIL_DESC    depthStencilDesc    = {};

    // 深度テスト：オン　深度ライト：オン
    depthStencilDesc.DepthEnable    = TRUE;	                        // 深度テストを有効にする
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;   // 深度ステンシル バッファーへの書き込みを有効にする
    depthStencilDesc.DepthFunc      = D3D11_COMPARISON_LESS_EQUAL;  // 深度データと既存データを比較する関数
    result = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[static_cast<int>(DEPTH_STATE::ZT_ON_ZW_ON)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    // 深度テスト：オン　深度ライト：オフ
    depthStencilDesc.DepthEnable    = TRUE;                         // 深度テストを有効にする
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // 深度ステンシル バッファーへの書き込みを無効にする
    result = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[static_cast<int>(DEPTH_STATE::ZT_ON_ZW_OFF)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    // 深度テスト：オフ　深度ライト：オン
    depthStencilDesc.DepthEnable    = FALSE;                        // 深度テストを無効にする
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;   // 深度ステンシル バッファーへの書き込みを有効にする
    result = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[static_cast<int>(DEPTH_STATE::ZT_OFF_ZW_ON)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    // 深度テスト：オフ　深度ライト：オフ
    depthStencilDesc.DepthEnable    = FALSE;                        // 深度テストを無効にする
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // 深度ステンシル バッファーでの書き込みを無効にする
    result = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[static_cast<int>(DEPTH_STATE::ZT_OFF_ZW_OFF)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
}

// ----- サンプラーステート作成 -----
void Shader::CreateSamplerStates()
{
    HRESULT             result      = S_OK;
    ID3D11Device*       device      = Graphics::Instance().GetDevice();
    D3D11_SAMPLER_DESC  samplerDesc = {};

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    result = device->CreateSamplerState(&samplerDesc, samplerState[static_cast<size_t>(SAMPLER_STATE::POINT)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    result = device->CreateSamplerState(&samplerDesc, samplerState[static_cast<size_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    result = device->CreateSamplerState(&samplerDesc, samplerState[static_cast<size_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    result = device->CreateSamplerState(&samplerDesc, samplerState[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_BLACK)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.BorderColor[0] = 1;
    samplerDesc.BorderColor[1] = 1;
    samplerDesc.BorderColor[2] = 1;
    samplerDesc.BorderColor[3] = 1;
    result = device->CreateSamplerState(&samplerDesc, samplerState[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_WHITE)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

    samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    samplerDesc.BorderColor[0] = 1;
    samplerDesc.BorderColor[1] = 1;
    samplerDesc.BorderColor[2] = 1;
    samplerDesc.BorderColor[3] = 1;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    result = device->CreateSamplerState(&samplerDesc, samplerState[static_cast<size_t>(SAMPLER_STATE::COMPARISON)].GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
}

// ----- G-Buffer作成 -----
void Shader::CreateGBuffer()
{
    HRESULT result = S_OK;

    D3D11_TEXTURE2D_DESC texture2dDesc = {};
    texture2dDesc.Width = SCREEN_WIDTH;
    texture2dDesc.Height = SCREEN_HEIGHT;
    texture2dDesc.MipLevels = 1;
    texture2dDesc.ArraySize = 1;
    texture2dDesc.SampleDesc.Count = 1;
    texture2dDesc.SampleDesc.Quality = 0;
    texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
    texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture2dDesc.CPUAccessFlags = 0;
    texture2dDesc.MiscFlags = 0;

    DXGI_FORMAT formats[] =
    {
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_R32G32B32A32_FLOAT,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_FORMAT_R32_FLOAT
    };

    for (int i = 0; i < static_cast<int>(GBufferId::Max); ++i)
    {
        texture2dDesc.Format = formats[i];

        Microsoft::WRL::ComPtr<ID3D11Texture2D> colorBuffer = {};
        result = Graphics::Instance().GetDevice()->CreateTexture2D(&texture2dDesc, NULL, colorBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

        // RenderTargetView生成
        result = Graphics::Instance().GetDevice()->CreateRenderTargetView(colorBuffer.Get(), NULL, gBufferRenderTargetView_[i].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));

        // ShaderResourceView生成
        result = Graphics::Instance().GetDevice()->CreateShaderResourceView(colorBuffer.Get(), NULL, gBufferShaderResourceView_[i].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(result), HRTrace(result));
    }

    CreatePsFromCso("./Resources/Shader/GltfModelGBufferPS.cso", gBufferPixelShader_.GetAddressOf());
}